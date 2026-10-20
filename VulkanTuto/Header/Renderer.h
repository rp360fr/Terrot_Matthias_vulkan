#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Application.h"	
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>
#include <fstream>

class Renderer
{

public:

	void CreateInstance();
	
	void SetWindow(GLFWwindow* _window) {
		window = _window;
	}
	void SetupDebugCallback();
	void CreateLogicalDevice();
	void createSurface();
	void pickPhysicalDevice();
	void createSwapChain();
	void createImageViews();	
	void createGraphicsPipeline();
	void createCommandPool();
	void createCommandBuffer();
	void createSyncObjects();
	void drawFrame();
	static std::vector<char> readFile(const std::string& filename);
	vk::raii::Device device = nullptr;
	
private: 
	uint32_t queueIndex = 0;
	vk::raii::CommandBuffer commandBuffer = nullptr;
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::Queue queue = nullptr;
	std::vector<const char*> GetRequiredExtension();
	std::vector<const char*> GetRequiredLayer();
	vk::raii::CommandPool commandPool = nullptr;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	vk::SurfaceFormatKHR   swapChainSurfaceFormat;
	vk::Extent2D           swapChainExtent;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline       graphicsPipeline = nullptr;
	GLFWwindow* window = nullptr;
	void recordCommandBuffer(uint32_t imageIndex);
	void transition_image_layout( uint32_t                imageIndex,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask);
	bool isDeviceSuitable(vk::raii::PhysicalDevice const & physicalDevice);
	vk::raii::Semaphore presentCompleteSemaphore = nullptr;
	vk::raii::Semaphore renderFinishedSemaphore  = nullptr;
	vk::raii::Fence     drawFence                = nullptr;

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMods(std::vector<vk::PresentModeKHR> const& availablePresntModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);
	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);


	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
};

