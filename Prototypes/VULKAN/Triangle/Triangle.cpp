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
	VulkanRenderer::Init(window);
	CreateOffscreenRenderPass();
	CreateGraphicsPipeline();
		
		
	CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	CreateVertexBuffers();
	CreateUniformBuffer();
	CreateDescriptorSet();
	CreateCommandBuffers();
	CreateOffscreenCommandBuffers();
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

	vkDeviceWaitIdle(VulkanRenderer::GetDevice());
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
	ubo.model = glm::mat4();
	ubo.view = glm::lookAt(glm::vec3(-50.0f, 10.0f, -1.0f), glm::vec3(10.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(75.0f), VulkanRenderer::GetSurfaceExtent().width / (float)VulkanRenderer::GetSurfaceExtent().height, 0.1f, 50000.0f);
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
	uniformBufferOffscreen.MapBuffer(VulkanRenderer::GetDevice(), 0, sizeof(uboOff), 0, &data);
	memcpy(data, &uboOff, sizeof(uboOff));
	uniformBufferOffscreen.UnMapBuffer(VulkanRenderer::GetDevice());

	
	uniformBuffer.MapBuffer(VulkanRenderer::GetDevice(), 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	uniformBuffer.UnMapBuffer(VulkanRenderer::GetDevice());
}

void HelloTriangleApplication::Cleanup()
{
	//CleanUpSwapChain();
	CleanUpOffscreenPass();

	uniformBuffer.Free(VulkanRenderer::GetDevice());
	uniformBufferOffscreen.Free(VulkanRenderer::GetDevice());

	m_kScene.Destroy();
	//m_kMesh.Destroy(VulkanRenderer::GetDevice());
	//m_kMeshPlane.Destroy(VulkanRenderer::GetDevice());

	vkDestroySampler(VulkanRenderer::GetDevice(), textureSampler, nullptr);
	textureImageView.Destroy(VulkanRenderer::GetDevice());
	textureImage.Free(VulkanRenderer::GetDevice());
	VulkanRenderer::Cleanup();

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
			app.m_kScene.Destroy();
			//app.m_kMesh.Destroy(VulkanRenderer::GetDevice());
			app.CreateVertexBuffers();
			app.RecreateSwapChain();
			VulkanCommandBuffer::Free(app.commandBuffersOffscreen, VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool());
			app.CreateOffscreenCommandBuffers();
		}
		break;
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(app.window, GLFW_TRUE);
		break;
	}


}


void HelloTriangleApplication::CreateGraphicsPipeline()
{
	//read the shaders
	std::vector<ShadersFileType> shaders;
	ShadersFileType vert;
	vert.filepath = "data/shaders/Triangle.vert.spv";
	vert.type = VERTEX;
	ShadersFileType frag;
	frag.filepath = "data/shaders/Triangle.frag.spv";
	frag.type = FRAGMENT;
	shaders.push_back(vert);
	shaders.push_back(frag);

	std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};
	
	//Init the pipeline
	m_kPipeline.Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	m_kPipeline.SetShaders(VulkanRenderer::GetDevice(), shaders.size(), shaders.data());
	m_kPipeline.SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//Create the pipline
	m_kPipeline.CreatePipeline(VulkanRenderer::GetDevice(), &VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));


	//offscreen
	shaders[0].filepath = "data/shaders/shadowmappingdirectional.vert.spv";
	shaders[1].filepath = "data/shaders/shadowmappingdirectional.frag.spv";

	//Init the pipeline
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
	m_kOffscreenPipeline.Initialise(VkExtent2D{ offscreenPass.width, offscreenPass.height });
	m_kOffscreenPipeline.SetShaders(VulkanRenderer::GetDevice(), shaders.size(), shaders.data());
	m_kOffscreenPipeline.SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	m_kOffscreenPipeline.SetDepthBias(true);
	m_kOffscreenPipeline.SetBlendAttachementCount(0);

	m_kOffscreenPipeline.CreatePipeline(VulkanRenderer::GetDevice(), &VulkanRenderer::GetDescriptorSetLayout(), offscreenPass.renderPass);
}


void HelloTriangleApplication::CreateVertexBuffers()
{	
	m_kScene.InitialiseFromFile("data/Models/Sponza/sponza.obj");
	//ObjectCreator c;
	//MeshData data = c.Execute(glm::vec3(-4, -2, -4), glm::vec3(4, 4, 4), .2f);
	/*m_kMesh.Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), data, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());

	Scene scene;
	scene.InitialiseFromFile("data/Models/Sponza/sponza.obj");
	scene.Destroy();
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
	m_kMeshPlane.Initialise(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), Plane, VulkanRenderer::GetTranferPool(), VulkanRenderer::GetTransferQueue());*/
}

