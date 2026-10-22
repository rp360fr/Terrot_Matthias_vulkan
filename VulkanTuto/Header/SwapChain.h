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
    void cleanupSwapChain();
    void recreateSwapChai(Device& device,GLFWwindow& window);
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
    vk::raii::SwapchainKHR* GetSwapChain()
    {
        return &swapChain_;
    }
    std::vector<vk::raii::ImageView>* GetSwapChainImageView()
    {
        return &swapChainImageViews;
    }
    std::vector<vk::raii::ImageView> swapChainImageViews;
private:
    
    vk::raii::SwapchainKHR swapChain_ = nullptr;
    std::vector<vk::Image> swapChainImages;
    vk::SurfaceFormatKHR   swapChainSurfaceFormat;
    vk::Extent2D           swapChainExtent;
  
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
    vk::PresentModeKHR chooseSwapPresentMods(std::vector<vk::PresentModeKHR> const& availablePresntModes);
    vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities,GLFWwindow& window);
    uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
};
