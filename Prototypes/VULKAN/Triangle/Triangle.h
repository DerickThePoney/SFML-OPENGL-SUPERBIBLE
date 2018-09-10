#pragma once
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanMesh.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

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

private:
	

	//VULKAN METHODS
	void CreateInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateGraphicsPipeline();
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateVertexBuffers();
	void CreateUniformBuffer();
	void CreateTextureImage();
	void CreateTextureImageView();
	void CreateTextureSampler();
	void CreateDepthResources();
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void DrawFrame();
	void CleanUpSwapChain();
	void RecreateSwapChain();
	void CreateDescriptorSetLayout();
	void CreateDescriptorPool();
	void CreateDescriptorSet();

	void SetupSubpassBuffer();

	void CreateOffscreenRenderPass();
	void PrepareOffscreenPass();
	void CreateOffscreenCommandBuffers();
	void CleanUpOffscreenPass();
	

	///HELPERS
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateCommandPool(int queueFamily, VkCommandPool* commandPool, VkCommandPoolCreateFlags flags);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VulkanBuffer& buffer, VkImage image, uint32_t width, uint32_t height);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
	VulkanCommandBuffer BeginSingleTimeCommands(VkCommandPool& pool);
	void EndSingleTimeCommands(VulkanCommandBuffer& commandBuffer, VkQueue& queue, VkCommandPool& pool);

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
	VulkanInstance m_kInstance;
	VulkanSurface m_kSurface;
	VulkanPhysicalDevice m_kPhysicalDevice;
	VulkanDevice m_kDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue transferQueue;

	VkSwapchainKHR swapChain;
	std::vector<VulkanImage> m_akSwapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VulkanImageView> m_akSwapChainImageViews;
	VulkanRenderPass m_kRenderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VulkanGraphicsPipeline m_kPipeline;
	VulkanGraphicsPipeline m_kOffscreenPipeline;
	std::vector<VulkanFramebuffer> m_akSwapchainFrameBuffers;

	VulkanMesh m_kMesh;
	VulkanMesh m_kMeshPlane;

	VulkanBuffer uniformBuffer;
	VulkanBuffer uniformBufferOffscreen;

	VkCommandPool commandPool;
	std::vector<VulkanCommandBuffer> commandBuffers;
	std::vector<VulkanCommandBuffer> commandBuffersSubpass;
	std::vector<VulkanCommandBuffer> commandBuffersOffscreen;

	VkCommandPool commandPoolTransfer;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> offscreenRenderSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	VkDescriptorSet descriptorSetOffscreen;

	VulkanImage textureImage;
	VulkanImageView textureImageView;
	VkSampler textureSampler;

	VulkanImage depthImage;
	VulkanImageView depthImageView;

	// Framebuffer for offscreen rendering
	struct FrameBufferAttachment {
		VulkanImage image;
		//VkDeviceMemory mem;
		VulkanImageView view;
	};
	struct OffscreenPass {
		const uint32_t width = 4096, height = 4096;
		VulkanFramebuffer frameBuffer;
		FrameBufferAttachment depth;
		VulkanRenderPass renderPass;
		VkSampler depthSampler;
		VkDescriptorImageInfo descriptor;
		//VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		// Semaphore used to synchronize between offscreen and final scene render pass
		//VkSemaphore semaphore = VK_NULL_HANDLE;
	} offscreenPass;
};