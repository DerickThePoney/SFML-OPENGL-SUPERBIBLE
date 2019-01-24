#include "stdafx.h"
#include "Triangle.h"
static void check_vk_result(VkResult err)
{
	if (err == 0) return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

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
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	window = glfwCreateWindow(mode->width, mode->height, "Vulkan triangle example", nullptr, nullptr);
	//window = glfwCreateWindow(mode->width, mode->height, "Vulkan triangle example", glfwGetPrimaryMonitor(), nullptr);
	WIDTH = mode->width;
	HEIGHT = mode->height;

	glfwSetKeyCallback(window, HelloTriangleApplication::key_callback);
	glfwSetWindowIconifyCallback(window, window_iconify_callback);
	m_bIsIconified = false;
}

void HelloTriangleApplication::InitVulkan()
{
	VulkanRenderer::Init(window);
	CreateOffscreenRenderPass();
	CreateGraphicsPipeline();
		
		
	CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	CreateUniformBuffer();
	CreateDescriptorSet();
	CreateVertexBuffers();
	CreateCommandBuffers();

	CreateOffscreenCommandBuffers();

	InitImGUI();
}

void HelloTriangleApplication::MainLoop()
{
	std::chrono::high_resolution_clock::time_point start, end;
	start = std::chrono::high_resolution_clock::now();
	end = std::chrono::high_resolution_clock::now();

	const int aveSize = 1024;
	static float durations[aveSize];
	static float aveDurations[aveSize];
	static int idx = 0;
	static float avg = 0;
	static int frm = 0;

	while (!glfwWindowShouldClose(window)) 
	{
		auto elapsed = (end - start);
		//durations[idx] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		if (frm < aveSize)
		{
			durations[idx] = (float) std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
			float mult = (float)frm;
			avg = (mult *avg + durations[idx]) / (float)(++frm);
		}
		else
		{
			avg = avg - (float)durations[idx] / (float)aveSize;
			durations[idx] = (float)std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
			avg = avg + (float)durations[idx] / (float)aveSize;
		}
		aveDurations[idx] = avg;

		//std::cout << "time elapsed " << durations[idx];
		//std::cout << "\ttime elapsed average " << avg << std::endl;

		start = std::chrono::high_resolution_clock::now();
		glfwPollEvents();

		if (m_bIsIconified) continue;

		VulkanRenderer::PrepareFrame();

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		if (width != WIDTH || height != HEIGHT)
		{
			WIDTH = width;
			HEIGHT = height;
			RecreateSwapChain();
		}

		ImGui::Begin("Renderer statistics");
		ImGui::LabelText("Frame duration:", "%f", durations[idx]);
		ImGui::LabelText("Frame duration (ave):", "%f", avg);
		std::stringstream sstr; sstr << "avg " << avg;
		float scalemax = ((avg * 2 < 16000) ? avg * 2 : 16000);
		ImGui::PlotLines("Frame times", durations, aveSize, idx+1, sstr.str().c_str(), 0, scalemax, ImVec2(0, 80));// , "", 0, 1.0f, ImVec2(0, 80));
		ImGui::PlotLines("Average frame times", aveDurations, aveSize, idx + 1, sstr.str().c_str(), 0, scalemax, ImVec2(0, 80));// , "", 0, 1.0f, ImVec2(0, 80));
		ImGui::End();

		VulkanRenderer::Inspect();
		m_pkScene->DrawOverlays();
		
		m_pkScene->Update((float)std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() / 1000000000.0f);
		UpdateUniformBufferData();
		VulkanRenderer::PrepareCommandBuffer(m_pkScene, frustum);
		VulkanRenderer::DrawFrame();
		idx = (idx + 1) % aveSize;

		VulkanRenderer::EndFrame();
		


		end = std::chrono::high_resolution_clock::now();
	}

	vkDeviceWaitIdle(VulkanRenderer::GetDevice());

	ImGui_ImplGlfwVulkan_Shutdown();
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
	static const auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	//std::cout << "time = " << time << std::endl;

	static CameraProgram program;
	static int nbReset = 0;

	if (!program.IsSet())
	{
		program.Set
		(
			glm::vec3(-30.0f, 3.0f, 3.0f), glm::vec3(15.0f, 8.0f, 1.0f), 0.005f,
			glm::vec3(20.0f, 0.0f, -90.0f), glm::vec3(-20.0f, 0.0f, 75.0f), 0.0185f
		);
	}
	
	glm::vec3 camPos, camODir;
	if (program.Update(time, camPos, camODir))
	{
		nbReset = ++nbReset % 5;
		switch (nbReset)
		{
		case 0:
			program.Set
			(
				glm::vec3(-30.0f, 3.0f, 3.0f), glm::vec3(15.0f, 8.0f, 1.0f), 0.005f,
				glm::vec3(20.0f, 0.0f, -90.0f), glm::vec3(-20.0f, 0.0f, 75.0f), 0.0185f
			);
			break;
		case 4:
			program.Set
			(
				glm::vec3(-25.0f, 10.0f, -11.0f), glm::vec3(22.5f, 14.0f, -10.0f), 0.005f,
				glm::vec3(0.0f, 0.0f, 270.0f), glm::vec3(-20.0f, 0.0f, 180.0f), 0.010f
			);
			break;
		case 2:
			program.Set
			(
				glm::vec3(20.0f, 35.0f, -20.0f), glm::vec3(-30.0f, 35.0f, 5.0f), 0.005f,
				glm::vec3(-45.0f, 0.0f, 160.0f), glm::vec3(-45.0f, 0.0f, 300.0f), 0.015f
			);
			break;
		case 3:
			program.Set
			(
				glm::vec3(20.0f, 1.0f, -11.0f), glm::vec3(-25.0f, 6.0f, -9.0f), 0.004f,
				glm::vec3(-10.0f, 0.0f, 80.0f), glm::vec3(0.0f, 0.0f, 160.0f), 0.007f
			);
			break;
		case 1:
			program.Set
			(
				glm::vec3(-20.0f, 3.0f, -1.0f), glm::vec3(20.0f, 3.0f, -1.0f), 0.003f,
				glm::vec3(180.0f, 90.0f, 0.0f), glm::vec3(0.0f, 0.0f, 180.0f), 0.0f
			);
			break;
		}
	}
	
	UniformBufferObject ubo = {};
	ubo.model = glm::mat4();
	//ubo.view = glm::translate(camPos) * glm::orientate4(camO); // glm::lookAt(camPos, glm::vec3(10.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//ubo.view = m_kScene.GetCameraPosition(time / 1000.0f); 
	ubo.view = glm::lookAt(glm::vec3(213.0f,120.0f,-8.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(35.0f), VulkanRenderer::GetSurfaceExtent().width / (float)VulkanRenderer::GetSurfaceExtent().height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1; // necessary Vulkan is Y down
	ubo.time = time;

	frustum.Set(ubo.proj * ubo.view);

	

	UniformBufferOffscreen uboOff = {};
	glm::vec3 lightInvDir = glm::vec3(1, 1, 1);
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-100, 1000, -100, 1000, -100, 2000);
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

void HelloTriangleApplication::CreateCommandBuffer(int i)
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


	m_pkScene->Draw(commandBuffers[i], frustum);


	/*static bool showTestWindow = true;
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	ImGui::ShowTestWindow(&showTestWindow);*/

	ImGui_ImplGlfwVulkan_Render(commandBuffers[currentFrame]);

	//End render pass and command buffer
	commandBuffers[i].EndRenderPass();

	if (commandBuffers[i].EndCommandBuffer() != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void HelloTriangleApplication::Cleanup()
{
	VulkanRenderer::EnsureDeviceIdle();
	//CleanUpSwapChain();
	CleanUpOffscreenPass();

	uniformBuffer.Free(VulkanRenderer::GetDevice());
	uniformBufferOffscreen.Free(VulkanRenderer::GetDevice());


	vkDestroySampler(VulkanRenderer::GetDevice(), textureSampler, nullptr);
	vkDestroySampler(VulkanRenderer::GetDevice(), textureSamplerNoRepeat, nullptr);
	textureImageView.Destroy(VulkanRenderer::GetDevice());
	textureImage.Free();
	m_pkScene->Destroy();
	delete m_pkScene;
	VulkanRenderer::Cleanup();
	

	glfwDestroyWindow(window);

	glfwTerminate();
}


void HelloTriangleApplication::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	HelloTriangleApplication& app = HelloTriangleApplication::getInstance();
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(app.window, GLFW_TRUE);
		break;
	default:
		ImGui_ImplGlfwVulkan_KeyCallback(window, key, scancode, action, mods);
		break;
	}


}

void HelloTriangleApplication::window_iconify_callback(GLFWwindow * window, int iconified)
{
	if (iconified)
	{
		getInstance().m_bIsIconified = true;
		std::cout << "We are iconified" << std::endl;
	}
	else
	{
		getInstance().m_bIsIconified = false;
		std::cout << "We are not iconified" << std::endl;
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
	//m_kPipeline.Initialise(VulkanRenderer::GetSurfaceExtent());

	//Set the shaders
	//m_kPipeline.SetShaders(shaders.size(), shaders.data());
	//m_kPipeline.SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	//m_kPipeline.SetPolygonMode(VK_POLYGON_MODE_LINE, .2f);

	//Create the pipline
	//m_kPipeline.CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), VulkanRenderer::GetRenderPass(0));


	//offscreen
	shaders[0].filepath = "data/shaders/shadowmappingdirectional.vert.spv";
	shaders[1].filepath = "data/shaders/shadowmappingdirectional.frag.spv";

	//Init the pipeline
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
	m_kOffscreenPipeline.Initialise(VkExtent2D{ offscreenPass.width, offscreenPass.height });
	m_kOffscreenPipeline.SetShaders(shaders.size(), shaders.data());
	m_kOffscreenPipeline.SetDynamicStates(dynamicStateEnables.data(), dynamicStateEnables.size());
	m_kOffscreenPipeline.SetDepthBias(true);
	m_kOffscreenPipeline.SetBlendAttachementCount(0);

	m_kOffscreenPipeline.CreatePipeline(&VulkanRenderer::GetDescriptorSetLayout(), offscreenPass.renderPass);
}


void HelloTriangleApplication::CreateVertexBuffers()
{	
	m_pkScene = new SceneTestObjLoading();
	m_pkScene->InitialiseFromFile("data/Models/Sponza/sponza.obj");
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
	textureImage.Init("data/textures/texture.jpg", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}


void HelloTriangleApplication::CreateTextureImageView()
{
	textureImageView = textureImage.CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
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
	samplerInfo.maxLod = 20.0f;

	if (vkCreateSampler(VulkanRenderer::GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create a sampler!");
	}

	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 20.0f;

	if (vkCreateSampler(VulkanRenderer::GetDevice(), &samplerInfo, nullptr, &textureSamplerNoRepeat) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create a sampler!");
	}
}
	
void HelloTriangleApplication::CreateCommandBuffers()
{
	glm::mat4 view = glm::lookAt(glm::vec3(-50.0f, 20.0f, -1.0f), glm::vec3(10.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(75.0f), VulkanRenderer::GetSurfaceExtent().width / (float)VulkanRenderer::GetSurfaceExtent().height, 0.1f, 50000.0f);
	frustum.Set(proj * view);

	commandBuffers = VulkanCommandBuffer::CreateCommandBuffers(VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VulkanRenderer::GetSwapchainBuffers().size());

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

	//m_kPipeline.Destroy(VulkanRenderer::GetDevice());
}

void HelloTriangleApplication::RecreateSwapChain()
{
	VulkanRenderer::RecreateSwapChain(window);
	CreateGraphicsPipeline();
	//CreateCommandBuffers();
}


void HelloTriangleApplication::CreateDescriptorSet()
{
	VkDescriptorSetLayout layouts[] = { VulkanRenderer::GetDescriptorSetLayout() };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = VulkanRenderer::GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	/*if (vkAllocateDescriptorSets(VulkanRenderer::GetDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}*/

	bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	imageInfoDepthShadows = {};
	imageInfoDepthShadows.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	imageInfoDepthShadows.imageView = offscreenPass.depth.view;
	imageInfoDepthShadows.sampler = offscreenPass.depthSampler;

	imageInfoDiffuseColor = {};
	imageInfoDiffuseColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageInfoDiffuseColor.imageView = offscreenPass.depth.view;
	imageInfoDiffuseColor.sampler = textureSampler;

	imageInfoAmbientColor = {};
	imageInfoAmbientColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageInfoAmbientColor.imageView = offscreenPass.depth.view;
	imageInfoAmbientColor.sampler = textureSampler;

	imageInfoOpacityMap = {};
	imageInfoOpacityMap.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageInfoOpacityMap.imageView = offscreenPass.depth.view;
	imageInfoOpacityMap.sampler = textureSampler;

	imageInfoNormalMap = {};
	imageInfoNormalMap.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageInfoNormalMap.imageView = offscreenPass.depth.view;
	imageInfoNormalMap.sampler = textureSampler;

	descriptorWrites = std::vector<VkWriteDescriptorSet>({ {}, {}, {}, {}, {} , {} });

	//descriptorWrites = {};
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
	descriptorWrites[1].pImageInfo = &imageInfoDepthShadows;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = descriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pImageInfo = &imageInfoDiffuseColor;

	descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[3].dstSet = descriptorSet;
	descriptorWrites[3].dstBinding = 3;
	descriptorWrites[3].dstArrayElement = 0;
	descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[3].descriptorCount = 1;
	descriptorWrites[3].pImageInfo = &imageInfoAmbientColor;

	descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[4].dstSet = descriptorSet;
	descriptorWrites[4].dstBinding = 4;
	descriptorWrites[4].dstArrayElement = 0;
	descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[4].descriptorCount = 1;
	descriptorWrites[4].pImageInfo = &imageInfoOpacityMap;

	descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[5].dstSet = descriptorSet;
	descriptorWrites[5].dstBinding = 5;
	descriptorWrites[5].dstArrayElement = 0;
	descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[5].descriptorCount = 1;
	descriptorWrites[5].pImageInfo = &imageInfoNormalMap;

	/*vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	*/
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

	vkUpdateDescriptorSets(VulkanRenderer::GetDevice(), static_cast<uint32_t>(descriptorWritesOff.size()), descriptorWritesOff.data(), 0, NULL);
}



void HelloTriangleApplication::CreateOffscreenRenderPass()
{

	VkFormat fbColorFormat = VK_FORMAT_R8G8B8A8_UNORM;

	// For shadow mapping we only need a depth attachment
	offscreenPass.depth.image.Init(offscreenPass.width, offscreenPass.height, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
	offscreenPass.depth.view = offscreenPass.depth.image.CreateImageView(VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT);


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
	commandBuffersOffscreen = VulkanCommandBuffer::CreateCommandBuffers(VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, static_cast<uint32_t>(VulkanRenderer::GetSwapchainBuffers().size()));

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

		/*commandBuffersOffscreen[i].BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kOffscreenPipeline);

		//commandBuffersOffscreen[i].BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_kOffscreenPipeline.GetLayout(), 0, 1, &descriptorSetOffscreen, 0, nullptr);

		//Draw
		/*commandBuffersOffscreen[i].DrawMesh(m_kMeshPlane);
		//Draw
		commandBuffersOffscreen[i].DrawMesh(m_kMesh);*/
		/*m_kScene.Draw(commandBuffersOffscreen[i], m_kOffscreenPipeline);*/
		

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
	offscreenPass.depth.image.Free();
	offscreenPass.frameBuffer.Destroy(VulkanRenderer::GetDevice());
	vkDestroySampler(VulkanRenderer::GetDevice(), offscreenPass.depthSampler, nullptr);
	vkDestroySemaphore(VulkanRenderer::GetDevice(), offscreenPass.semaphore, nullptr);

	m_kOffscreenPipeline.Destroy();
	offscreenPass.renderPass.Destroy(VulkanRenderer::GetDevice());
}

void HelloTriangleApplication::InitImGUI()
{
	ImGui_ImplGlfwVulkan_Init_Data init_data = {};
	init_data.allocator = nullptr;
	init_data.gpu = VulkanRenderer::GetPhysicalDevice();
	init_data.device = VulkanRenderer::GetDevice();
	init_data.render_pass = VulkanRenderer::GetOverlayRenderPass();
	init_data.pipeline_cache = VK_NULL_HANDLE;
	init_data.descriptor_pool = VulkanRenderer::GetDescriptorPool();
	init_data.check_vk_result = check_vk_result;
	ImGui_ImplGlfwVulkan_Init(window, true, &init_data);

	// Upload Fonts
	{
		VkResult err;
		err = vkResetCommandPool(VulkanRenderer::GetDevice(), VulkanRenderer::GetTranferPool(), 0);
		check_vk_result(err);

		VulkanCommandBuffer buffer = VulkanCommandBuffer::CreateCommandBuffer(VulkanRenderer::GetDevice(), VulkanRenderer::GetGraphicsPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		buffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);


		ImGui_ImplGlfwVulkan_CreateFontsTexture(buffer);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = buffer.GetCommandBuffer();
		buffer.EndCommandBuffer();
		err = vkQueueSubmit(VulkanRenderer::GetGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE);
		check_vk_result(err);

		err = vkDeviceWaitIdle(VulkanRenderer::GetDevice());
		check_vk_result(err);
		ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();
	}
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