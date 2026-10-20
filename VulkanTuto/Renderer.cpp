#define NOMINMAX
#include "Header/Renderer.h"
#include <GLFW/glfw3.h>
#include "Header/Debug.h"

#include <iostream>


void Renderer::CreateInstance()
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

std::vector<const char*> Renderer::GetRequiredExtension()

{
	uint32_t glfwExtensionCount = 0;
	auto extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<char const*> requiredExtension = {vk::EXTDebugUtilsExtensionName};

	std::vector extension(extensions, extensions + glfwExtensionCount);
	extension.append_range(requiredExtension);
	return extension;
}

std::vector<const char*> Renderer::GetRequiredLayer()
{
	const std::vector<char const*> validationlayer = { "VK_LAYER_KHRONOS_validation" };

	std::vector<char const*> requiredLayer = {};

	if (Debug::enabaleDebug) {
		requiredLayer.append_range(validationlayer);
	}
	return requiredLayer;
}

void Renderer::SetupDebugCallback()
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

std::vector requiredDeviceExtension = { vk::KHRSwapchainExtensionName };


void Renderer::recordCommandBuffer(uint32_t imageIndex)
{
	commandBuffer.begin({});
	transition_image_layout(    imageIndex,
	vk::ImageLayout::eUndefined,
	vk::ImageLayout::eColorAttachmentOptimal,
	{},                                                        
	vk::AccessFlagBits2::eColorAttachmentWrite,                
	vk::PipelineStageFlagBits2::eColorAttachmentOutput,        
	vk::PipelineStageFlagBits2::eColorAttachmentOutput );
	
	vk::ClearValue clearColor = vk::ClearColorValue(0.0f,0.0f,0.0f,1.0f);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = swapChainImageViews[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp =  vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor,
	};
	vk::RenderingInfo renderingInfo = {
		.renderArea           = {.offset = {0, 0}, .extent = swapChainExtent},
		.layerCount           = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments    = &attachmentInfo
	};
	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);
	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));
	commandBuffer.draw(3, 1, 0, 0);
	commandBuffer.endRendering();
	// After rendering, transition the swapchain image to vk::ImageLayout::ePresentSrcKHR
	transition_image_layout(
		imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
		{},                                                     // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
		vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
	);
	commandBuffer.end();
	
}

void Renderer::transition_image_layout(uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout,
	vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask,
	vk::PipelineStageFlags2 dst_stage_mask)
{
	vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask  = src_stage_mask,
		.srcAccessMask = src_access_mask,
		.dstStageMask = dst_stage_mask,
		.dstAccessMask = dst_access_mask,
		.oldLayout = old_layout,
		.newLayout =  new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapChainImages[imageIndex],
		.subresourceRange = { .aspectMask     = vk::ImageAspectFlagBits::eColor,
				   .baseMipLevel   = 0,
				   .levelCount     = 1,
				   .baseArrayLayer = 0,
				   .layerCount     = 1}
	};
		vk::DependencyInfo dependency_info = {
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
	};
	commandBuffer.pipelineBarrier2(dependency_info);
}


bool Renderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	bool supportsVulkan1_4 = physicalDevice.getProperties().apiVersion >= VK_API_VERSION_1_4;
	auto lambdaqueue = [](auto const& qfp) {return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);};
	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	
	bool supportsGraphics = std::ranges::any_of(queueFamilies, lambdaqueue);

	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();

	bool supportsAllRequiredExtension = std::ranges::all_of(requiredDeviceExtension, [&availableDeviceExtensions](auto const& requiredDeviceExtension)
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

[[nodiscard]] vk::raii::ShaderModule Renderer::createShaderModule(const std::vector<char>& code) const
{
	vk::ShaderModuleCreateInfo createInfo
	{
		.codeSize = code.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())								
	};
	vk::raii::ShaderModule shaderModule{device,createInfo};
	return shaderModule;
}

void Renderer::pickPhysicalDevice()
{

	std::vector<vk::raii::PhysicalDevice> physicalDevives = instance.enumeratePhysicalDevices();
	auto const devIter = std::ranges::find_if(physicalDevives, [&](auto const& physicalDevice) {return isDeviceSuitable(physicalDevice);});

	if (devIter == physicalDevives.end()) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	physicalDevice = *devIter;
}

void Renderer::createSurface()
{
	
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
		throw std::runtime_error("failed to create window surface!");
	}
	surface = vk::raii::SurfaceKHR(instance, _surface);
}

void Renderer::CreateLogicalDevice()
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

	// create a Device
	float                     queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
	vk::DeviceCreateInfo      deviceCreateInfo{ .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
											   .queueCreateInfoCount = 1,
											   .pQueueCreateInfos = &deviceQueueCreateInfo,
											   .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
											   .ppEnabledExtensionNames = requiredDeviceExtension.data() };

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	queue = vk::raii::Queue(device, queueIndex, 0);


}

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
	assert(!availableFormats.empty());
	


	const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format) {return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;});
	return availableFormats[0];
}

