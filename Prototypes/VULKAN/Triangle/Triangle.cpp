#include "stdafx.h"
#include "Triangle.h"


static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}




void HelloTriangleApplication::Run()
{
	TestBlockAlocator();
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
	Allocator::DisplayAlignmentsAndSizes();
}

void HelloTriangleApplication::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan triangle example", nullptr, nullptr);
	glfwSetKeyCallback(window, HelloTriangleApplication::key_callback);
}

void HelloTriangleApplication::InitVulkan()
{
	CreateInstance();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateOffscreenRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateCommandPool();
	CreateDepthResources();
	CreateFrameBuffers();
		
		
	CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	CreateVertexBuffers();
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSet();
	CreateCommandBuffers();
	CreateOffscreenCommandBuffers();
	CreateSyncObjects();
}

void HelloTriangleApplication::MainLoop()
{
	std::chrono::high_resolution_clock::time_point start, end;
	start = std::chrono::high_resolution_clock::now();
	end = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window)) 
	{
		auto elapsed = (end - start);
		//std::cout << "time elapsed " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << std::endl;
		start = std::chrono::high_resolution_clock::now();
		glfwPollEvents();

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		if (width != WIDTH || height != HEIGHT)
		{
			WIDTH = width;
			HEIGHT = height;
			RecreateSwapChain();
		}

		UpdateUniformBufferData();
		DrawFrame();
		end = std::chrono::high_resolution_clock::now();
	}

	vkDeviceWaitIdle(m_kDevice);
}

void HelloTriangleApplication::TestBlockAlocator()
{
	BlockAllocator<8, 4, 16> alloc;

	double* d1 = reinterpret_cast<double*>(alloc.Allocate());
	double* d2 = reinterpret_cast<double*>(alloc.Allocate());
	double* d3 = reinterpret_cast<double*>(alloc.Allocate());
	double* d4 = reinterpret_cast<double*>(alloc.Allocate());
	double* d5 = reinterpret_cast<double*>(alloc.Allocate());
	double* d6 = reinterpret_cast<double*>(alloc.Allocate());

	alloc.Free(d1);
	alloc.Free(d2);
	alloc.Free(d3);
	d1 = reinterpret_cast<double*>(alloc.Allocate());
	d2 = reinterpret_cast<double*>(alloc.Allocate());
	d3 = reinterpret_cast<double*>(alloc.Allocate());
	alloc.Free(d4);
	alloc.Free(d5);
	alloc.Free(d6);
	d4 = reinterpret_cast<double*>(alloc.Allocate());
	d5 = reinterpret_cast<double*>(alloc.Allocate());
	d6 = reinterpret_cast<double*>(alloc.Allocate());

	alloc.Free(d1);
	alloc.Free(d2);
	alloc.Free(d3);
	alloc.Free(d4);
	alloc.Free(d5);
	alloc.Free(d6);
}

