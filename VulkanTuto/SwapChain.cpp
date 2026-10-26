#include "Header/SwapChain.h"

void SwapChain::cleanupSwapChain()
{
    swapChainImageViews.clear();
    swapChain_ = nullptr;
}

void SwapChain::recreateSwapChai(Device& device,GLFWwindow& window,TextureMap& texture,VertexBuffer& vertex,SwapChain& swapchain)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(&window, &width, &height);
    while ((width == 0 || height == 0) && !glfwWindowShouldClose(&window)) {
        glfwGetFramebufferSize(&window, &width, &height);
        glfwWaitEvents();
    }
    if (glfwWindowShouldClose(&window)) {
        return;
    }
    device.GetDevice()->waitIdle();
    cleanupSwapChain();
    createSwapChain(device,window);
    createImageViews(device,texture);
    vertex.createDepthResources(swapchain,texture,device,vertex);
}

void SwapChain::createSwapChain(Device& device,GLFWwindow& window_)
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = device.GetphysiacalDevice()->getSurfaceCapabilitiesKHR(*device.GetSurface());
    swapChainExtent = chooseSwapExtent(surfaceCapabilities,window_);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);
    std::vector<vk::PresentModeKHR> availablePresentModes = device.GetphysiacalDevice()->getSurfacePresentModesKHR(*device.GetSurface());
    vk::PresentModeKHR              presentMode = chooseSwapPresentMods(availablePresentModes);
    std::vector<vk::SurfaceFormatKHR> availableFormats = device.GetphysiacalDevice()->getSurfaceFormatsKHR(*device.GetSurface());
    swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{ .surface = *device.GetSurface(),
        
                                               .minImageCount = minImageCount,
                                                
                                               .imageFormat = swapChainSurfaceFormat.format,
        
                                               .imageColorSpace = swapChainSurfaceFormat.colorSpace,
                                               .imageExtent = swapChainExtent,
                                               .imageArrayLayers = 1,
                                               .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
                                               .imageSharingMode = vk::SharingMode::eExclusive,
                                               .preTransform = surfaceCapabilities.currentTransform,
                                               .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                               .presentMode = chooseSwapPresentMods(availablePresentModes),
                                               .clipped = true,
    .oldSwapchain = *swapChain_,};

    swapChain_ = vk::raii::SwapchainKHR(*device.GetDevice(), swapChainCreateInfo);
    swapChainImages = swapChain_.getImages();
}

void SwapChain::createImageViews(Device& device,TextureMap& texture)
{
    
    assert(swapChainImageViews.empty());

    swapChainImageViews.reserve(swapChainImages.size());
    for ( auto &image: swapChainImages )
    {
        swapChainImageViews.emplace_back(texture.createImageView(device,image, swapChainSurfaceFormat.format,vk::ImageAspectFlagBits::eColor));
    }
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
    assert(!availableFormats.empty());
	


    const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format) {return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;});
    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMods(std::vector<vk::PresentModeKHR> const& availablePresntModes)
{
    assert(std::ranges::any_of(availablePresntModes, [](auto presentMode) {return presentMode == vk::PresentModeKHR::eFifo;}));
    return std::ranges::any_of(availablePresntModes, [](const vk::PresentModeKHR value) {return vk::PresentModeKHR::eMailbox == value;}) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities,GLFWwindow& window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(&window, &width, &height);
    return { std::clamp<uint32_t>(width,capabilities.minImageExtent.width,capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height,capabilities.minImageExtent.height,capabilities.maxImageExtent.height) };
}

uint32_t SwapChain::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

