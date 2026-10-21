#include "Header/Device.h"

#define NOMINMAX
#include "Header/Renderer.h"
#include <GLFW/glfw3.h>
#include "Header/Debug.h"

#include <iostream>

Device::Device() 
{
    
}

void Device::CreateLogicalDevice(Instance& instance)
{
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	
    queueIndex = ~0;
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
    {
        if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
        {
			
            queueIndex = qfpIndex;
            break;
        }
    }
    if (queueIndex == ~0)
    {
        throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {
        {},                                   
        {.shaderDrawParameters = true},       
        {.dynamicRendering = true},           
        {.extendedDynamicState = true}        
        };
    auto requiredExtensions = instance.GetrequiredDeviceExtension();

    // create a Device
    float                     queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
    vk::DeviceCreateInfo      deviceCreateInfo{ .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
                                               .queueCreateInfoCount = 1,
                                               .pQueueCreateInfos = &deviceQueueCreateInfo,
                                               .enabledExtensionCount = static_cast<uint32_t>(instance.GetrequiredDeviceExtension().size()),
                                               .ppEnabledExtensionNames = requiredExtensions.data() };

    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    queue = vk::raii::Queue(device, queueIndex, 0);
}

void Device::createSurface(Instance& instance,GLFWwindow& window)
{
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(**instance.GetInstance(), &window, nullptr, &_surface) != 0) {
        throw std::runtime_error("failed to create window surface!");
    }
    surface = vk::raii::SurfaceKHR(*instance.GetInstance(), _surface);
}

void Device::pickPhysicalDevice(Instance& instance)
{
    std::vector<vk::raii::PhysicalDevice> physicalDevives = instance.GetInstance()->enumeratePhysicalDevices();
    auto const devIter = std::ranges::find_if(physicalDevives, [&](auto const& physicalDevice) {return isDeviceSuitable(physicalDevice,instance);});

    if (devIter == physicalDevives.end()) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    physicalDevice = *devIter;
}

bool Device::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice,Instance& instance)
{
    bool supportsVulkan1_4 = physicalDevice.getProperties().apiVersion >= VK_API_VERSION_1_4;
    auto lambdaqueue = [](auto const& qfp) {return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);};
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	
    bool supportsGraphics = std::ranges::any_of(queueFamilies, lambdaqueue);

    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();

    bool supportsAllRequiredExtension = std::ranges::all_of(instance.GetrequiredDeviceExtension(), [&availableDeviceExtensions](auto const& requiredDeviceExtension)
        {
            return std::ranges::any_of(availableDeviceExtensions, [requiredDeviceExtension](auto const& availableDeviceExtension) {
                return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
                });
        });
    auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
        features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
        features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

    return supportsVulkan1_4 && supportsGraphics && supportsAllRequiredExtension && supportsRequiredFeatures;
}