void HelloTriangleApplication::UpdateUniformBufferData()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(.1f *time, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::mat4(1.0f);
	ubo.view = glm::lookAt(glm::vec3(0.0f, 3.0f, -10.0f), glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 20.0f);
	ubo.proj[1][1] *= -1; // necessary Vulkan is Y down

	

	UniformBufferOffscreen uboOff = {};
	glm::vec3 lightInvDir = glm::vec3(1, 1, 1);
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	depthProjectionMatrix[1][1] *= -1;
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = ubo.model;

	//uboOff.depthMVP = ubo.proj * ubo.view * ubo.model;
	uboOff.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	ubo.lightSpace =  uboOff.depthMVP;

	void* data;
	uniformBufferOffscreen.MapBuffer(m_kDevice, 0, sizeof(uboOff), 0, &data);
	memcpy(data, &uboOff, sizeof(uboOff));
	uniformBufferOffscreen.UnMapBuffer(m_kDevice);

	
	uniformBuffer.MapBuffer(m_kDevice, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	uniformBuffer.UnMapBuffer(m_kDevice);
}

void HelloTriangleApplication::Cleanup()
{
	CleanUpSwapChain();
	CleanUpOffscreenPass();

	vkDestroyDescriptorPool(m_kDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(m_kDevice, descriptorSetLayout, nullptr);


	for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
	{
		vkDestroySemaphore(m_kDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_kDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_kDevice, offscreenRenderSemaphores[i], nullptr);
		vkDestroyFence(m_kDevice, inFlightFences[i], nullptr);
	}


	uniformBuffer.Free(m_kDevice);
	uniformBufferOffscreen.Free(m_kDevice);

	m_kMesh.Destroy(m_kDevice);
	m_kMeshPlane.Destroy(m_kDevice);

	vkDestroySampler(m_kDevice, textureSampler, nullptr);
	textureImageView.Destroy(m_kDevice);
	textureImage.Free(m_kDevice);


	vkDestroyCommandPool(m_kDevice, commandPoolTransfer, nullptr);
	vkDestroyCommandPool(m_kDevice, commandPool, nullptr);


	m_kDevice.Destroy();

		
	m_kSurface.Destroy(m_kInstance);
	m_kInstance.Destroy();

	glfwDestroyWindow(window);

	glfwTerminate();
}


void HelloTriangleApplication::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	HelloTriangleApplication& app = HelloTriangleApplication::getInstance();
	switch (key)
	{
	case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS)
		{
			
			app.m_kMesh.Destroy(app.m_kDevice);
			app.CreateVertexBuffers();
			app.RecreateSwapChain();
			VulkanCommandBuffer::Free(app.commandBuffersOffscreen, app.m_kDevice, app.commandPool);
			app.CreateOffscreenCommandBuffers();
		}
		break;
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(app.window, GLFW_TRUE);
		break;
	}


}

void HelloTriangleApplication::CreateInstance()
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

void HelloTriangleApplication::PickPhysicalDevice()
{
	m_kPhysicalDevice.PickDevice(m_kInstance, m_kSurface, deviceExtensions);
}

void HelloTriangleApplication::CreateLogicalDevice()
{
	//create queue create info
	QueueFamilyIndices indices = m_kPhysicalDevice.GetQueueFamilyIndices();
	m_kDevice.Initialise(m_kPhysicalDevice, enableValidationLayers, validationLayers, deviceExtensions);


	//get the actual graphics queue handle
	m_kDevice.GetQueue(indices.graphicsFamily, 0, graphicsQueue);
	m_kDevice.GetQueue(indices.presentFamily, 1, presentQueue);
	m_kDevice.GetQueue(indices.transferFamily, 0, transferQueue);
}

void HelloTriangleApplication::CreateSurface()
{
	m_kSurface.Init(m_kInstance, window);
}

