#include "Header/TextureMap.h"
#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"
void TextureMap::createtextureImage(VertexBuffer &vertexBuffer,Device& device,Pipeline& pipeline)
{
    int            texWidth, texHeight, texChannels;
    stbi_uc       *pixels    = stbi_load("Texture/Texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    auto [stagingBuffer, stagingBufferMemory] =
        vertexBuffer.CreateBuffer(device,imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *data = stagingBufferMemory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBufferMemory.unmapMemory();

    stbi_image_free(pixels);

    std::tie(textureImage, textureImageMemory) = createImage(device,vertexBuffer,texWidth,
                                                             texHeight,
                                                             vk::Format::eR8G8B8A8Srgb,
                                                             vk::ImageTiling::eOptimal,
                                                             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                                                             vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::raii::CommandBuffer commandBuffer = beginSingleTimeCommands(device,pipeline);
    transitionImageLayout(commandBuffer, textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(commandBuffer, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(commandBuffer, textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    endSingleTimeCommands(std::move(commandBuffer),device);
    
}

std::pair<vk::raii::Image, vk::raii::DeviceMemory> TextureMap::createImage(Device& device,VertexBuffer& vertexBuffer,
  uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties )
{
    vk::ImageCreateInfo imageInfo{.imageType   = vk::ImageType::e2D,
                                  .format      = format,
                                  .extent      = {width, height, 1},
                                  .mipLevels   = 1,
                                  .arrayLayers = 1,
                                  .samples     = vk::SampleCountFlagBits::e1,
                                  .tiling      = tiling,
                                  .usage       = usage,
                                  .sharingMode = vk::SharingMode::eExclusive};

    vk::raii::Image image = vk::raii::Image(*device.GetDevice(), imageInfo);

    vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{.allocationSize  = memRequirements.size,
                                     .memoryTypeIndex = vertexBuffer.findMemoryType(memRequirements.memoryTypeBits, properties,device)};
    vk::raii::DeviceMemory imageMemory = vk::raii::DeviceMemory(*device.GetDevice(), allocInfo);
    image.bindMemory(imageMemory, 0);

    return {std::move(image), std::move(imageMemory)};
}

vk::raii::CommandBuffer TextureMap::beginSingleTimeCommands(Device& device,Pipeline& pipeline)
{
    vk::CommandBufferAllocateInfo allocInfo{.commandPool = *pipeline.GetcommandPoll(), .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};
    vk::raii::CommandBuffer       commandBuffer = std::move(vk::raii::CommandBuffers(*device.GetDevice(), allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(beginInfo);

    return std::move(commandBuffer);
}

void TextureMap::endSingleTimeCommands(vk::raii::CommandBuffer &&commandBuffer,Device& device)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo{.commandBufferCount = 1, .pCommandBuffers = &*commandBuffer};
   device.GetQueue()->submit(submitInfo, nullptr);
    device.GetQueue()->waitIdle();
}



void TextureMap::createTextureImageView(Device& device)
{
    textureImageView = createImageView(device,*textureImage, vk::Format::eR8G8B8A8Srgb,vk::ImageAspectFlagBits::eColor);
    
}

void TextureMap::createTextureSampler(Device& device)
{
    vk::PhysicalDeviceProperties properties = device.GetphysiacalDevice()->getProperties();
    vk::SamplerCreateInfo        samplerInfo{.magFilter        = vk::Filter::eLinear,
                                             .minFilter        = vk::Filter::eLinear,
                                             .mipmapMode       = vk::SamplerMipmapMode::eLinear,
                                             .addressModeU     = vk::SamplerAddressMode::eRepeat,
                                             .addressModeV     = vk::SamplerAddressMode::eRepeat,
                                             .addressModeW     = vk::SamplerAddressMode::eRepeat,
                                             .mipLodBias       = 0.0f,
                                             .anisotropyEnable = vk::True,
                                             .maxAnisotropy    = properties.limits.maxSamplerAnisotropy,
                                             .compareEnable    = vk::False,
                                             .compareOp        = vk::CompareOp::eAlways};
    textureSampler = vk::raii::Sampler(*device.GetDevice(), samplerInfo);
    
}

vk::raii::ImageView TextureMap::createImageView(Device& device,vk::Image const &image, vk::Format format,vk::ImageAspectFlags aspectFlags)
{
    vk::ImageViewCreateInfo viewInfo{
        .image            = image,
        .viewType         = vk::ImageViewType::e2D,
        .format           = format,
        .subresourceRange = {.aspectMask =  aspectFlags, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};
    return vk::raii::ImageView(*device.GetDevice(), viewInfo);
}

void TextureMap::transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image,
                                       vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
   vk::ImageMemoryBarrier barrier{.oldLayout           = oldLayout,
                               .newLayout           = newLayout,
                               .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                               .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                               .image               = image,
                               .subresourceRange    = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1}};
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage      = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
}

void TextureMap::copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer,
    vk::raii::Image& image, uint32_t width, uint32_t height)
{
    vk::BufferImageCopy region{.bufferOffset      = 0,
                           .bufferRowLength   = 0,
                           .bufferImageHeight = 0,
                           .imageSubresource  = {.aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
                           .imageOffset       = {0, 0, 0},
                           .imageExtent       = {width, height, 1}};
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
}
