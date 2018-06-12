#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"

#include "VulkanBuffer.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

class HelloTriangleApplication
{
public:
	void Run();
	void InitWindow();
	void InitVulkan();
	void MainLoop();

	void UpdateUniformBufferData();
	void Cleanup();


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
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
	VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& pool);
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue& queue, VkCommandPool& pool);


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
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapchainFrameBuffers;

	VulkanBuffer vertexBuffer;

	VulkanBuffer uniformBuffer;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkCommandPool commandPoolTransfer;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

};