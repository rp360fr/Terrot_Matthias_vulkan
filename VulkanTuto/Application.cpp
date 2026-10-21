#include "Header/Application.h"

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
        pipeline.drawFrame(device,swapChain);
    }
    device.GetDevice()->waitIdle();
}

void Application::Nuke()
{
    NukeVulkan();
    NukeWidows();
}





void Application::InitWidows()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

     window = glfwCreateWindow(800, 600, "Vulan Tuto", nullptr, nullptr);
}

void Application::InitVulkan()
{
    renderer.InstanceInit(instance);
    renderer.DeviceInit(device,instance,*window);
    renderer.swapChainInit(swapChain,device,*window);
    renderer.PipelineInit(pipeline,swapChain,device);

    
}

void Application::NukeWidows()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void Application::NukeVulkan()
{
}


