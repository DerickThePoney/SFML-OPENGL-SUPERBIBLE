#include "stdafx.h"
#include "VulkanRenderer.h"
#include "MaterialManager.h"


VulkanRenderer VulkanRenderer::instance = {};



VulkanRenderer::VulkanRenderer()
	: m_bDrawOverlays(true)
{
}


VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Init(GLFWwindow * window)
{
	instance.InstanceInit(window);
}

void VulkanRenderer::Cleanup()
{
	instance.InstanceCleanup();
}

void VulkanRenderer::AddRenderPass(VulkanRenderPass & renderPass, std::vector<VulkanFramebuffer>& framebuffers, bool bCreateSemaphore)
{
	RenderPassData data;
	data.renderPass = renderPass;
	data.framebuffers = framebuffers;

	if (bCreateSemaphore)
	{
		data.vulkanSemaphore.resize(framebuffers.size());
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (size_t i = 0; i < data.vulkanSemaphore.size(); ++i)
		{
			if (vkCreateSemaphore(instance.m_kDevice, &info, nullptr, &data.vulkanSemaphore[i]) != VK_SUCCESS)
			{

				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}


	instance.m_akAdditionalRenderPasses.push_back(data);
}

void VulkanRenderer::SetFinalRenderPassAndFramebuffers(VulkanRenderPass & renderPass, std::vector<VulkanFramebuffer>& framebuffers)
{
	instance.m_kFinalRenderPass = renderPass;
	instance.m_akSwapchainFrameBuffers = framebuffers;
}

void VulkanRenderer::RecreateSwapChain(GLFWwindow * window)
{
	instance.InstanceRecreateSwapChain(window);
}

void VulkanRenderer::EnsureDeviceIdle()
{
	instance.InstanceEnsureDeviceIdle();
}

void VulkanRenderer::PrepareFrame()
{
	instance.InstancePrepareFrame();
}

void VulkanRenderer::PrepareCommandBuffer(IScene* scene, const Frustum& frustum)
{
	instance.InstancePrepareCommandBuffer(scene, frustum);
}

void VulkanRenderer::DrawFrame()
{
	instance.InstanceDrawFrame();
}

void VulkanRenderer::EndFrame()
{
	instance.InstanceEndFrame();
}

void VulkanRenderer::Inspect()
{
	instance.InstanceInspect();
}


void VulkanRenderer::InstanceInit(GLFWwindow* window)
{
	this->window = window;
	glfwGetWindowSize(window, &width, &height);
	CreateInstance();
	CreateSurface(window);
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();


	CreateFinalRenderPass();
	CreateOverlaysRenderPass();
	CreateDescriptorPool();
	CreateDescriptorSetLayout();
	//CreateGraphicsPipeline();
	CreateCommandPool();
	CreateDepthAttachment();
	CreateFrameBuffers();
	CreateSyncObjects();

	CreateCommandBuffers();

	MaterialManager::Get().Initialise();


	/*CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	CreateVertexBuffers();
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSet();
	CreateCommandBuffers();
	CreateSyncObjects();*/
}

void VulkanRenderer::InstanceCleanup()
{
	MaterialManager::Get().Destroy();
	CleanupAdditionalRenderPasses();
	CleanupSwapChain();
	
	vkDestroyDescriptorPool(m_kDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(m_kDevice, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
	{
		vkDestroySemaphore(m_kDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_kDevice, imageAvailableSemaphores[i], nullptr);
		//vkDestroySemaphore(m_kDevice, offscreenRenderSemaphores[i], nullptr);
		vkDestroyFence(m_kDevice, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_kDevice, commandPoolTransfer, nullptr);
	vkDestroyCommandPool(m_kDevice, commandPoolGraphics, nullptr);
	m_kDevice.Destroy();
	m_kSurface.Destroy(m_kInstance);
	m_kInstance.Destroy();
}

void VulkanRenderer::CleanupSwapChain()
{
	depthAttachement.view.Destroy(m_kDevice);
	depthAttachement.image.Free();

	for (size_t i = 0; i < m_akSwapchainFrameBuffers.size(); i++) {
		m_akSwapchainFrameBuffers[i].Destroy(m_kDevice);
	}

	VulkanCommandBuffer::Free(commandBuffers, m_kDevice, commandPoolGraphics);

	//m_kPipeline.Destroy(m_kDevice);
	m_kOverlaysRenderPass.Destroy(m_kDevice);
	m_kFinalRenderPass.Destroy(m_kDevice);

	for (size_t i = 0; i < m_kSwapwhain.swapchainAttachments.size(); i++) {
		m_kSwapwhain.swapchainAttachments[i].view.Destroy(m_kDevice);
	}

	vkDestroySwapchainKHR(m_kDevice, m_kSwapwhain.swapChain, nullptr);
}

void VulkanRenderer::CleanupAdditionalRenderPasses()
{
	for (size_t i = 0; i < m_akAdditionalRenderPasses.size(); ++i)
	{
		m_akAdditionalRenderPasses[i].renderPass.Destroy(m_kDevice);
		
		for (size_t j = 0; j < m_akAdditionalRenderPasses[i].framebuffers.size(); ++j)
		{
			m_akAdditionalRenderPasses[i].framebuffers[j].Destroy(m_kDevice);
		}

		for (size_t j = 0; j < m_akAdditionalRenderPasses[i].vulkanSemaphore.size(); ++j)
		{
			vkDestroySemaphore(m_kDevice, m_akAdditionalRenderPasses[i].vulkanSemaphore[j], nullptr);
		}
	}
}

void VulkanRenderer::InstanceRecreateSwapChain(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;

	vkDeviceWaitIdle(m_kDevice);

	CleanupSwapChain();
	m_kPhysicalDevice.ResetSwapchainSupportDetails(m_kSurface);

	CreateSwapChain();
	CreateImageViews();
	CreateOverlaysRenderPass();
	CreateFinalRenderPass();
	CreateDepthAttachment();
	CreateFrameBuffers();
	CreateCommandBuffers();
}

void VulkanRenderer::InstanceEnsureDeviceIdle()
{
	for (size_t i = 0; i < inFlightFences.size(); ++i)
	{
		vkWaitForFences(m_kDevice, 1, &inFlightFences[i], VK_TRUE, std::numeric_limits<uint64_t>::max());
	}

	vkQueueWaitIdle(presentQueue);
	vkQueueWaitIdle(transferQueue);
	vkQueueWaitIdle(graphicsQueue);
	vkDeviceWaitIdle(m_kDevice);
}

void VulkanRenderer::InstancePrepareFrame()
{
	ImGui_ImplGlfwVulkan_NewFrame();
}

void VulkanRenderer::InstancePrepareCommandBuffer(IScene* scene, const Frustum& frustum)
{
	//wait for the device to finish before killing of the buffer
	vkWaitForFences(m_kDevice, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	//vkDeviceWaitIdle(VulkanRenderer::GetDevice());

	if (commandBuffers[currentFrame].BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkViewport viewport = {};// vks::initializers::viewport((float)width, (float)height, 0.0f, 1.0f);
	viewport.height = (float)VulkanRenderer::GetSurfaceExtent().height;
	viewport.width = (float)VulkanRenderer::GetSurfaceExtent().width;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	commandBuffers[currentFrame].SetViewport(0, 1, &viewport);

	VkRect2D scissor = {};// vks::initializers::rect2D(width, height, 0, 0);
	scissor.extent = VulkanRenderer::GetSurfaceExtent();
	scissor.offset = VkOffset2D{ 0,0 };
	commandBuffers[currentFrame].SetScissors(0, 1, &scissor);

	//start render pass
	std::vector<VkClearValue> clearColor = { {},{} };
	clearColor[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearColor[1].depthStencil = { 1.0f, 0 };
	commandBuffers[currentFrame].BeginRenderPass
	(
		VulkanRenderer::GetRenderPass(0),
		VulkanRenderer::GetSwapchainBuffers()[currentFrame],
		VkOffset2D({ 0, 0 }),
		VulkanRenderer::GetSurfaceExtent(),
		clearColor,
		VK_SUBPASS_CONTENTS_INLINE//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
	);

	scene->Draw(commandBuffers[currentFrame],frustum);

	//End render pass and command buffer
	commandBuffers[currentFrame].EndRenderPass();

	if (m_bDrawOverlays) RenderOverlays();

	if (commandBuffers[currentFrame].EndCommandBuffer() != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VulkanRenderer::RenderOverlays()
{
	
	VkViewport viewport = {};// vks::initializers::viewport((float)width, (float)height, 0.0f, 1.0f);
	viewport.height = (float)VulkanRenderer::GetSurfaceExtent().height;
	viewport.width = (float)VulkanRenderer::GetSurfaceExtent().width;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	commandBuffers[currentFrame].SetViewport(0, 1, &viewport);

	VkRect2D scissor = {};// vks::initializers::rect2D(width, height, 0, 0);
	scissor.extent = VulkanRenderer::GetSurfaceExtent();
	scissor.offset = VkOffset2D{ 0,0 };
	commandBuffers[currentFrame].SetScissors(0, 1, &scissor);

	//start render pass
	std::vector<VkClearValue> clearColor = { {} };
	clearColor[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//clearColor[1].depthStencil = { 1.0f, 0 };
	commandBuffers[currentFrame].BeginRenderPass
	(
		m_kOverlaysRenderPass,
		m_akSwapchainFrameBuffers[currentFrame],
		VkOffset2D({ 0, 0 }),
		m_kSwapwhain.swapChainExtent,
		clearColor,
		VK_SUBPASS_CONTENTS_INLINE//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
	);


	ImGui_ImplGlfwVulkan_Render(commandBuffers[currentFrame]);

	//End render pass and command buffer
	commandBuffers[currentFrame].EndRenderPass();
}

void VulkanRenderer::InstanceDrawFrame()
{
	vkWaitForFences(VulkanRenderer::GetDevice(), 1, &VulkanRenderer::GetFence(currentFrame), VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(VulkanRenderer::GetDevice(), 1, &VulkanRenderer::GetFence(currentFrame));

	//SetupSubpassBuffer();
	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(VulkanRenderer::GetDevice(), VulkanRenderer::GetSwapChain(), std::numeric_limits<uint64_t>::max(), VulkanRenderer::GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain(window);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}


	/*VkSubmitInfo submitInfoOffscreen = {};
	submitInfoOffscreen.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphoresOff[] = { VulkanRenderer::GetImageAvailableSemaphore(currentFrame) };
	VkPipelineStageFlags waitStagesOff[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfoOffscreen.waitSemaphoreCount = 1;
	submitInfoOffscreen.pWaitSemaphores = waitSemaphoresOff;
	submitInfoOffscreen.pWaitDstStageMask = waitStagesOff;

	submitInfoOffscreen.commandBufferCount = 1;
	submitInfoOffscreen.pCommandBuffers = commandBuffersOffscreen[imageIndex].GetCommandBuffer();

	VkSemaphore signalSemaphoresOff[] = { offscreenPass.semaphore };
	submitInfoOffscreen.signalSemaphoreCount = 1;
	submitInfoOffscreen.pSignalSemaphores = signalSemaphoresOff;*/

	//if (vkQueueSubmit(VulkanRenderer::GetGraphicsQueue(), 1, &submitInfoOffscreen, VK_NULL_HANDLE) != VK_SUCCESS) {
		//throw std::runtime_error("failed to submit draw command buffer!");
	//}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { VulkanRenderer::GetImageAvailableSemaphore(currentFrame) };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers[imageIndex].GetCommandBuffer();

	VkSemaphore signalSemaphores[] = { VulkanRenderer::GetRenderFinishedSemaphores(currentFrame) };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	VkResult res = vkQueueSubmit(VulkanRenderer::GetGraphicsQueue(), 1, &submitInfo, VulkanRenderer::GetFence(currentFrame));
	if (res != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { VulkanRenderer::GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(VulkanRenderer::GetPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		RecreateSwapChain(window);
		return;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void VulkanRenderer::InstanceEndFrame()
{
	currentFrame = (currentFrame + 1) % m_akSwapchainFrameBuffers.size();
}

void VulkanRenderer::InstanceInspect()
{
	ImGui::Begin("Renderer");

	ImGui::Text("%s", m_kPhysicalDevice.GetDeviceProperties().deviceName);

	if (ImGui::CollapsingHeader("Available extensions"))
	{
		for (const auto& extension : m_kInstance.GetExtensionsProps())
		{
			ImGui::BulletText("%s", extension.extensionName);
		}
	}

	if (ImGui::CollapsingHeader("Physical device properties"))
	{
		const VkPhysicalDeviceProperties& props = m_kPhysicalDevice.GetDeviceProperties();
		const VkPhysicalDeviceFeatures& feats = m_kPhysicalDevice.GetDeviceFeatures();
		ImGui::Text("Device name:\t%s", props.deviceName);

		InspectFeatures(feats);
		InspectSparseProperties(props);
		InspectLimits(props);	
	}

	ImGui::End();
}

void VulkanRenderer::CreateInstance()
{
	m_kInstance.Init(
		"Hello Triangle",
		VK_MAKE_VERSION(1, 0, 0),
		"No Engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_1,
		enableValidationLayers,
		validationLayers
	);
}

void VulkanRenderer::PickPhysicalDevice()
{
	m_kPhysicalDevice.PickDevice(m_kInstance, m_kSurface, deviceExtensions);
}

void VulkanRenderer::CreateLogicalDevice()
{
	//create queue create info
	QueueFamilyIndices indices = m_kPhysicalDevice.GetQueueFamilyIndices();
	m_kDevice.Initialise(m_kPhysicalDevice, enableValidationLayers, validationLayers, deviceExtensions);


	//get the actual graphics queue handle
	m_kDevice.GetQueue(indices.graphicsFamily, 0, graphicsQueue);
	m_kDevice.GetQueue(indices.presentFamily, 1, presentQueue);
	m_kDevice.GetQueue(indices.transferFamily, 0, transferQueue);
}

void VulkanRenderer::CreateSurface(GLFWwindow* window)
{
	m_kSurface.Init(m_kInstance, window);
}

void VulkanRenderer::CreateSwapChain()
{
	SwapChainSupportDetails details = m_kPhysicalDevice.GetSwapchainSupportDetails();

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(details.presentModes);
	VkExtent2D extent = ChooseSwapExtent(details.capabilities);

	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_kSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = m_kPhysicalDevice.GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_kDevice, &createInfo, nullptr, &m_kSwapwhain.swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_kDevice, m_kSwapwhain.swapChain, &imageCount, nullptr);
	std::vector<VkImage> swapChainImages(imageCount);
	swapChainImages.resize(imageCount);
	m_kSwapwhain.swapchainAttachments.resize(imageCount);
	vkGetSwapchainImagesKHR(m_kDevice, m_kSwapwhain.swapChain, &imageCount, swapChainImages.data());

	for (uint32_t i = 0; i < imageCount; ++i)
	{
		m_kSwapwhain.swapchainAttachments[i].image.Init(swapChainImages[i], extent.width, extent.height,
			surfaceFormat.format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	}


	m_kSwapwhain.swapChainImageFormat = surfaceFormat.format;
	m_kSwapwhain.swapChainExtent = extent;
	CreateImageViews();

	currentFrame = 0;
}

void VulkanRenderer::CreateImageViews()
{
	for (size_t i = 0; i < m_kSwapwhain.swapchainAttachments.size(); i++)
	{
		m_kSwapwhain.swapchainAttachments[i].view = m_kSwapwhain.swapchainAttachments[i].image.CreateImageView(m_kSwapwhain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void VulkanRenderer::CreateDepthAttachment()
{
	VkFormat depthFormat = FindDepthFormat();
	depthAttachement.image.Init(m_kSwapwhain.swapChainExtent.width, m_kSwapwhain.swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	depthAttachement.view = depthAttachement.image.CreateImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	depthAttachement.image.TransitionImageLayout(commandPoolGraphics, graphicsQueue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

void VulkanRenderer::CreateFrameBuffers()
{
	m_akSwapchainFrameBuffers.resize(m_kSwapwhain.swapchainAttachments.size());

	for (size_t i = 0; i < m_kSwapwhain.swapchainAttachments.size(); ++i)
	{
		m_akSwapchainFrameBuffers[i].AddAttachement(m_kSwapwhain.swapchainAttachments[i].view);
		m_akSwapchainFrameBuffers[i].AddAttachement(depthAttachement.view);

		m_akSwapchainFrameBuffers[i].Init(m_kDevice, m_kFinalRenderPass, m_kSwapwhain.swapChainExtent.width, m_kSwapwhain.swapChainExtent.height, 1);
	}
}

void VulkanRenderer::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(m_akSwapchainFrameBuffers.size());
	renderFinishedSemaphores.resize(m_akSwapchainFrameBuffers.size());
	//offscreenRenderSemaphores.resize(m_akSwapchainFrameBuffers.size());
	inFlightFences.resize(m_akSwapchainFrameBuffers.size());

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < m_akSwapchainFrameBuffers.size(); i++) {
		if (vkCreateSemaphore(m_kDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_kDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			//vkCreateSemaphore(m_kDevice, &semaphoreInfo, nullptr, &offscreenRenderSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_kDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void VulkanRenderer::CreateCommandPool()
{
	QueueFamilyIndices indices = m_kPhysicalDevice.GetQueueFamilyIndices();
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_kDevice, &poolInfo, nullptr, &commandPoolGraphics) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create graphics command pool");
	}

	poolInfo.queueFamilyIndex = indices.transferFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	if (vkCreateCommandPool(m_kDevice, &poolInfo, nullptr, &commandPoolTransfer) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create graphics command pool");
	}
}

void VulkanRenderer::CreateCommandBuffers()
{
	commandBuffers = VulkanCommandBuffer::CreateCommandBuffers(m_kDevice, commandPoolGraphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_akSwapchainFrameBuffers.size());
}

void VulkanRenderer::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 2;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 3;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1000;

	if (vkCreateDescriptorPool(m_kDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void VulkanRenderer::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBindingDiffuse = {};
	samplerLayoutBindingDiffuse.binding = 2;
	samplerLayoutBindingDiffuse.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBindingDiffuse.descriptorCount = 1;
	samplerLayoutBindingDiffuse.pImmutableSamplers = nullptr;
	samplerLayoutBindingDiffuse.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	VkDescriptorSetLayoutBinding samplerLayoutBindingAmbient = {};
	samplerLayoutBindingAmbient.binding = 3;
	samplerLayoutBindingAmbient.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBindingAmbient.descriptorCount = 1;
	samplerLayoutBindingAmbient.pImmutableSamplers = nullptr;
	samplerLayoutBindingAmbient.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerNormalMap = {};
	samplerNormalMap.binding = 5;
	samplerNormalMap.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerNormalMap.descriptorCount = 1;
	samplerNormalMap.pImmutableSamplers = nullptr;
	samplerNormalMap.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBindingOpacity = {};
	samplerLayoutBindingOpacity.binding = 4;
	samplerLayoutBindingOpacity.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBindingOpacity.descriptorCount = 1;
	samplerLayoutBindingOpacity.pImmutableSamplers = nullptr;
	samplerLayoutBindingOpacity.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	

	std::vector<VkDescriptorSetLayoutBinding> bindings = 
	{ 
		uboLayoutBinding, 
		samplerLayoutBinding,
		samplerLayoutBindingDiffuse,
		samplerLayoutBindingAmbient,
		samplerLayoutBindingOpacity,
		samplerNormalMap
	};
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_kDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void VulkanRenderer::CreateFinalRenderPass()
{
	m_kFinalRenderPass.AddAttachment(
		ATTACHMENT_COLOR, m_kSwapwhain.swapChainImageFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);

	m_kFinalRenderPass.AddAttachment(
		ATTACHMENT_DEPTH, FindDepthFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);

	m_kFinalRenderPass.AddSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, { 0 }, { 0 }, {});

	m_kFinalRenderPass.AddSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	m_kFinalRenderPass.AddSubpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);

	m_kFinalRenderPass.Init(m_kDevice);
}

void VulkanRenderer::CreateOverlaysRenderPass()
{
	m_kOverlaysRenderPass.AddAttachment(
		ATTACHMENT_COLOR, m_kSwapwhain.swapChainImageFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);

	m_kOverlaysRenderPass.AddAttachment(
		ATTACHMENT_DEPTH, FindDepthFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);

	m_kOverlaysRenderPass.AddSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, { 0 }, 0, {});

	m_kOverlaysRenderPass.AddSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	m_kOverlaysRenderPass.AddSubpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);

	m_kOverlaysRenderPass.Init(m_kDevice);
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}


	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];

}

VkPresentModeKHR VulkanRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else 
	{
		VkExtent2D actualExtent = { (size_t)width, (size_t)height};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkFormat VulkanRenderer::FindDepthFormat()
{
	return m_kPhysicalDevice.FindSupportedFormats({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void VulkanRenderer::InspectSparseProperties(const VkPhysicalDeviceProperties & props)
{
	if (ImGui::CollapsingHeader("Sparse properties"))
	{
		if (props.sparseProperties.residencyStandard2DBlockShape) ImGui::BulletText("residencyStandard2DBlockShape");
		if (props.sparseProperties.residencyStandard2DMultisampleBlockShape) ImGui::BulletText("residencyStandard2DMultisampleBlockShape");
		if (props.sparseProperties.residencyStandard3DBlockShape) ImGui::BulletText("residencyStandard3DBlockShape");
		if (props.sparseProperties.residencyAlignedMipSize) ImGui::BulletText("residencyAlignedMipSize");
		if (props.sparseProperties.residencyNonResidentStrict) ImGui::BulletText("residencyNonResidentStrict");
	}
}

void VulkanRenderer::InspectLimits(const VkPhysicalDeviceProperties & props)
{
	
	if (ImGui::CollapsingHeader("Limits"))
	{
		ImGui::BulletText("maxImageDimension1D:\t%u", props.limits.maxImageDimension1D);
		ImGui::BulletText("maxImageDimension2D:\t%u", props.limits.maxImageDimension2D);
		ImGui::BulletText("maxImageDimension3D:\t%u", props.limits.maxImageDimension3D);
		ImGui::BulletText("maxImageDimensionCube:\t%u", props.limits.maxImageDimensionCube);
		ImGui::BulletText("maxImageArrayLayers:\t%u", props.limits.maxImageArrayLayers);
		ImGui::BulletText("maxTexelBufferElements:\t%u", props.limits.maxTexelBufferElements);
		ImGui::BulletText("maxUniformBufferRange:\t%u", props.limits.maxUniformBufferRange);
		ImGui::BulletText("maxStorageBufferRange:\t%u", props.limits.maxStorageBufferRange);
		ImGui::BulletText("maxPushConstantsSize:\t%u", props.limits.maxPushConstantsSize);
		ImGui::BulletText("maxMemoryAllocationCount:\t%u", props.limits.maxMemoryAllocationCount);
		ImGui::BulletText("maxSamplerAllocationCount:\t%u", props.limits.maxSamplerAllocationCount);
		ImGui::BulletText("bufferImageGranularity:\t%u", props.limits.bufferImageGranularity);
		ImGui::BulletText("sparseAddressSpaceSize:\t%u", props.limits.sparseAddressSpaceSize);
		ImGui::BulletText("maxBoundDescriptorSets:\t%u", props.limits.maxBoundDescriptorSets);
		ImGui::BulletText("maxPerStageDescriptorSamplers:\t%u", props.limits.maxPerStageDescriptorSamplers);
		ImGui::BulletText("maxPerStageDescriptorUniformBuffers:\t%u", props.limits.maxPerStageDescriptorUniformBuffers);
		ImGui::BulletText("maxPerStageDescriptorStorageBuffers:\t%u", props.limits.maxPerStageDescriptorStorageBuffers);
		ImGui::BulletText("maxPerStageDescriptorSampledImages:\t%u", props.limits.maxPerStageDescriptorSampledImages);
		ImGui::BulletText("maxPerStageDescriptorStorageImages:\t%u", props.limits.maxPerStageDescriptorStorageImages);
		ImGui::BulletText("maxPerStageDescriptorInputAttachments:\t%u", props.limits.maxPerStageDescriptorInputAttachments);
		ImGui::BulletText("maxPerStageResources:\t%u", props.limits.maxPerStageResources);
		ImGui::BulletText("maxDescriptorSetSamplers:\t%u", props.limits.maxDescriptorSetSamplers);
		ImGui::BulletText("maxDescriptorSetUniformBuffers:\t%u", props.limits.maxDescriptorSetUniformBuffers);
		ImGui::BulletText("maxDescriptorSetUniformBuffersDynamic:\t%u", props.limits.maxDescriptorSetUniformBuffersDynamic);
		ImGui::BulletText("maxDescriptorSetStorageBuffers:\t%u", props.limits.maxDescriptorSetStorageBuffers);
		ImGui::BulletText("maxDescriptorSetStorageBuffersDynamic:\t%u", props.limits.maxDescriptorSetStorageBuffersDynamic);
		ImGui::BulletText("maxDescriptorSetSampledImages:\t%u", props.limits.maxDescriptorSetSampledImages);
		ImGui::BulletText("maxDescriptorSetInputAttachments:\t%u", props.limits.maxDescriptorSetInputAttachments);
		ImGui::BulletText("maxVertexInputBindings:\t%u", props.limits.maxVertexInputBindings);
		ImGui::BulletText("maxVertexInputAttributes:\t%u", props.limits.maxVertexInputAttributes);
		ImGui::BulletText("maxVertexInputAttributeOffset:\t%u", props.limits.maxVertexInputAttributeOffset);
		ImGui::BulletText("maxVertexInputBindingStride:\t%u", props.limits.maxVertexInputBindingStride);
		ImGui::BulletText("maxVertexOutputComponents:\t%u", props.limits.maxVertexOutputComponents);
		ImGui::BulletText("maxTessellationGenerationLevel:\t%u", props.limits.maxTessellationGenerationLevel);
		ImGui::BulletText("maxTessellationPatchSize:\t%u", props.limits.maxTessellationPatchSize);
		ImGui::BulletText("maxTessellationControlPerVertexInputComponents:\t%u", props.limits.maxTessellationControlPerVertexInputComponents);
		ImGui::BulletText("maxTessellationControlPerVertexOutputComponents:\t%u", props.limits.maxTessellationControlPerVertexOutputComponents);
		ImGui::BulletText("maxTessellationControlPerPatchOutputComponents:\t%u", props.limits.maxTessellationControlPerPatchOutputComponents);
		ImGui::BulletText("maxTessellationControlTotalOutputComponents:\t%u", props.limits.maxTessellationControlTotalOutputComponents);
		ImGui::BulletText("maxTessellationEvaluationInputComponents:\t%u", props.limits.maxTessellationEvaluationInputComponents);
		ImGui::BulletText("maxTessellationEvaluationOutputComponents:\t%u", props.limits.maxTessellationEvaluationOutputComponents);
		ImGui::BulletText("maxGeometryShaderInvocations:\t%u", props.limits.maxGeometryShaderInvocations);
		ImGui::BulletText("maxGeometryInputComponents:\t%u", props.limits.maxGeometryInputComponents);
		ImGui::BulletText("maxGeometryOutputComponents:\t%u", props.limits.maxGeometryOutputComponents);
		ImGui::BulletText("maxGeometryOutputVertices:\t%u", props.limits.maxGeometryOutputVertices);
		ImGui::BulletText("maxGeometryTotalOutputComponents:\t%u", props.limits.maxGeometryTotalOutputComponents);
		ImGui::BulletText("maxFragmentInputComponents:\t%u", props.limits.maxFragmentInputComponents);
		ImGui::BulletText("maxFragmentOutputAttachments:\t%u", props.limits.maxFragmentOutputAttachments);
		ImGui::BulletText("maxFragmentDualSrcAttachments:\t%u", props.limits.maxFragmentDualSrcAttachments);
		ImGui::BulletText("maxFragmentCombinedOutputResources:\t%u", props.limits.maxFragmentCombinedOutputResources);
		ImGui::BulletText("maxComputeSharedMemorySize:\t%u", props.limits.maxComputeSharedMemorySize);
		ImGui::BulletText("maxComputeWorkGroupCount[0]:\t%u", props.limits.maxComputeWorkGroupCount[0]);
		ImGui::BulletText("maxComputeWorkGroupCount[1]:\t%u", props.limits.maxComputeWorkGroupCount[1]);
		ImGui::BulletText("maxComputeWorkGroupCount[2]:\t%u", props.limits.maxComputeWorkGroupCount[2]);
		ImGui::BulletText("maxComputeWorkGroupInvocations:\t%u", props.limits.maxComputeWorkGroupInvocations);
		ImGui::BulletText("maxComputeWorkGroupSize[0]:\t%u", props.limits.maxComputeWorkGroupSize[0]);
		ImGui::BulletText("maxComputeWorkGroupSize[1]:\t%u", props.limits.maxComputeWorkGroupSize[1]);
		ImGui::BulletText("maxComputeWorkGroupSize[2]:\t%u", props.limits.maxComputeWorkGroupSize[2]);
		ImGui::BulletText("subPixelPrecisionBits:\t%u", props.limits.subPixelPrecisionBits);
		ImGui::BulletText("maxDrawIndexedIndexValue:\t%u", props.limits.maxDrawIndexedIndexValue);
		ImGui::BulletText("maxDrawIndirectCount:\t%u", props.limits.maxDrawIndirectCount);
		ImGui::BulletText("maxSamplerLodBias:\t%f", props.limits.maxSamplerLodBias);
		ImGui::BulletText("maxSamplerAnisotropy:\t%f", props.limits.maxSamplerAnisotropy);
		ImGui::BulletText("maxViewports:\t%u", props.limits.maxViewports);
		ImGui::BulletText("maxViewportDimensions[0]:\t%u", props.limits.maxViewportDimensions[0]);
		ImGui::BulletText("maxViewportDimensions[1]:\t%u", props.limits.maxViewportDimensions[1]);
		ImGui::BulletText("viewportBoundsRange[0]:\t%f", props.limits.viewportBoundsRange[0]);
		ImGui::BulletText("viewportBoundsRange[1]:\t%f", props.limits.viewportBoundsRange[1]);
		ImGui::BulletText("viewportSubPixelBits:\t%u", props.limits.viewportSubPixelBits);
		ImGui::BulletText("minMemoryMapAlignment:\t%u", props.limits.minMemoryMapAlignment);
		ImGui::BulletText("minTexelBufferOffsetAlignment:\t%u", props.limits.minTexelBufferOffsetAlignment);
		ImGui::BulletText("minUniformBufferOffsetAlignment:\t%u", props.limits.minUniformBufferOffsetAlignment);
		ImGui::BulletText("minStorageBufferOffsetAlignment:\t%u", props.limits.minStorageBufferOffsetAlignment);
		ImGui::BulletText("minTexelOffset:\t%i", props.limits.minTexelOffset);
		ImGui::BulletText("maxTexelOffset:\t%u", props.limits.maxTexelOffset);
		ImGui::BulletText("minTexelGatherOffset:\t%i", props.limits.minTexelGatherOffset);
		ImGui::BulletText("maxTexelGatherOffset:\t%u", props.limits.maxTexelGatherOffset);
		ImGui::BulletText("minInterpolationOffset:\t%f", props.limits.minInterpolationOffset);
		ImGui::BulletText("maxInterpolationOffset:\t%f", props.limits.maxInterpolationOffset);
		ImGui::BulletText("subPixelInterpolationOffsetBits:\t%u", props.limits.subPixelInterpolationOffsetBits);
		ImGui::BulletText("maxFramebufferWidth:\t%u", props.limits.maxFramebufferWidth);
		ImGui::BulletText("maxFramebufferHeight:\t%u", props.limits.maxFramebufferHeight);
		ImGui::BulletText("maxFramebufferLayers:\t%u", props.limits.maxFramebufferLayers);

		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.framebufferColorSampleCounts, sstr);
			ImGui::BulletText("framebufferColorSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.framebufferDepthSampleCounts, sstr);
			ImGui::BulletText("framebufferDepthSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.framebufferStencilSampleCounts, sstr);
			ImGui::BulletText("framebufferStencilSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.framebufferNoAttachmentsSampleCounts, sstr);
			ImGui::BulletText("framebufferNoAttachmentsSampleCounts:\t%s", sstr.str().c_str());//
		}
		
		ImGui::BulletText("maxColorAttachments:\t%u", props.limits.maxColorAttachments);

		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.sampledImageColorSampleCounts, sstr);
			ImGui::BulletText("sampledImageColorSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.sampledImageIntegerSampleCounts, sstr);
			ImGui::BulletText("sampledImageIntegerSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.sampledImageDepthSampleCounts, sstr);
			ImGui::BulletText("sampledImageDepthSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.sampledImageStencilSampleCounts, sstr);
			ImGui::BulletText("sampledImageStencilSampleCounts:\t%s", sstr.str().c_str());//
		}
		{
			std::stringstream sstr;
			InspectSampleCounts(props.limits.storageImageSampleCounts, sstr);
			ImGui::BulletText("storageImageSampleCounts:\t%s", sstr.str().c_str());//
		}

		ImGui::BulletText("maxSampleMaskWords:\t%u", props.limits.maxSampleMaskWords);
		ImGui::BulletText("timestampComputeAndGraphics:\t%s", (props.limits.timestampComputeAndGraphics) ? "true" : "false");
		ImGui::BulletText("timestampPeriod:\t%f", props.limits.timestampPeriod);
		ImGui::BulletText("maxClipDistances:\t%u", props.limits.maxClipDistances);
		ImGui::BulletText("maxCullDistances:\t%u", props.limits.maxCullDistances);
		ImGui::BulletText("maxCombinedClipAndCullDistances:\t%u", props.limits.maxCombinedClipAndCullDistances);
		ImGui::BulletText("discreteQueuePriorities:\t%u", props.limits.discreteQueuePriorities);
		ImGui::BulletText("pointSizeRange[0]:\t%f", props.limits.pointSizeRange[0]);
		ImGui::BulletText("pointSizeRange[1]:\t%f", props.limits.pointSizeRange[1]);
		ImGui::BulletText("lineWidthRange[0]:\t%f", props.limits.lineWidthRange[0]);
		ImGui::BulletText("lineWidthRange[1]:\t%f", props.limits.lineWidthRange[1]);
		ImGui::BulletText("pointSizeGranularity:\t%f", props.limits.pointSizeGranularity);
		ImGui::BulletText("lineWidthGranularity:\t%f", props.limits.lineWidthGranularity);
		ImGui::BulletText("strictLines:\t%s", (props.limits.strictLines) ? "true" : "false");
		ImGui::BulletText("standardSampleLocations:\t%s", (props.limits.standardSampleLocations) ? "true" : "false");
		ImGui::BulletText("optimalBufferCopyOffsetAlignment:\t%u", props.limits.optimalBufferCopyOffsetAlignment);
		ImGui::BulletText("optimalBufferCopyRowPitchAlignment:\t%u", props.limits.optimalBufferCopyRowPitchAlignment);
		ImGui::BulletText("nonCoherentAtomSize:\t%u", props.limits.nonCoherentAtomSize);
	}
}

void VulkanRenderer::InspectFeatures(const VkPhysicalDeviceFeatures & feats)
{
	if (ImGui::CollapsingHeader("Features"))
	{
		if (feats.robustBufferAccess) ImGui::BulletText("robustBufferAccess");
		if (feats.fullDrawIndexUint32) ImGui::BulletText("fullDrawIndexUint32");
		if (feats.imageCubeArray) ImGui::BulletText("imageCubeArray");
		if (feats.independentBlend) ImGui::BulletText("independentBlend");
		if (feats.geometryShader) ImGui::BulletText("geometryShader");
		if (feats.tessellationShader) ImGui::BulletText("tessellationShader");
		if (feats.sampleRateShading) ImGui::BulletText("sampleRateShading");
		if (feats.dualSrcBlend) ImGui::BulletText("dualSrcBlend");
		if (feats.logicOp) ImGui::BulletText("logicOp");
		if (feats.multiDrawIndirect) ImGui::BulletText("multiDrawIndirect");
		if (feats.drawIndirectFirstInstance) ImGui::BulletText("drawIndirectFirstInstance");
		if (feats.depthClamp) ImGui::BulletText("depthClamp");
		if (feats.depthBiasClamp) ImGui::BulletText("depthBiasClamp");
		if (feats.fillModeNonSolid) ImGui::BulletText("fillModeNonSolid");
		if (feats.depthBounds) ImGui::BulletText("depthBounds");
		if (feats.wideLines) ImGui::BulletText("wideLines");
		if (feats.largePoints) ImGui::BulletText("largePoints");
		if (feats.alphaToOne) ImGui::BulletText("alphaToOne");
		if (feats.multiViewport) ImGui::BulletText("multiViewport");
		if (feats.samplerAnisotropy) ImGui::BulletText("samplerAnisotropy");
		if (feats.textureCompressionETC2) ImGui::BulletText("textureCompressionETC2");
		if (feats.textureCompressionASTC_LDR) ImGui::BulletText("textureCompressionASTC_LDR");
		if (feats.textureCompressionBC) ImGui::BulletText("textureCompressionBC");
		if (feats.occlusionQueryPrecise) ImGui::BulletText("occlusionQueryPrecise");
		if (feats.pipelineStatisticsQuery) ImGui::BulletText("pipelineStatisticsQuery");
		if (feats.vertexPipelineStoresAndAtomics) ImGui::BulletText("vertexPipelineStoresAndAtomics");
		if (feats.fragmentStoresAndAtomics) ImGui::BulletText("fragmentStoresAndAtomics");
		if (feats.shaderTessellationAndGeometryPointSize) ImGui::BulletText("shaderTessellationAndGeometryPointSize");
		if (feats.shaderImageGatherExtended) ImGui::BulletText("shaderImageGatherExtended");
		if (feats.shaderStorageImageExtendedFormats) ImGui::BulletText("shaderStorageImageExtendedFormats");
		if (feats.shaderStorageImageMultisample) ImGui::BulletText("shaderStorageImageMultisample");
		if (feats.shaderStorageImageReadWithoutFormat) ImGui::BulletText("shaderStorageImageReadWithoutFormat");
		if (feats.shaderStorageImageWriteWithoutFormat) ImGui::BulletText("shaderStorageImageWriteWithoutFormat");
		if (feats.shaderUniformBufferArrayDynamicIndexing) ImGui::BulletText("shaderUniformBufferArrayDynamicIndexing");
		if (feats.shaderSampledImageArrayDynamicIndexing) ImGui::BulletText("shaderSampledImageArrayDynamicIndexing");
		if (feats.shaderStorageBufferArrayDynamicIndexing) ImGui::BulletText("shaderStorageBufferArrayDynamicIndexing");
		if (feats.shaderStorageImageArrayDynamicIndexing) ImGui::BulletText("shaderStorageImageArrayDynamicIndexing");
		if (feats.shaderClipDistance) ImGui::BulletText("shaderClipDistance");
		if (feats.shaderCullDistance) ImGui::BulletText("shaderCullDistance");
		if (feats.shaderFloat64) ImGui::BulletText("shaderFloat64");
		if (feats.shaderInt64) ImGui::BulletText("shaderInt64");
		if (feats.shaderInt16) ImGui::BulletText("shaderInt16");
		if (feats.shaderResourceResidency) ImGui::BulletText("shaderResourceResidency");
		if (feats.shaderResourceMinLod) ImGui::BulletText("shaderResourceMinLod");
		if (feats.sparseBinding) ImGui::BulletText("sparseBinding");
		if (feats.sparseResidencyBuffer) ImGui::BulletText("sparseResidencyBuffer");
		if (feats.sparseResidencyImage2D) ImGui::BulletText("sparseResidencyImage2D");
		if (feats.sparseResidencyImage3D) ImGui::BulletText("sparseResidencyImage3D");
		if (feats.sparseResidency2Samples) ImGui::BulletText("sparseResidency2Samples");
		if (feats.sparseResidency4Samples) ImGui::BulletText("sparseResidency4Samples");
		if (feats.sparseResidency8Samples) ImGui::BulletText("sparseResidency8Samples");
		if (feats.sparseResidency16Samples) ImGui::BulletText("sparseResidency16Samples");
		if (feats.sparseResidencyAliased) ImGui::BulletText("sparseResidencyAliased");
		if (feats.variableMultisampleRate) ImGui::BulletText("variableMultisampleRate");
		if (feats.inheritedQueries) ImGui::BulletText("inheritedQueries");
	}
}

void VulkanRenderer::InspectSampleCounts(const VkSampleCountFlags flags, std::stringstream & sstr)
{
	if (flags & VK_SAMPLE_COUNT_1_BIT) sstr << "1, ";
	if (flags & VK_SAMPLE_COUNT_2_BIT) sstr << "2, ";
	if (flags & VK_SAMPLE_COUNT_4_BIT) sstr << "4, ";
	if (flags & VK_SAMPLE_COUNT_8_BIT) sstr << "8, ";
	if (flags & VK_SAMPLE_COUNT_16_BIT) sstr << "16, ";
	if (flags & VK_SAMPLE_COUNT_32_BIT) sstr << "32, ";
	if (flags & VK_SAMPLE_COUNT_64_BIT) sstr << "64, ";
}

