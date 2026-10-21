#include "Header/Instance.h"
#define NOMINMAX
#include "Header/Renderer.h"
#include <GLFW/glfw3.h>
#include "Header/Debug.h"

#include <iostream>


void Instance::CreateInstance()
{
    
    constexpr vk::ApplicationInfo info
    {
        .pApplicationName = "Triangleux",
        .applicationVersion = VK_MAKE_VERSION(67,0,0),
        .pEngineName = "gaming Campus super vulkan triangle engine turbo deluxe edition chapitre 1 remake",
        .engineVersion = VK_MAKE_VERSION(67,0,0),
        .apiVersion = vk::ApiVersion14
    };
        std::vector RequiredExtensions = GetRequiredExtension();

        auto extensionPropertiers = context.enumerateInstanceExtensionProperties();


        for (uint32_t i = 0; i < RequiredExtensions.size(); i++)
        {
            auto compareExtension = [glfwExtension = RequiredExtensions[i]](auto const& extensionProperty) {


                return strcmp(extensionProperty.extensionName, glfwExtension) == 0;

            };
            if (std::ranges::none_of(extensionPropertiers, compareExtension
                )) 
            {
                throw std::runtime_error("Required GLFW extension not supported :" + std::string(RequiredExtensions[i]));
            }
        }
	
	
        std::vector<char const*> requieredLayer = GetRequiredLayer();
	

        auto layerProperties = context.enumerateInstanceLayerProperties();

        auto unsupportedLayerIt = std::ranges::find_if(requieredLayer, [&layerProperties](auto const& required)
            {

                return std::ranges::none_of(layerProperties, [required](auto const& layerProperty) 
                    {
                        return strcmp(layerProperty.layerName, required) == 0;
                    });

            });

        if (unsupportedLayerIt != requieredLayer.end()){
            throw std::runtime_error("Required Layer not supported:" + std::string(*unsupportedLayerIt));
        }

        vk::InstanceCreateInfo create_info{
            .pApplicationInfo = &info,
            .enabledLayerCount = static_cast<uint32_t>(requieredLayer.size()),
            .ppEnabledLayerNames = requieredLayer.data(),
            .enabledExtensionCount = static_cast<uint32_t>(RequiredExtensions.size()) ,
            .ppEnabledExtensionNames = RequiredExtensions.data()
		
		
        };

        instance = vk::raii::Instance(context, create_info);



        SetupDebugCallback();


    }

void Instance::SetupDebugCallback()
{
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    vk::DebugUtilsMessengerCreateInfoEXT debugInfo{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &DebugCallback,
    };

    debugMessenger = instance.createDebugUtilsMessengerEXT(debugInfo);
}

std::vector<const char*> Instance::GetRequiredExtension()
{
    uint32_t glfwExtensionCount = 0;
    auto extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<char const*> requiredExtension = {vk::EXTDebugUtilsExtensionName};

    std::vector extension(extensions, extensions + glfwExtensionCount);
    extension.append_range(requiredExtension);
    return extension;
}
std::vector requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

std::vector<const char*> Instance::GetRequiredLayer()
{
    const std::vector<char const*> validationlayer = { "VK_LAYER_KHRONOS_validation" };

    std::vector<char const*> requiredLayer = {};

    if (Debug::enabaleDebug) {
        requiredLayer.append_range(validationlayer);
    }
    return requiredLayer;
}

vk::Bool32 Instance::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT type,
                                   const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
    std::cout << "Validation layer: Type [" << vk::to_string(type) << "] - Message : " << pCallbackData->pMessage << '\n';

    return  vk::False;
}
