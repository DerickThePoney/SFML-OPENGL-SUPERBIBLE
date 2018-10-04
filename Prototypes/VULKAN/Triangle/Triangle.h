#pragma once
#include "VulkanRenderer.h"
#include "Scene.h"

#include "ObjectCreator.h"

using namespace PolyVox;

const int MAX_FRAMES_IN_FLIGHT = 3;

class HelloTriangleApplication
{
public:
	void Run();
	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void TestBlockAlocator();

	void UpdateUniformBufferData();
	void Cleanup();

	static HelloTriangleApplication& getInstance() // Singleton is accessed via getInstance()
	{
		static HelloTriangleApplication instance; // lazy singleton, instantiated on first use
		return instance;
	}
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static std::array<VkWriteDescriptorSet, 3> GetDescriptorWrites() { return getInstance().descriptorWrites; }
private:
	

	//VULKAN METHODS
	
	void CreateGraphicsPipeline();
	void CreateVertexBuffers();
	void CreateUniformBuffer();
	void CreateTextureImage();
	void CreateTextureImageView();
	void CreateTextureSampler();
	void CreateCommandBuffers();
	void DrawFrame();
	void CleanUpSwapChain();
	void RecreateSwapChain();
	void CreateDescriptorSet();

	void CreateOffscreenRenderPass();
	void PrepareOffscreenPass();
	void CreateOffscreenCommandBuffers();
	void CleanUpOffscreenPass();
	

	
	//void createSphereInVolume(SimpleVolume<float>& volData, Vector3DFloat v3dVolCenter, float fRadius, float cellSize);
private:
	//window stuff
	int WIDTH = 800;
	int HEIGHT = 600;
	

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif



	GLFWwindow* window;

	//Vulkan Data
	
	VulkanGraphicsPipeline m_kPipeline;
	VulkanGraphicsPipeline m_kOffscreenPipeline;
	

	//VulkanMesh m_kMesh;
	//VulkanMesh m_kMeshPlane;
	Scene m_kScene;

	VulkanBuffer uniformBuffer;
	VulkanBuffer uniformBufferOffscreen;

	std::vector<VulkanCommandBuffer> commandBuffers;
	std::vector<VulkanCommandBuffer> commandBuffersSubpass;
	std::vector<VulkanCommandBuffer> commandBuffersOffscreen;

	size_t currentFrame = 0;

	VkDescriptorSet descriptorSet;
	VkDescriptorSet descriptorSetOffscreen;
	VkDescriptorBufferInfo bufferInfo;
	VkDescriptorImageInfo imageInfo;
	VkDescriptorImageInfo imageInfo2;
	std::array<VkWriteDescriptorSet, 3> descriptorWrites;

	VulkanImage textureImage;
	VulkanImageView textureImageView;
	VkSampler textureSampler;

	// Framebuffer for offscreen rendering
	struct OffscreenPass {
		const uint32_t width = 4096, height = 4096;
		VulkanFramebuffer frameBuffer;
		FramebufferAttachment depth;
		VulkanRenderPass renderPass;
		VkSampler depthSampler;
		VkDescriptorImageInfo descriptor;
		//VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		// Semaphore used to synchronize between offscreen and final scene render pass
		VkSemaphore semaphore = VK_NULL_HANDLE;
	} offscreenPass;
};