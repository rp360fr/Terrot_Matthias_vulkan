#include "Header/Application.h"

void Application::Init()
{
    InitWidows();
    InitVulkan();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        renderer.get()->drawFrame();
    }
    renderer.get()->device.waitIdle();
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
    renderer = std::make_unique<Renderer>();
    renderer.get()->SetWindow(window);
    renderer.get()->CreateInstance();
    renderer.get()->SetupDebugCallback();
    renderer.get()->createSurface();
    renderer.get()->pickPhysicalDevice();
    renderer.get()->CreateLogicalDevice();
    renderer.get()->createSwapChain();
    renderer.get()->createImageViews();
    renderer.get()->createGraphicsPipeline();
    renderer.get()->createCommandPool();
    renderer.get()->createCommandBuffer();
    renderer.get()->createSyncObjects();
}

void Application::NukeWidows()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void Application::NukeVulkan()
{
}


