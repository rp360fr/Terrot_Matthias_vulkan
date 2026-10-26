#pragma once
#include <array>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h> 
#include "Device.h"
#include "pipeline.h"
#include "TextureMap.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
class Pipeline;
class SwapChain;
class TextureMap;
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    
    
    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex
        };
        
        
    }
    
    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        return {
            vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
            vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
            vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord))
        };
    }
};


struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
class VertexBuffer
{
public:
    
   Vertex vertex;
    
		void CreateVertexBuffer(Device& device,Pipeline& pipeline,TextureMap& texture);
    void CreateIndexBuffer(Device& device,Pipeline& pipeline,TextureMap& texture);
    void CreateUniformBuffer(Device& device,Pipeline& pipeline);
    void updateUniformBuffers(Device& device,SwapChain& swapchain,uint32_t currentImage);
    void createDescriptorSetLayout(Device& device);
    void  createDescriptorPool(Device& device,Pipeline& pipeline);
    void createDescriptorSets(Device& device,Pipeline& pipeline,TextureMap& texture);
    void copyBuffer(vk::raii::Buffer & srcBuffer, vk::raii::Buffer & dstBuffer, vk::DeviceSize size,Device& device,Pipeline& pipeline,TextureMap& texture);
    std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> CreateBuffer(Device& ,vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags property_flags,Device& device);
    
    void  createDepthResources(SwapChain& swapChain,TextureMap& texture,Device& device,VertexBuffer& vertexBuffer);
   vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features,Device& device);
    
    
    vk::Format findDepthFormat(Device& device);


    
    vk::raii::Buffer* GetvertexBuffer()
    {
        return &vertexBuffer;
    }
    
   
    const std::vector<Vertex> vertices = {

        {{-0.5f, -0.5f,  0.5f}, {1, 1, 1}, {0, 0}}, // 0
        {{ 0.5f, -0.5f,  0.5f}, {1, 1, 1}, {1, 0}}, // 1
        {{ 0.5f,  0.5f,  0.5f}, {1, 1, 1}, {1, 1}}, // 2
        {{-0.5f,  0.5f,  0.5f}, {1, 1, 1}, {0, 1}}, // 3

        {{ 0.5f, -0.5f, -0.5f}, {1, 1, 1}, {0, 0}}, // 4
        {{-0.5f, -0.5f, -0.5f}, {1, 1, 1}, {1, 0}}, // 5
        {{-0.5f,  0.5f, -0.5f}, {1, 1, 1}, {1, 1}}, // 6
        {{ 0.5f,  0.5f, -0.5f}, {1, 1, 1}, {0, 1}}, // 7

        {{-0.5f, -0.5f, -0.5f}, {1, 1, 1}, {0, 0}}, // 8
        {{-0.5f, -0.5f,  0.5f}, {1, 1, 1}, {1, 0}}, // 9
        {{-0.5f,  0.5f,  0.5f}, {1, 1, 1}, {1, 1}}, // 10
        {{-0.5f,  0.5f, -0.5f}, {1, 1, 1}, {0, 1}}, // 11

        {{ 0.5f, -0.5f,  0.5f}, {1, 1, 1}, {0, 0}}, // 12
        {{ 0.5f, -0.5f, -0.5f}, {1, 1, 1}, {1, 0}}, // 13
        {{ 0.5f,  0.5f, -0.5f}, {1, 1, 1}, {1, 1}}, // 14
        {{ 0.5f,  0.5f,  0.5f}, {1, 1, 1}, {0, 1}}, // 15

        {        {-0.5f,  0.5f,  0.5f}, {1, 1, 1}, {0, 0}}, // 16
        {{ 0.5f,  0.5f,  0.5f}, {1, 1, 1}, {1, 0}}, // 17
        {{ 0.5f,  0.5f, -0.5f}, {1, 1, 1}, {1, 1}}, // 18
        {{-0.5f,  0.5f, -0.5f}, {1, 1, 1}, {0, 1}}, // 19

        {{-0.5f, -0.5f, -0.5f}, {1, 1, 1}, {0, 0}}, // 20
        {{ 0.5f, -0.5f, -0.5f}, {1, 1, 1}, {1, 0}}, // 21
        {{ 0.5f, -0.5f,  0.5f}, {1, 1, 1}, {1, 1}}, // 22
        {{-0.5f, -0.5f,  0.5f}, {1, 1, 1}, {0, 1}}  // 23
    };

    const std::vector<uint32_t> indices = {
        0,  1,  2,   0,  2,  3,   
        4,  5,  6,   4,  6,  7,   
        8,  9, 10,   8, 10, 11,   
       12, 13, 14,  12, 14, 15,   
       16, 17, 18,  16, 18, 19,   
       20, 21, 22,  20, 22, 23    
   };
    vk::raii::Buffer* GetindixBuffer()
    {
        return &indexBuffer;
    }
    vk::raii::DescriptorSetLayout* GetdescriptorSetLayout(){return &descriptorSetLayout;}
    std::vector<vk::raii::DescriptorSet>* GetdescriptorSets(){return &descriptorSets;}
    std::vector<vk::raii::DescriptorSet> descriptorSets;
    
    vk::raii::Image *       GetdepthImage (){return &depthImage;}
    vk::raii::ImageView *  GetdepthImageView(){return &depthImageView;}
private:
    vk::raii::Buffer vertexBuffer = nullptr;
    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
    vk::raii::PipelineLayout      pipelineLayout      = nullptr;
    vk::raii::DescriptorPool descriptorPool = nullptr;
    
    vk::raii::Buffer       indexBuffer        = nullptr;
    vk::raii::DeviceMemory indexBufferMemory  = nullptr;
    vk::raii::DeviceMemory vertexBufferMemory = nullptr;
    std::vector<vk::raii::Buffer>       uniformBuffers;
    std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
    std::vector<void *>                 uniformBuffersMapped;
    
    vk::raii::Image        depthImage       = nullptr;
    vk::raii::DeviceMemory depthImageMemory = nullptr;
    vk::raii::ImageView    depthImageView   = nullptr;
    
    
};