void HelloTriangleApplication::CreateSwapChain()
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

	if (vkCreateSwapchainKHR(m_kDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_kDevice, swapChain, &imageCount, nullptr);
	m_akSwapChainImages.resize(imageCount);
	std::vector<VkImage> swapChainImages(imageCount);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_kDevice, swapChain, &imageCount, swapChainImages.data());

	for (uint32_t i = 0; i < imageCount; ++i)
	{
		m_akSwapChainImages[i].Init(swapChainImages[i], extent.width, extent.height,
			surfaceFormat.format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	}


	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void HelloTriangleApplication::CreateImageViews()
{
	m_akSwapChainImageViews.resize(m_akSwapChainImages.size());

	for (size_t i = 0; i < m_akSwapChainImages.size(); i++)
	{
		m_akSwapChainImageViews[i] = m_akSwapChainImages[i].CreateImageView(m_kDevice, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}

}

void HelloTriangleApplication::CreateGraphicsPipeline()
{
	//read the shaders
	std::vector<ShadersFileType> shaders;
	ShadersFileType vert;
	vert.filepath = "shaders/Triangle.vert.spv";
	vert.type = VERTEX;
	ShadersFileType frag;
	frag.filepath = "shaders/Triangle.frag.spv";
	frag.type = FRAGMENT;
	shaders.push_back(vert);
	shaders.push_back(frag);

	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};
	
	//Init the pipeline
	m_kPipeline.Initialise(swapChainExtent);

	//Set the shaders
	m_kPipeline.SetShaders(m_kDevice, shaders.size(), shaders.data());
	m_kPipeline.SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//Create the pipline
	m_kPipeline.CreatePipeline(m_kDevice, &descriptorSetLayout, m_kRenderPass);


	//offscreen
	shaders[0].filepath = "shaders/shadowmappingdirectional.vert.spv";
	shaders[1].filepath = "shaders/shadowmappingdirectional.frag.spv";

	//Init the pipeline
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
	m_kOffscreenPipeline.Initialise(VkExtent2D{ offscreenPass.width, offscreenPass.height });
	m_kOffscreenPipeline.SetShaders(m_kDevice, shaders.size(), shaders.data());
	m_kOffscreenPipeline.SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	m_kOffscreenPipeline.SetDepthBias(true);
	m_kOffscreenPipeline.SetBlendAttachementCount(0);

	m_kOffscreenPipeline.CreatePipeline(m_kDevice, &descriptorSetLayout, offscreenPass.renderPass);
}

void HelloTriangleApplication::CreateRenderPass()
{
	m_kRenderPass.AddAttachment(
		ATTACHMENT_COLOR, swapChainImageFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);

	m_kRenderPass.AddAttachment(
		ATTACHMENT_DEPTH, FindDepthFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);

	m_kRenderPass.AddSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, { 0 }, { 0 }, {});

	m_kRenderPass.AddSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	m_kRenderPass.AddSubpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);

	m_kRenderPass.Init(m_kDevice);

}

void HelloTriangleApplication::CreateFrameBuffers()
{
	m_akSwapchainFrameBuffers.resize(m_akSwapChainImageViews.size());

	for (size_t i = 0; i < m_akSwapChainImageViews.size(); ++i)
	{
		m_akSwapchainFrameBuffers[i].AddAttachement(m_akSwapChainImageViews[i]);
		m_akSwapchainFrameBuffers[i].AddAttachement(depthImageView);

		m_akSwapchainFrameBuffers[i].Init(m_kDevice, m_kRenderPass, swapChainExtent.width, swapChainExtent.height, 1);
	}
}

