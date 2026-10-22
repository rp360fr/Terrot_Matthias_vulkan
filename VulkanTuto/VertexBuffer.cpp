#include "Header/VertexBuffer.h"

void VertexBuffer::CreateVertexBuffer(Device& device,Pipeline& pipeline)
{
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, vertices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();
    
    std::tie(vertexBuffer, vertexBufferMemory) =
    CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize,device,pipeline);
}

void VertexBuffer::CreateIndexBuffer(Device& device, Pipeline& pipeline)
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, indices.data(), (size_t) bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(indexBuffer, indexBufferMemory) =
        CreateBuffer(device,bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize,device,pipeline);
}

void VertexBuffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size,Device& device,Pipeline& pipeline)
{
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = *pipeline.GetcommandPoll(),.level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(device.GetDevice()->allocateCommandBuffers(allocInfo).front());
    commandCopyBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
    commandCopyBuffer.end();
    device.GetQueue()->submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer}, nullptr);
   device.GetQueue()->waitIdle();
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
