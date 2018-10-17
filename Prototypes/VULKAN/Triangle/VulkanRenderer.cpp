#include "stdafx.h"
#include "VulkanRenderer.h"


VulkanRenderer VulkanRenderer::instance = {};



VulkanRenderer::VulkanRenderer()
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


void VulkanRenderer::InstanceInit(GLFWwindow* window)
{
	glfwGetWindowSize(window, &width, &height);
	CreateInstance();
	CreateSurface(window);
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();


	CreateFinalRenderPass();
	CreateDescriptorPool();
	CreateDescriptorSetLayout();
	//CreateGraphicsPipeline();
	CreateCommandPool();
	CreateDepthAttachment();
	CreateFrameBuffers();
	CreateSyncObjects();


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

	//VulkanCommandBuffer::Free(commandBuffers, m_kDevice, commandPoolGraphics);

	//m_kPipeline.Destroy(m_kDevice);
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
	CreateFinalRenderPass();
	CreateDepthAttachment();
	CreateFrameBuffers();
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
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
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
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);

	m_kFinalRenderPass.AddAttachment(
		ATTACHMENT_DEPTH, FindDepthFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
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

