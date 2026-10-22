//#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
#include "Header/Pipeline.h"

#include "Header/Device.h"
#include "Header/SwapChain.h"


void Pipeline::recordCommandBuffer(uint32_t imageIndex,SwapChain& swapchain)
{
	auto &commandBuffer = commandBuffers[frameIndex];
	commandBuffer.begin({});
	transition_image_layout(    imageIndex,
	vk::ImageLayout::eUndefined,
	vk::ImageLayout::eColorAttachmentOptimal,
	{},                                                        
	vk::AccessFlagBits2::eColorAttachmentWrite,                
	vk::PipelineStageFlagBits2::eColorAttachmentOutput,        
	vk::PipelineStageFlagBits2::eColorAttachmentOutput,swapchain);
	
	vk::ClearValue clearColor = vk::ClearColorValue(0.0f,0.0f,0.0f,1.0f);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = swapchain.swapChainImageViews[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp =  vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor,
	};
	vk::RenderingInfo renderingInfo = {
		.renderArea           = {.offset = {0, 0}, .extent = swapchain.GetswapChainExtent()},
		.layerCount           = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments    = &attachmentInfo
	};
	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);
	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchain.GetswapChainExtent().width), static_cast<float>(swapchain.GetswapChainExtent().height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchain.GetswapChainExtent()));
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
		vk::PipelineStageFlagBits2::eBottomOfPipe,
		swapchain
	
	);
	commandBuffer.end();
	
}

void Pipeline::transition_image_layout(uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout,
	vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask,
	vk::PipelineStageFlags2 dst_stage_mask,SwapChain& swapchain)
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
		.image = swapchain.GetSwapCHainImage()[imageIndex],
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
	commandBuffers[frameIndex].pipelineBarrier2(dependency_info);
}


[[nodiscard]] vk::raii::ShaderModule Pipeline::createShaderModule(const std::vector<char>& code,Device& device) const
{
	vk::ShaderModuleCreateInfo createInfo
	{
		.codeSize = code.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())								
	};
	vk::raii::ShaderModule shaderModule{*device.GetDevice(),createInfo};
	return shaderModule;
}


std::vector<char> Pipeline::readFile(const std::string& filename)
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

void Pipeline::createGraphicsPipeline(Device& device,SwapChain& swapchain,VertexBuffer vertexbuffer)
{
	vk::raii::ShaderModule shaderModule = createShaderModule(readFile("shader/slang.spv"),device);
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
	vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
	
	
	auto                                     bindingDescription    = vertexbuffer.getBindingDescription();
	auto                                     attributeDescriptions = vertexbuffer.getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo   vertexInputInfo{.vertexBindingDescriptionCount   = 1,
															 .pVertexBindingDescriptions      = &bindingDescription,
															 .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
															 .pVertexAttributeDescriptions    = attributeDescriptions.data()};
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


	pipelineLayout = vk::raii::PipelineLayout(*device.GetDevice(), pipelineLayoutInfo);
	
	const vk::Format colorFormat = swapchain.GetSurfaceFormatKHR().format;
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
		{.colorAttachmentCount = 1, .pColorAttachmentFormats = &colorFormat}};

	graphicsPipeline = vk::raii::Pipeline(*device.GetDevice(), nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
	
}

void Pipeline::createCommandPool(Device& device,SwapChain& swapchain)
{
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = *device.GetQueueIndex(),	
	};
	commandPool = vk::raii::CommandPool(*device.GetDevice(), poolInfo);
	
}

void Pipeline::createCommandBuffers(Device& device)
{
	vk::CommandBufferAllocateInfo allocInfo{.commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)};
	commandBuffers = vk::raii::CommandBuffers( *device.GetDevice(), allocInfo );
}

void Pipeline::createSyncObjects(Device& device,SwapChain& swapchain)
{
	
	assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

	for (size_t i = 0; i < swapchain.GetSwapCHainImage().size(); i++)
	{
		renderFinishedSemaphores.emplace_back(*device.GetDevice(), vk::SemaphoreCreateInfo());
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		presentCompleteSemaphores.emplace_back(*device.GetDevice(), vk::SemaphoreCreateInfo());
		inFlightFences.emplace_back(*device.GetDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
	}
}

void Pipeline::drawFrame(Device& device,SwapChain& swapchain,GLFWwindow& window)
{
	auto fenceResult = device.GetDevice()->waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	auto [result, imageIndex] = swapchain.GetSwapChain()->acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized)
	{
		framebufferResized = false;
		swapchain.recreateSwapChai(device,window);
		return;
	}
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Only reset the fence if we are submitting work
	device.GetDevice()->resetFences(*inFlightFences[frameIndex]);
	commandBuffers[frameIndex].reset();
	recordCommandBuffer(imageIndex,swapchain);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo   submitInfo{.waitSemaphoreCount   = 1,
									  .pWaitSemaphores      = &*presentCompleteSemaphores[frameIndex],
									  .pWaitDstStageMask    = &waitDestinationStageMask,
									  .commandBufferCount   = 1,
									  .pCommandBuffers      = &*commandBuffers[frameIndex],
									  .signalSemaphoreCount = 1,
									  .pSignalSemaphores    = &*renderFinishedSemaphores[imageIndex]};
	device.GetQueue()->submit(submitInfo, *inFlightFences[frameIndex]);
	
	const vk::PresentInfoKHR presentInfoKHR{.waitSemaphoreCount = 1,
												.pWaitSemaphores    = &*renderFinishedSemaphores[imageIndex],
												.swapchainCount     = 1,
												.pSwapchains        = &**swapchain.GetSwapChain(),
												.pImageIndices      = &imageIndex};
	result = device.GetQueue()->presentKHR(presentInfoKHR);
	// Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined, eErrorOutOfDateKHR can be checked as a result
	// here and does not need to be caught by an exception.
	if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) || framebufferResized)
	{
		framebufferResized = false;
		swapchain.recreateSwapChai(device,window);
	}
	else
	{
		// There are no other success codes than eSuccess; on any error code, presentKHR already threw an exception.
		assert(result == vk::Result::eSuccess);
	}
	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