void HelloTriangleApplication::CreateCommandPool()
{
	QueueFamilyIndices indices = m_kPhysicalDevice.GetQueueFamilyIndices();

	CreateCommandPool(indices.graphicsFamily, &commandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	CreateCommandPool(indices.transferFamily, &commandPoolTransfer, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
}


void HelloTriangleApplication::CreateVertexBuffers()
{	
	ObjectCreator c;
	MeshData data = c.Execute(glm::vec3(-4, -2, -4), glm::vec3(4, 4, 4), .2f);
	m_kMesh.Initialise(m_kPhysicalDevice, m_kDevice, data, commandPoolTransfer, transferQueue);
	const float extent = 10.0f;
	MeshData Plane;
	VertexData v;
	v.pos = glm::vec3(-extent, 0.0f, -extent);
	v.color = glm::vec3(127.0f / 255.0f, 80.0f / 255.0f, 53.0f / 255.0f);
	v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	Plane.vertices.push_back(v);
	v.pos = glm::vec3(extent, 0.0f, -extent);
	v.color = glm::vec3(127.0f / 255.0f, 80.0f / 255.0f, 53.0f / 255.0f);
	v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	Plane.vertices.push_back(v);
	v.pos = glm::vec3(extent, 0.0f, extent);
	v.color = glm::vec3(127.0f / 255.0f, 80.0f / 255.0f, 53.0f / 255.0f);
	v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	Plane.vertices.push_back(v);
	v.pos = glm::vec3(-extent, 0.0f, extent);
	v.color = glm::vec3(127.0f / 255.0f, 80.0f / 255.0f, 53.0f / 255.0f);
	v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	Plane.vertices.push_back(v);
	Plane.indices = { 0, 2, 1, 0, 3, 2 };
	m_kMeshPlane.Initialise(m_kPhysicalDevice, m_kDevice, Plane, commandPoolTransfer, transferQueue);
}

void HelloTriangleApplication::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	uniformBuffer.Init(m_kPhysicalDevice, m_kDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	bufferSize = sizeof(UniformBufferOffscreen);
	uniformBufferOffscreen.Init(m_kPhysicalDevice, m_kDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void HelloTriangleApplication::CreateTextureImage()
{
	textureImage.Init(m_kPhysicalDevice, m_kDevice, commandPoolTransfer, graphicsQueue, transferQueue, "textures/texture.jpg", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}


void HelloTriangleApplication::CreateTextureImageView()
{
	textureImageView = textureImage.CreateImageView(m_kDevice, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void HelloTriangleApplication::CreateTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(m_kDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create a sampler!");
	}
}
	
/**/void HelloTriangleApplication::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();
	depthImage.Init(m_kPhysicalDevice, m_kDevice, swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	depthImageView = depthImage.CreateImageView(m_kDevice, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	depthImage.TransitionImageLayout(m_kDevice, commandPool, graphicsQueue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

void HelloTriangleApplication::CreateCommandBuffers()
{
	commandBuffers = VulkanCommandBuffer::CreateCommandBuffers(m_kDevice, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_akSwapchainFrameBuffers.size());
	//commandBuffersSubpass = VulkanCommandBuffer::CreateCommandBuffers(m_kDevice, commandPool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, swapchainFrameBuffers.size());

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		if (commandBuffers[i].BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkViewport viewport = {};// vks::initializers::viewport((float)width, (float)height, 0.0f, 1.0f);
		viewport.height = (float)swapChainExtent.height;
		viewport.width = (float)swapChainExtent.width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		commandBuffers[i].SetViewport(0, 1, &viewport);

		VkRect2D scissor = {};// vks::initializers::rect2D(width, height, 0, 0);
		scissor.extent = swapChainExtent;
		scissor.offset = VkOffset2D{ 0,0 };
		commandBuffers[i].SetScissors(0, 1, &scissor);

		//start render pass
		std::vector<VkClearValue> clearColor = { {},{} };
		clearColor[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearColor[1].depthStencil = { 1.0f, 0 };
		commandBuffers[i].BeginRenderPass
		(
			m_kRenderPass,
			m_akSwapchainFrameBuffers[i],
			VkOffset2D({ 0, 0 }),
			swapChainExtent,
			clearColor,
			VK_SUBPASS_CONTENTS_INLINE//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
		);

		//commandBuffers[i].ExecuteCommandBuffer(&commandBuffersSubpass[i], 1);
		//Bind the pipeline
		commandBuffers[i].BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipeline);

		//Bind Objects		
		commandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

		//Draw
		commandBuffers[i].DrawMesh(m_kMeshPlane);
		//Draw
		commandBuffers[i].DrawMesh(m_kMesh);
		

		//End render pass and command buffer
		commandBuffers[i].EndRenderPass();

		if (commandBuffers[i].EndCommandBuffer() != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void HelloTriangleApplication::CreateSyncObjects() {
	imageAvailableSemaphores.resize(m_akSwapchainFrameBuffers.size());
	renderFinishedSemaphores.resize(m_akSwapchainFrameBuffers.size());
	offscreenRenderSemaphores.resize(m_akSwapchainFrameBuffers.size());
	inFlightFences.resize(m_akSwapchainFrameBuffers.size());

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < m_akSwapchainFrameBuffers.size(); i++) {
		if (vkCreateSemaphore(m_kDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_kDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_kDevice, &semaphoreInfo, nullptr, &offscreenRenderSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_kDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void HelloTriangleApplication::DrawFrame()
{
	vkWaitForFences(m_kDevice, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(m_kDevice, 1, &inFlightFences[currentFrame]);

	//SetupSubpassBuffer();
	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(m_kDevice, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}


	VkSubmitInfo submitInfoOffscreen = {};
	submitInfoOffscreen.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphoresOff[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStagesOff[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfoOffscreen.waitSemaphoreCount = 1;
	submitInfoOffscreen.pWaitSemaphores = waitSemaphoresOff;
	submitInfoOffscreen.pWaitDstStageMask = waitStagesOff;

	submitInfoOffscreen.commandBufferCount = 1;
	submitInfoOffscreen.pCommandBuffers = commandBuffersOffscreen[imageIndex].GetCommandBuffer();

	VkSemaphore signalSemaphoresOff[] = { offscreenRenderSemaphores[currentFrame] };
	submitInfoOffscreen.signalSemaphoreCount = 1;
	submitInfoOffscreen.pSignalSemaphores = signalSemaphoresOff;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfoOffscreen, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { offscreenRenderSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers[imageIndex].GetCommandBuffer();

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to present swap chain image!");
	}


	/*if (enableValidationLayers)
	{
		vkQueueWaitIdle(presentQueue);
	}*/

	currentFrame = (currentFrame + 1) % commandBuffers.size();
}

void HelloTriangleApplication::CleanUpSwapChain()
{
	/*depthImageView.Destroy(m_kDevice);
	depthImage.Free(m_kDevice);

	for (size_t i = 0; i < m_akSwapchainFrameBuffers.size(); i++) {
		m_akSwapchainFrameBuffers[i].Destroy(m_kDevice);
	}*/

	VulkanCommandBuffer::Free(commandBuffers, m_kDevice, commandPool);

	m_kPipeline.Destroy(m_kDevice);
	/*m_kRenderPass.Destroy(m_kDevice);

	for (size_t i = 0; i < m_akSwapChainImageViews.size(); i++) {
		m_akSwapChainImageViews[i].Destroy(m_kDevice);
	}

	vkDestroySwapchainKHR(m_kDevice, swapChain, nullptr);*/
}

void HelloTriangleApplication::RecreateSwapChain()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;

	vkDeviceWaitIdle(m_kDevice);

	CleanUpSwapChain();
	m_kPhysicalDevice.ResetSwapchainSupportDetails(m_kSurface);

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFrameBuffers();
	CreateCommandBuffers();
	//CreateOffscreenCommandBuffers();
}

void HelloTriangleApplication::CreateDescriptorSetLayout()
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

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_kDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void HelloTriangleApplication::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize,2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 2;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 4;

	if (vkCreateDescriptorPool(m_kDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void HelloTriangleApplication::CreateDescriptorSet()
{
	VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(m_kDevice, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	imageInfo.imageView = offscreenPass.depth.view;
	imageInfo.sampler = offscreenPass.depthSampler;

	std::array<VkWriteDescriptorSet,2> descriptorWrites = {};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(m_kDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	if (vkAllocateDescriptorSets(m_kDevice, &allocInfo, &descriptorSetOffscreen) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfoOff = {};
	bufferInfoOff.buffer = uniformBufferOffscreen;
	bufferInfoOff.offset = 0;
	bufferInfoOff.range = sizeof(UniformBufferOffscreen);

	std::array<VkWriteDescriptorSet, 1> descriptorWritesOff = {  };
	descriptorWritesOff[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWritesOff[0].dstSet = descriptorSetOffscreen;
	descriptorWritesOff[0].dstBinding = 0;
	descriptorWritesOff[0].dstArrayElement = 0;
	descriptorWritesOff[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWritesOff[0].descriptorCount = 1;
	descriptorWritesOff[0].pBufferInfo = &bufferInfoOff;

	vkUpdateDescriptorSets(m_kDevice, descriptorWritesOff.size(), descriptorWritesOff.data(), 0, NULL);
}


void HelloTriangleApplication::SetupSubpassBuffer()
{
	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfo.framebuffer = m_akSwapchainFrameBuffers[currentFrame];
	inheritanceInfo.renderPass = m_kRenderPass;

	std::cout << currentFrame << std::endl;
	if (commandBuffersSubpass[currentFrame].BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &inheritanceInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	//Bind the pipeline
	commandBuffersSubpass[currentFrame].BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipeline);

	//Bind Objects		
	commandBuffersSubpass[currentFrame].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

	//Draw
	commandBuffersSubpass[currentFrame].DrawMesh(m_kMesh);

	if (commandBuffersSubpass[currentFrame].EndCommandBuffer() != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}

void HelloTriangleApplication::CreateOffscreenRenderPass()
{
	/*offscreenPass.width = 2048;
	offscreenPass.height = 2048;
*/
	VkFormat fbColorFormat = VK_FORMAT_R8G8B8A8_UNORM;

	// For shadow mapping we only need a depth attachment
	offscreenPass.depth.image.Init(m_kPhysicalDevice, m_kDevice, offscreenPass.width, offscreenPass.height, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	//CreateImage(offscreenPass.width, offscreenPass.height, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, offscreenPass.depth.image, offscreenPass.depth.mem);
	
	offscreenPass.depth.view = offscreenPass.depth.image.CreateImageView(m_kDevice, VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT);


	// Create sampler to sample from to depth attachment 
	// Used to sample in the fragment shader for shadowed rendering
	VkSamplerCreateInfo sampler = {};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.maxAnisotropy = 1.0f;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeV = sampler.addressModeU;
	sampler.addressModeW = sampler.addressModeU;
	sampler.mipLodBias = 0.0f;
	sampler.maxAnisotropy = 1.0f;
	sampler.minLod = 0.0f;
	sampler.maxLod = 1.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	if (vkCreateSampler(m_kDevice, &sampler, nullptr, &offscreenPass.depthSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create a sampler!");
	}

	PrepareOffscreenPass();

	// Create frame buffer
	offscreenPass.frameBuffer.AddAttachement(offscreenPass.depth.view);
	offscreenPass.frameBuffer.Init(m_kDevice, m_kRenderPass, offscreenPass.width, offscreenPass.height, 1);
}
void HelloTriangleApplication::PrepareOffscreenPass()
{
	offscreenPass.renderPass.AddAttachment(ATTACHMENT_DEPTH, VK_FORMAT_D16_UNORM, VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	offscreenPass.renderPass.AddSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, { 0 }, {});
	offscreenPass.renderPass.AddSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	offscreenPass.renderPass.AddSubpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	offscreenPass.renderPass.Init(m_kDevice);
}
void HelloTriangleApplication::CreateOffscreenCommandBuffers()
{
	commandBuffersOffscreen = VulkanCommandBuffer::CreateCommandBuffers(m_kDevice, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_akSwapchainFrameBuffers.size());

	for (size_t i = 0; i < commandBuffersOffscreen.size(); ++i)
	{
		if (commandBuffersOffscreen[i].BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkViewport viewport{};
		viewport.width = (float)offscreenPass.width;
		viewport.height = (float)offscreenPass.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		commandBuffersOffscreen[i].SetViewport(0, 1, &viewport);

		VkRect2D scissor{};
		scissor.extent.width = offscreenPass.width;
		scissor.extent.height = offscreenPass.height;
		scissor.offset.x = 0; 
		scissor.offset.y = 0;
		commandBuffersOffscreen[i].SetScissors(0, 1, &scissor);

		commandBuffersOffscreen[i].SetDepthBias(
			1.25f,
			0.0f,
			1.75f);

		std::vector<VkClearValue> clearValues = { {} };
		clearValues[0].depthStencil.depth = 1.0f;
		commandBuffersOffscreen[i].BeginRenderPass(offscreenPass.renderPass, offscreenPass.frameBuffer, VkOffset2D{ 0,0 }, VkExtent2D{ offscreenPass.width, offscreenPass.height }, clearValues, VK_SUBPASS_CONTENTS_INLINE);

		commandBuffersOffscreen[i].BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kOffscreenPipeline);

		commandBuffersOffscreen[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kOffscreenPipeline.GetLayout(), 0, 1, &descriptorSetOffscreen, 0, nullptr);

		//Draw
		commandBuffersOffscreen[i].DrawMesh(m_kMeshPlane);
		//Draw
		commandBuffersOffscreen[i].DrawMesh(m_kMesh);
		

		commandBuffersOffscreen[i].EndRenderPass();

		if (commandBuffersOffscreen[i].EndCommandBuffer() != VK_SUCCESS)
		{
			throw std::runtime_error("failed to end recording command buffer!");
		}
	}
}
void HelloTriangleApplication::CleanUpOffscreenPass()
{
	VulkanCommandBuffer::Free(commandBuffersOffscreen, m_kDevice, commandPool);
	offscreenPass.depth.view.Destroy(m_kDevice);
	offscreenPass.depth.image.Free(m_kDevice);
	offscreenPass.frameBuffer.Destroy(m_kDevice);

	m_kOffscreenPipeline.Destroy(m_kDevice);
	offscreenPass.renderPass.Destroy(m_kDevice);
}
/////HELPERS
	

VkFormat HelloTriangleApplication::FindDepthFormat()
{
	return m_kPhysicalDevice.FindSupportedFormats({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool HelloTriangleApplication::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT;
}

	
bool HelloTriangleApplication::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

VkSurfaceFormatKHR HelloTriangleApplication::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

VkPresentModeKHR HelloTriangleApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
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

VkExtent2D HelloTriangleApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { (size_t)WIDTH, (size_t)HEIGHT };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkShaderModule HelloTriangleApplication::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = code.size();
	shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		
	VkShaderModule module;
	if (vkCreateShaderModule(m_kDevice, &shaderCreateInfo, nullptr, &module) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create shader module!");
	}

	return module;			
}

void HelloTriangleApplication::CreateCommandPool(int queueFamily, VkCommandPool* commandPool, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamily;
	poolInfo.flags = flags;

	if (vkCreateCommandPool(m_kDevice, &poolInfo, nullptr, commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create graphics command pool");
	}
}

void HelloTriangleApplication::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VulkanCommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
		
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
		
		
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	commandBuffer.PipelineBarrier
	(
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);


	EndSingleTimeCommands(commandBuffer, graphicsQueue, commandPool);
}

void HelloTriangleApplication::CopyBufferToImage(VulkanBuffer& buffer, VkImage image, uint32_t width, uint32_t height) {
	VulkanCommandBuffer commandBuffer = BeginSingleTimeCommands(commandPoolTransfer);
		
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	commandBuffer.CopyBufferToImage(
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	EndSingleTimeCommands(commandBuffer, transferQueue, commandPoolTransfer);
}

void HelloTriangleApplication::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height =height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = imageTiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	const QueueFamilyIndices& indices = m_kPhysicalDevice.GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.transferFamily };
	if (indices.graphicsFamily != indices.transferFamily)
	{
		imageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		imageInfo.queueFamilyIndexCount = 2;
		imageInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}


	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	if (vkCreateImage(m_kDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create image object!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_kDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = m_kPhysicalDevice.FindMemoryType(memRequirements.memoryTypeBits, memoryFlags);

	if (vkAllocateMemory(m_kDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(m_kDevice, image, imageMemory, 0);

}
	
VkImageView HelloTriangleApplication::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectMask;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_kDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

VulkanCommandBuffer HelloTriangleApplication::BeginSingleTimeCommands(VkCommandPool& pool)
{
	VulkanCommandBuffer commandBuffer = VulkanCommandBuffer::CreateCommandBuffer(m_kDevice, pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	
	commandBuffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
	
	return commandBuffer;
}

void HelloTriangleApplication::EndSingleTimeCommands(VulkanCommandBuffer& commandBuffer, VkQueue& queue, VkCommandPool& pool)
{
	commandBuffer.EndCommandBuffer();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer.GetCommandBuffer();

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	VulkanCommandBuffer::Free(commandBuffer, m_kDevice, pool);
}


int main() 
{
		int tries = 0;

		try {
			HelloTriangleApplication::getInstance().Run();
			//break;
		}
		catch (const std::runtime_error& e) {
			std::cerr << e.what() << std::endl;
			++tries;
		}
	

	return EXIT_SUCCESS;
}