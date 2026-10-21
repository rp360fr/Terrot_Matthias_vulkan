#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#include "Instance.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan_raii.hpp>
#include <fstream>
class Instance;
class Device
{
public:
   Device();
    
    void CreateLogicalDevice(Instance& instance);
    void createSurface(Instance& instance,GLFWwindow& window_);
    void pickPhysicalDevice(Instance& instance);
    vk::raii::Device* GetDevice()
    {
        return &device;
    }
    vk::raii::PhysicalDevice* GetphysiacalDevice()
    {
        return &physicalDevice;
    }
    vk::raii::SurfaceKHR* GetSurface()
    {
        return &surface;
    }
    uint32_t* GetQueueIndex()
    {
        return &queueIndex;
    }
    vk::raii::Queue* GetQueue()
    {
        return &queue;
    }
  
   
    
private:
    
    bool isDeviceSuitable(vk::raii::PhysicalDevice const & physicalDevice,Instance& instance);
    vk::raii::PhysicalDevice physicalDevice = nullptr;
   
    uint32_t queueIndex = 0;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::Device device = nullptr;
    vk::raii::Queue queue = nullptr;
    
};
