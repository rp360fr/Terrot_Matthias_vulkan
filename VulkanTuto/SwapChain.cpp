#include "Header/SwapChain.h"

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
                                               .clipped = true };

    swapChain_ = vk::raii::SwapchainKHR(*device.GetDevice(), swapChainCreateInfo);
    swapChainImages = swapChain_.getImages();
}

void SwapChain::createImageViews(Device& device)
{
    
    assert(swapChainImageViews.empty());
    vk::ImageViewCreateInfo image_view_create_info {
        .viewType = vk::ImageViewType::e2D,
        .format = swapChainSurfaceFormat.format,
        .subresourceRange ={ vk::ImageAspectFlagBits::eColor,0,1,0,1}
    };
	
    for (auto &image : swapChainImages)
    {
        image_view_create_info.image = image;
        swapChainImageViews.emplace_back(*device.GetDevice(),image_view_create_info);
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

