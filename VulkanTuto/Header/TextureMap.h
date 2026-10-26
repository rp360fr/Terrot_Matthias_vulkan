#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#include "Instance.h"
#include "Pipeline.h"
#include "Device.h"
#include "SwapChain.h"
#include  "VertexBuffer.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>

class VertexBuffer;
class Device;
class Pipeline;
class TextureMap
{
public:
    void createtextureImage(VertexBuffer &vertexBuffer,Device& device,Pipeline& pipeline);
    vk::raii::CommandBuffer beginSingleTimeCommands(Device& device,Pipeline& pipeline);
    void endSingleTimeCommands(vk::raii::CommandBuffer &&commandBuffer,Device& device);
  
    void createTextureImageView(Device& device);
    void createTextureSampler(Device& device);
  vk::raii::ImageView createImageView(Device& device,vk::Image const &image, vk::Format format,vk::ImageAspectFlags aspectFlags);
    void transitionImageLayout(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer, const vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width, uint32_t height);
    
    std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(Device& device,VertexBuffer& vertexBuffer,
    
  uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties );
    
    vk::raii::Sampler* GetTExtureSampler()
    {
        return &textureSampler;
    }
    vk::raii::ImageView *  GetTextureImageView()
    {
        return &textureImageView;
    }
private:
    vk::raii::Image        textureImage       = nullptr;
    vk::raii::ImageView    textureImageView   = nullptr;
    vk::raii::DeviceMemory textureImageMemory = nullptr;
    
    vk::raii::Sampler      textureSampler     = nullptr;
};
