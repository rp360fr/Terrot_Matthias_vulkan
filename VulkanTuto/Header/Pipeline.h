#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h> 

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>
#include <fstream>
class Device;
class SwapChain;
class Pipeline
{
public:
    
    void createGraphicsPipeline(Device& device,SwapChain& swapchain);
    void createCommandPool(Device& device,SwapChain& swapchain);
    void createCommandBuffer(Device& device);
    void createSyncObjects(Device& device);
    void drawFrame(Device& device,SwapChain& swapchain);
    static std::vector<char> readFile(const std::string& filename);
private:
    
    vk::raii::CommandBuffer commandBuffer = nullptr;
    vk::raii::Context context;
	
	
	
    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	

	
    vk::raii::CommandPool commandPool = nullptr;
    std::vector<vk::raii::ImageView> swapChainImageViews;
    vk::raii::SwapchainKHR swapChain = nullptr;

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
	
    vk::raii::Semaphore presentCompleteSemaphore = nullptr;
    vk::raii::Semaphore renderFinishedSemaphore  = nullptr;
    vk::raii::Fence     drawFence                = nullptr;

    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code,Device& device) const;
};
