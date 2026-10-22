#pragma once
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <GLFW/glfw3.h> 
#include "Device.h"
#include "pipeline.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
class Pipeline;
struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
    
    
    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex
        };
        
        
    }
    
    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        return {{{.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)},
                 {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)}}};
    }
};
class VertexBuffer
{
public:
    
   Vertex vertex;
    
		void CreateVertexBuffer(Device& device,Pipeline& pipeline);
    void CreateIndexBuffer(Device& device,Pipeline& pipeline);
    void copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size,Device& device,Pipeline& pipeline);
    std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> CreateBuffer(Device& ,vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags property_flags,Device& device);
    
    
    
    
    vk::raii::Buffer* GetvertexBuffer()
    {
        return &vertexBuffer;
    }
    
   
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    vk::raii::Buffer* GetindixBuffer()
    {
        return &indexBuffer;
    }
private:
    vk::raii::Buffer vertexBuffer = nullptr;
    
    
    vk::raii::Buffer       indexBuffer        = nullptr;
    vk::raii::DeviceMemory indexBufferMemory  = nullptr;
    vk::raii::DeviceMemory vertexBufferMemory = nullptr;
    
    
};

