#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#include "Device.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>
#include <fstream>
class Device;
class SwapChain
{
public:
    SwapChain(){}
    ~SwapChain(){}
    void createSwapChain(Device& device,GLFWwindow& window_);
    void createImageViews(Device& device);	
    vk::SurfaceFormatKHR GetSurfaceFormatKHR()
    {
        return swapChainSurfaceFormat;
    }
    vk::Extent2D  GetswapChainExtent()
    {
        return swapChainExtent;
    }
    std::vector<vk::Image> GetSwapCHainImage()
    {
        return swapChainImages;
    }
    
private:
    std::vector<vk::raii::ImageView> swapChainImageViews;
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> swapChainImages;
    vk::SurfaceFormatKHR   swapChainSurfaceFormat;
    vk::Extent2D           swapChainExtent;
  
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
    vk::PresentModeKHR chooseSwapPresentMods(std::vector<vk::PresentModeKHR> const& availablePresntModes);
    vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities,GLFWwindow& window);
    uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
};
