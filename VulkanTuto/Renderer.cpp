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

void Renderer::swapChainInit( SwapChain& swapChain,Device& device,GLFWwindow& window_,TextureMap& texture)
{
    swapChain.createSwapChain(device,window_);
    swapChain.createImageViews(device,texture);
}

void Renderer::PipelineInit( Pipeline& pipeline,SwapChain& swapchain,Device& device,VertexBuffer& vertex_buffer,TextureMap& texture)
{
    vertex_buffer.createDescriptorSetLayout(device);
    pipeline.createGraphicsPipeline(device,swapchain,vertex_buffer);
    pipeline.createCommandPool(device,swapchain);
    vertex_buffer.createDepthResources(swapchain,texture,device,vertex_buffer);
    texture.createtextureImage(vertex_buffer,device,pipeline);
    texture.createTextureImageView(device);
    texture.createTextureSampler(device);

    vertex_buffer.CreateVertexBuffer(device,pipeline,texture);
    vertex_buffer.CreateIndexBuffer(device,pipeline,texture);
    vertex_buffer.CreateUniformBuffer(device,pipeline);
    vertex_buffer.createDescriptorPool(device,pipeline);
    vertex_buffer.createDescriptorSets(device,pipeline,texture);
    pipeline.createCommandBuffers(device);
    pipeline.createSyncObjects(device,swapchain);
}