vk::PresentModeKHR Renderer::chooseSwapPresentMods(std::vector<vk::PresentModeKHR> const& availablePresntModes)
{
	assert(std::ranges::any_of(availablePresntModes, [](auto presentMode) {return presentMode == vk::PresentModeKHR::eFifo;}));
	return std::ranges::any_of(availablePresntModes, [](const vk::PresentModeKHR value) {return vk::PresentModeKHR::eMailbox == value;}) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;

}

vk::Extent2D Renderer::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return { std::clamp<uint32_t>(width,capabilities.minImageExtent.width,capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(height,capabilities.minImageExtent.height,capabilities.maxImageExtent.height) };
}

uint32_t Renderer::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount)) {
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

void Renderer::createSwapChain()
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	swapChainExtent = chooseSwapExtent(surfaceCapabilities);
	uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);
	std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
	vk::PresentModeKHR              presentMode = chooseSwapPresentMods(availablePresentModes);
	std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
	swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);
	vk::SwapchainCreateInfoKHR swapChainCreateInfo{ .surface = *surface,
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

	swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
	swapChainImages = swapChain.getImages();
	
}

std::vector<char> Renderer::readFile(const std::string& filename)
{
	std::ifstream file(filename,std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("failed to open le ficher ta mère");
	}
	
	std::vector<char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
	file.close();
	return buffer;
}

void Renderer::createGraphicsPipeline()
{
	vk::raii::ShaderModule shaderModule = createShaderModule(readFile("shader/slang.spv"));
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
	vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
	
	
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};
	vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1, .scissorCount = 1};
	
	vk::PipelineRasterizationStateCreateInfo rasterizer{
		.depthClampEnable = vk::False,
		.rasterizerDiscardEnable =  vk::False,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise,
		.depthBiasEnable = vk::False,
		.lineWidth = 1.0f,
	};
	vk::PipelineMultisampleStateCreateInfo multisampling{.rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False};
	
	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};
	vk::PipelineColorBlendStateCreateInfo colorBlending{
		.logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment};
	
	std::vector<vk::DynamicState>      dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	vk::PipelineDynamicStateCreateInfo dynamicState{.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data()};
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{.setLayoutCount = 0, .pushConstantRangeCount = 0};


	pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);
	

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
		{.stageCount          = 2,
		 .pStages             = shaderStages,
		 .pVertexInputState   = &vertexInputInfo,
		 .pInputAssemblyState = &inputAssembly,
		 .pViewportState      = &viewportState,
		 .pRasterizationState = &rasterizer,
		 .pMultisampleState   = &multisampling,
		 .pColorBlendState    = &colorBlending,
		 .pDynamicState       = &dynamicState,
		 .layout              = pipelineLayout,
		 .renderPass          = nullptr},
		{.colorAttachmentCount = 1, .pColorAttachmentFormats = &swapChainSurfaceFormat.format}};

	graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
	
}

void Renderer::createCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queueIndex	
	};
	commandPool = vk::raii::CommandPool(device, poolInfo);
	
}

void Renderer::createCommandBuffer()
{
	vk::CommandBufferAllocateInfo allocInfo {
		.commandPool = commandPool,
		.level =  vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	commandBuffer = std::move(vk::raii::CommandBuffers(device,allocInfo).front());
}

void Renderer::createSyncObjects()
{
	
	presentCompleteSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
	renderFinishedSemaphore  = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
	drawFence                = vk::raii::Fence(device, {.flags = vk::FenceCreateFlagBits::eSignaled});
}

void Renderer::drawFrame()
{
	auto fenceResult = device.waitForFences(*drawFence, vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}
	device.resetFences(*drawFence);
	auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphore, nullptr);
	recordCommandBuffer(imageIndex);
	vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
	const vk::SubmitInfo   submitInfo{.waitSemaphoreCount   = 1,
									  .pWaitSemaphores      = &*presentCompleteSemaphore,
									  .pWaitDstStageMask    = &waitDestinationStageMask,
									  .commandBufferCount   = 1,
									  .pCommandBuffers      = &*commandBuffer,
									  .signalSemaphoreCount = 1,
									  .pSignalSemaphores    = &*renderFinishedSemaphore};
	queue.submit(submitInfo, *drawFence);
	const vk::PresentInfoKHR presentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores    = &*renderFinishedSemaphore,
		.swapchainCount     = 1,
		.pSwapchains        = &*swapChain,
		.pImageIndices      = &imageIndex};
	result = queue.presentKHR(presentInfoKHR);
}


void Renderer::createImageViews()
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
		swapChainImageViews.emplace_back(device,image_view_create_info);
	}
}


VKAPI_ATTR vk::Bool32 Renderer::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
	std::cout << "Validation layer: Type [" << vk::to_string(type) << "] - Message : " << pCallbackData->pMessage << '\n';

	return  vk::False;
}


