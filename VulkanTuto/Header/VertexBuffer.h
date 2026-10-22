#pragma once
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <GLFW/glfw3.h> 
#include "Device.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

class VertexBuffer
{
public:
    
  
    glm::vec2 pos;
    glm::vec3 color;
		void CreateVertexBuffer();
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags property_flags,Device& device);
    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {
            .binding = 0,
            .stride = sizeof(VertexBuffer),
            .inputRate = vk::VertexInputRate::eVertex
        };
        
        
    }
    
    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        return {{{.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(VertexBuffer, pos)},
                 {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(VertexBuffer, color)}}};
    }
private:
    vk::raii::Buffer vertexBuffer = nullptr;
    vk::MemoryRequirements memRequirements = vertexBuffer.getMemoryRequirements();
    
    const std::vector<VertexBuffer> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
};

