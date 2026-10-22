#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "Instance.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include <vulkan/vulkan_raii.hpp>
#include <fstream>
class Device;
class Instance;
class SwapChain;
class Renderer
{

public:
	void InstanceInit( Instance& instance);
	void DeviceInit( Device& device,Instance& instance,GLFWwindow& window);
	void swapChainInit( SwapChain& swapChain,Device& device,GLFWwindow& window_);
	void PipelineInit( Pipeline& pipeline,SwapChain& swapchain,Device& device,VertexBuffer vertex_buffer);
	
	
	
	
	
	
private: 
	
	
	


	
};

