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

void Renderer::PipelineInit( Pipeline& pipeline,SwapChain& swapchain,Device& device)
{
    pipeline.createGraphicsPipeline(device,swapchain);
    pipeline.createCommandPool(device,swapchain);
    pipeline.createCommandBuffer(device);
    pipeline.createSyncObjects(device);
}