void HelloTriangleApplication::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	uniformBuffer.Init(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	bufferSize = sizeof(UniformBufferOffscreen);
	uniformBufferOffscreen.Init(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void HelloTriangleApplication::CreateTextureImage()
{
	textureImage.Init(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), VulkanRenderer::GetTranferPool(), VulkanRenderer::GetGraphicsQueue(), VulkanRenderer::GetTransferQueue(), "data/textures/texture.jpg", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}


void HelloTriangleApplication::CreateTextureImageView()
{
	textureImageView = textureImage.CreateImageView(VulkanRenderer::GetDevice(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
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

	if (vkCreateSampler(VulkanRenderer::GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create a sampler!");
	}
}
	
void HelloTriangleApplication::CreateCommandBuffers()
{
	commandBuffers = VulkanCommandBuffer::CreateCommandBuffers(VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VulkanRenderer::GetSwapchainBuffers().size());

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		if (commandBuffers[i].BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkViewport viewport = {};// vks::initializers::viewport((float)width, (float)height, 0.0f, 1.0f);
		viewport.height = (float)VulkanRenderer::GetSurfaceExtent().height;
		viewport.width = (float)VulkanRenderer::GetSurfaceExtent().width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		commandBuffers[i].SetViewport(0, 1, &viewport);

		VkRect2D scissor = {};// vks::initializers::rect2D(width, height, 0, 0);
		scissor.extent = VulkanRenderer::GetSurfaceExtent();
		scissor.offset = VkOffset2D{ 0,0 };
		commandBuffers[i].SetScissors(0, 1, &scissor);

		//start render pass
		std::vector<VkClearValue> clearColor = { {},{} };
		clearColor[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearColor[1].depthStencil = { 1.0f, 0 };
		commandBuffers[i].BeginRenderPass
		(
			VulkanRenderer::GetRenderPass(0),
			VulkanRenderer::GetSwapchainBuffers()[i],
			VkOffset2D({ 0, 0 }),
			VulkanRenderer::GetSurfaceExtent(),
			clearColor,
			VK_SUBPASS_CONTENTS_INLINE//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
		);

		//commandBuffers[i].ExecuteCommandBuffer(&commandBuffersSubpass[i], 1);
		//Bind the pipeline
		commandBuffers[i].BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipeline);

		//Bind Objects		
		commandBuffers[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

		//Draw
		//commandBuffers[i].DrawMesh(m_kMeshPlane);
		//Draw
		//commandBuffers[i].DrawMesh(m_kMesh);
		m_kScene.Draw(commandBuffers[i]);

		//End render pass and command buffer
		commandBuffers[i].EndRenderPass();

		if (commandBuffers[i].EndCommandBuffer() != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void HelloTriangleApplication::DrawFrame()
{
	vkWaitForFences(VulkanRenderer::GetDevice(), 1, &VulkanRenderer::GetFence(currentFrame), VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(VulkanRenderer::GetDevice(), 1, &VulkanRenderer::GetFence(currentFrame));

	//SetupSubpassBuffer();
	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(VulkanRenderer::GetDevice(), VulkanRenderer::GetSwapChain(), std::numeric_limits<uint64_t>::max(), VulkanRenderer::GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);

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
	VkSemaphore waitSemaphoresOff[] = { VulkanRenderer::GetImageAvailableSemaphore(currentFrame) };
	VkPipelineStageFlags waitStagesOff[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfoOffscreen.waitSemaphoreCount = 1;
	submitInfoOffscreen.pWaitSemaphores = waitSemaphoresOff;
	submitInfoOffscreen.pWaitDstStageMask = waitStagesOff;

	submitInfoOffscreen.commandBufferCount = 1;
	submitInfoOffscreen.pCommandBuffers = commandBuffersOffscreen[imageIndex].GetCommandBuffer();

	VkSemaphore signalSemaphoresOff[] = { offscreenPass.semaphore };
	submitInfoOffscreen.signalSemaphoreCount = 1;
	submitInfoOffscreen.pSignalSemaphores = signalSemaphoresOff;

	if (vkQueueSubmit(VulkanRenderer::GetGraphicsQueue(), 1, &submitInfoOffscreen, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { offscreenPass.semaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers[imageIndex].GetCommandBuffer();

	VkSemaphore signalSemaphores[] = { VulkanRenderer::GetRenderFinishedSemaphores(currentFrame) };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(VulkanRenderer::GetGraphicsQueue(), 1, &submitInfo, VulkanRenderer::GetFence(currentFrame)) != VK_SUCCESS) {
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
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % commandBuffers.size();
}

void HelloTriangleApplication::CleanUpSwapChain()
{

	VulkanCommandBuffer::Free(commandBuffers, VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool());

	m_kPipeline.Destroy(VulkanRenderer::GetDevice());
}

void HelloTriangleApplication::RecreateSwapChain()
{
	VulkanRenderer::RecreateSwapChain(window);
	CreateGraphicsPipeline();
	CreateCommandBuffers();
}


void HelloTriangleApplication::CreateDescriptorSet()
{
	VkDescriptorSetLayout layouts[] = { VulkanRenderer::GetDescriptorSetLayout() };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = VulkanRenderer::GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(VulkanRenderer::GetDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
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

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	if (vkAllocateDescriptorSets(VulkanRenderer::GetDevice(), &allocInfo, &descriptorSetOffscreen) != VK_SUCCESS) {
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

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), descriptorWritesOff.size(), descriptorWritesOff.data(), 0, NULL);
}



void HelloTriangleApplication::CreateOffscreenRenderPass()
{

	VkFormat fbColorFormat = VK_FORMAT_R8G8B8A8_UNORM;

	// For shadow mapping we only need a depth attachment
	offscreenPass.depth.image.Init(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), offscreenPass.width, offscreenPass.height, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
	offscreenPass.depth.view = offscreenPass.depth.image.CreateImageView(VulkanRenderer::GetDevice(), VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT);


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
	if (vkCreateSampler(VulkanRenderer::GetDevice(), &sampler, nullptr, &offscreenPass.depthSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create a sampler!");
	}

	PrepareOffscreenPass();

	// Create frame buffer
	offscreenPass.frameBuffer.AddAttachement(offscreenPass.depth.view);
	offscreenPass.frameBuffer.Init(VulkanRenderer::GetDevice(), offscreenPass.renderPass, offscreenPass.width, offscreenPass.height, 1);
	
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	if (vkCreateSemaphore(VulkanRenderer::GetDevice(), &semaphoreInfo, nullptr, &offscreenPass.semaphore) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}
	
}
void HelloTriangleApplication::PrepareOffscreenPass()
{
	offscreenPass.renderPass.AddAttachment(ATTACHMENT_DEPTH, VK_FORMAT_D16_UNORM, VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	offscreenPass.renderPass.AddSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, { 0 }, {});
	offscreenPass.renderPass.AddSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT,
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	offscreenPass.renderPass.AddSubpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT,
		VK_DEPENDENCY_BY_REGION_BIT);
	offscreenPass.renderPass.Init(VulkanRenderer::GetDevice());
}
void HelloTriangleApplication::CreateOffscreenCommandBuffers()
{
	commandBuffersOffscreen = VulkanCommandBuffer::CreateCommandBuffers(VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VulkanRenderer::GetSwapchainBuffers().size());

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

		/*//Draw
		commandBuffersOffscreen[i].DrawMesh(m_kMeshPlane);
		//Draw
		commandBuffersOffscreen[i].DrawMesh(m_kMesh);*/
		m_kScene.Draw(commandBuffersOffscreen[i]);
		

		commandBuffersOffscreen[i].EndRenderPass();

		if (commandBuffersOffscreen[i].EndCommandBuffer() != VK_SUCCESS)
		{
			throw std::runtime_error("failed to end recording command buffer!");
		}
	}
}
void HelloTriangleApplication::CleanUpOffscreenPass()
{
	VulkanCommandBuffer::Free(commandBuffersOffscreen, VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool());
	offscreenPass.depth.view.Destroy(VulkanRenderer::GetDevice());
	offscreenPass.depth.image.Free(VulkanRenderer::GetDevice());
	offscreenPass.frameBuffer.Destroy(VulkanRenderer::GetDevice());
	vkDestroySemaphore(VulkanRenderer::GetDevice(), offscreenPass.semaphore, nullptr);

	m_kOffscreenPipeline.Destroy(VulkanRenderer::GetDevice());
	offscreenPass.renderPass.Destroy(VulkanRenderer::GetDevice());
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