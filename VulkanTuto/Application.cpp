#include "Header/Application.h"
//#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
Application::Application()
{

    
}

void Application::Init()
{
    InitWidows();
    InitVulkan();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        pipeline.drawFrame(device,swapChain,*window);
    }
}

void Application::Nuke()
{
    NukeVulkan();
    NukeWidows();
}

void Application::cleanup()
{
    swapChain.cleanupSwapChain();
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    *app->pipeline.GetFramebufferResized() = true;
   
}

void Application::InitWidows()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    
     window = glfwCreateWindow(800, 600, "Vulan Tuto", nullptr, nullptr);
    glfwSetWindowUserPointer(window,this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Application::InitVulkan()
{
    renderer.InstanceInit(instance);
    renderer.DeviceInit(device,instance,*window);
    renderer.swapChainInit(swapChain,device,*window);
    renderer.PipelineInit(pipeline,swapChain,device,vertexBuffer);

    
}

void Application::NukeWidows()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void Application::NukeVulkan()
{
}



