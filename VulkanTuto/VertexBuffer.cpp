#include "Header/VertexBuffer.h"
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "Header/SwapChain.h"

void VertexBuffer::CreateVertexBuffer(Device& device,Pipeline& pipeline,TextureMap& texture)
{
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, vertices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();
    
    std::tie(vertexBuffer, vertexBufferMemory) =
    CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize,device,pipeline,texture);
}

void VertexBuffer::CreateIndexBuffer(Device& device, Pipeline& pipeline,TextureMap& texture)
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, indices.data(), (size_t) bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(indexBuffer, indexBufferMemory) =
        CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize,device,pipeline,texture);
}

void VertexBuffer::CreateUniformBuffer(Device& device, Pipeline& pipeline)
{
    for (size_t i = 0; i < pipeline.GetMAX_FRAMES_IN_FLIGHT(); i++)
    {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        auto [buffer, bufferMem]  = CreateBuffer(device,
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        uniformBuffers.emplace_back(std::move(buffer));
        uniformBuffersMemory.emplace_back(std::move(bufferMem));
        uniformBuffersMapped.emplace_back( uniformBuffersMemory.back().mapMemory(0, bufferSize));
    }
}

void VertexBuffer::updateUniformBuffers(Device& device,SwapChain& swapchain,uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
  
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj =
    glm::perspective(glm::radians(45.0f), static_cast<float>(swapchain.GetswapChainExtent()->width) / static_cast<float>(swapchain.GetswapChainExtent()->height), 0.1f, 10.0f);
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
void VertexBuffer::createDescriptorSetLayout(Device& device)
{
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings{
        {{.binding = 0, .descriptorType = vk::DescriptorType::eUniformBuffer, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eVertex},
         {.binding = 1, .descriptorType = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eFragment}}};

    vk::DescriptorSetLayoutCreateInfo layoutInfo{.bindingCount = static_cast<uint32_t>(bindings.size()), .pBindings = bindings.data()};
    descriptorSetLayout = vk::raii::DescriptorSetLayout(*device.GetDevice(), layoutInfo);
}

void VertexBuffer::createDescriptorPool(Device& device,Pipeline& pipeline)
{
    std::array<vk::DescriptorPoolSize, 2> poolSize{{{.type = vk::DescriptorType::eUniformBuffer, .descriptorCount = static_cast<uint32_t>(pipeline.GetMAX_FRAMES_IN_FLIGHT())},
                                                {.type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = static_cast<uint32_t>(pipeline.GetMAX_FRAMES_IN_FLIGHT())}}};
    vk::DescriptorPoolCreateInfo          poolInfo{.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                                   .maxSets       = static_cast<uint32_t>(pipeline.GetMAX_FRAMES_IN_FLIGHT()),
                                                   .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
                                                   .pPoolSizes    = poolSize.data()};

    descriptorPool = vk::raii::DescriptorPool(*device.GetDevice(), poolInfo);
}

void VertexBuffer::createDescriptorSets(Device& device, Pipeline& pipeline,TextureMap& texture)
{
    std::vector<vk::DescriptorSetLayout> layouts(pipeline.GetMAX_FRAMES_IN_FLIGHT(), *descriptorSetLayout);
    vk::DescriptorSetAllocateInfo        allocInfo{.descriptorPool     = descriptorPool,
                                                   .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
                                                   .pSetLayouts        = layouts.data()};

    descriptorSets = device.GetDevice()->allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < pipeline.GetMAX_FRAMES_IN_FLIGHT(); i++)
    {
        vk::DescriptorBufferInfo bufferInfo{.buffer = uniformBuffers[i], .offset = 0, .range = sizeof(UniformBufferObject)};
        vk::DescriptorImageInfo  imageInfo{.sampler = *texture.GetTExtureSampler(), .imageView =* texture.GetTextureImageView(), .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};
        std::array<vk::WriteDescriptorSet, 2> descriptorWrites{{{.dstSet          = descriptorSets[i],
                                                          .dstBinding      = 0,
                                                          .dstArrayElement = 0,
                                                          .descriptorCount = 1,
                                                          .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                                          .pBufferInfo     = &bufferInfo},
                                                         {.dstSet          = descriptorSets[i],
                                                          .dstBinding      = 1,
                                                          .dstArrayElement = 0,
                                                          .descriptorCount = 1,
                                                          .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
                                                          .pImageInfo      = &imageInfo}}};
        device.GetDevice()->updateDescriptorSets(descriptorWrites, {});
    }
}

void VertexBuffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size,Device& device,Pipeline& pipeline,TextureMap& texture)
{
    vk::raii::CommandBuffer commandCopyBuffer = texture.beginSingleTimeCommands(device,pipeline);
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy{.size = size});
   texture. endSingleTimeCommands(std::move(commandCopyBuffer),device);
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> VertexBuffer::CreateBuffer(Device& device,vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
    vk::BufferCreateInfo   bufferInfo{.size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
    vk::raii::Buffer       buffer          = vk::raii::Buffer(*device.GetDevice(), bufferInfo);
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{.allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties,device)};
    vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(*device.GetDevice(), allocInfo);
    buffer.bindMemory(*bufferMemory, 0);
    return {std::move(buffer), std::move(bufferMemory)};
}

uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags property_flags,Device& device)
{
    vk::PhysicalDeviceMemoryProperties memProperties = device.GetphysiacalDevice()->getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
    
    
}

void VertexBuffer::createDepthResources(SwapChain& swapChain,TextureMap& texture,Device& device,VertexBuffer& vertexBuffer)
{
    vk::Format depthFormat = findDepthFormat(device);

    std::tie(depthImage, depthImageMemory) = texture.createImage(device,vertexBuffer,swapChain.GetswapChainExtent()->width, swapChain.GetswapChainExtent()->height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
    depthImageView                         = texture.createImageView(device,depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

vk::Format VertexBuffer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
    vk::FormatFeatureFlags features, Device& device)
{
    for (const auto format : candidates)
    {
        vk::FormatProperties props =device.GetphysiacalDevice()->getFormatProperties(format);
        if (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
            ((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)))
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

vk::Format VertexBuffer::findDepthFormat(Device& device)
{
    return findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                                   vk::ImageTiling::eOptimal,
                                   vk::FormatFeatureFlagBits::eDepthStencilAttachment,device);
}
