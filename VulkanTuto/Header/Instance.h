#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>
#include <fstream>

class Instance
{
public:
    Instance()
    {
        
    }
    void CreateInstance();
    void SetupDebugCallback();
    std::vector<const char*> GetrequiredDeviceExtension()
    {
        return requiredDeviceExtension;
    }
    vk::raii::Instance* GetInstance()
    {
        return &instance;
    }
    
private:
    std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
    vk::raii::Context context;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    vk::raii::Instance instance = nullptr;
    
    std::vector<const char*> GetRequiredExtension();
    std::vector<const char*> GetRequiredLayer();
    
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
};
