#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h> 
#include "VertexBuffer.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>
#include <fstream>
class Device;
class SwapChain;
class Pipeline
{
public:
    
    void createGraphicsPipeline(Device& device,SwapChain& swapchain,VertexBuffer vertexbuffer);
    void createCommandPool(Device& device,SwapChain& swapchain);
    void createCommandBuffers(Device& device);
    void createSyncObjects(Device& device,SwapChain& swapchain);
    void drawFrame(Device& device,SwapChain& swapchain,GLFWwindow& window);
    static std::vector<char> readFile(const std::string& filename);
	bool* GetFramebufferResized()
	{
		return &framebufferResized;
	}
	
	
	
	
private:
    
    std::vector<vk::raii::CommandBuffer> commandBuffers;
    vk::raii::Context context;
	int MAX_FRAMES_IN_FLIGHT = 2;
	bool framebufferResized = false;
	
    vk::raii::Queue graphicsQueue = nullptr;
  
	const std::vector<vertex> vertives = {
		{{0.0f,-0.5f},{1.0f,0.0f,0.0f}},
		{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
		{{-0.5f,0.5f},{0.0f,0.0f,1.0f}},
	};
	uint32_t frameIndex = 0;
	
    vk::raii::CommandPool commandPool = nullptr;
    std::vector<vk::raii::ImageView> swapChainImageViews;
    


    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline       graphicsPipeline = nullptr;
    GLFWwindow* window = nullptr;
    void recordCommandBuffer(uint32_t imageIndex,SwapChain& swapchain);
    void transition_image_layout( uint32_t                imageIndex,
        vk::ImageLayout         old_layout,
        vk::ImageLayout         new_layout,
        vk::AccessFlags2        src_access_mask,
        vk::AccessFlags2        dst_access_mask,
        vk::PipelineStageFlags2 src_stage_mask,
        vk::PipelineStageFlags2 dst_stage_mask,
        SwapChain& swapchain);
	
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;

    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code,Device& device) const;
};
