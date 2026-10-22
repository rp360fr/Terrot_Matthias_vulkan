#define NOMINMAX
#include "Header/Renderer.h"
#include <GLFW/glfw3.h>
#include "Header/Debug.h"

#include <iostream>


void Renderer::InstanceInit(Instance& instance)
{
    instance.CreateInstance();
    instance.SetupDebugCallback();
}

void Renderer::DeviceInit(Device& device,Instance& instance,GLFWwindow& window_)
{
    device.createSurface(instance,window_);
    device.pickPhysicalDevice(instance);
    device.CreateLogicalDevice(instance);
}

void Renderer::swapChainInit( SwapChain& swapChain,Device& device,GLFWwindow& window_)
{
    swapChain.createSwapChain(device,window_);
    swapChain.createImageViews(device);
}

void Renderer::PipelineInit( Pipeline& pipeline,SwapChain& swapchain,Device& device,VertexBuffer vertex_buffer)
{
    pipeline.createGraphicsPipeline(device,swapchain,vertex_buffer);
    pipeline.createCommandPool(device,swapchain);
    pipeline.createCommandBuffers(device);
    pipeline.createSyncObjects(device,swapchain);
}
