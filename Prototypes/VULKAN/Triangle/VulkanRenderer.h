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
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"

// Framebuffer for offscreen rendering
struct FrameBufferAttachment
{
	VulkanImage image;
	VulkanImageView view;
};

class RenderPassData 
{
	void Init();
	void Cleanup();

	void SetSize(uint32_t width, uint32_t height);
	void SetFrameBufferAttachement();

private:
	uint32_t width = 4096, height = 4096;
	VulkanFramebuffer frameBuffer;
	std::vector<FrameBufferAttachment> attachements;
	VulkanRenderPass renderPass;
	VkSampler depthSampler;
	VkDescriptorImageInfo descriptor;
	VulkanCommandBuffer commandBuffer;
	
	// Semaphore used to synchronize between offscreen and final scene render pass
	VkSemaphore semaphore = VK_NULL_HANDLE;
};

struct Swapchain
{
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<FrameBufferAttachment> swapchainAttachments;
};

class VulkanRenderer
{
	friend class RenderPassData;

private:
	static VulkanRenderer instance;

public:
	static void Init(GLFWwindow* window);
	static void Cleanup();
	
	static VulkanInstance& GetInstance() { return instance.m_kInstance; }
	static VulkanDevice& GetDevice() { return instance.m_kDevice; }
	static VulkanPhysicalDevice& GetPhysicalDevice() { return instance.m_kPhysicalDevice; }
	static VkFormat& GetSurfaceFormat() { return instance.m_kSwapwhain.swapChainImageFormat; }
	static VkExtent2D& GetSurfaceExtent() { return instance.m_kSwapwhain.swapChainExtent; }
	static VkCommandPool& GetGraphicsPool() { return instance.commandPoolGraphics; }
	static VkCommandPool& GetTranferPool() { return instance.commandPoolTransfer; }
	static VulkanRenderPass& GetRenderPass(uint32_t index) { return instance.m_kFinalRenderPass; }
	static VkDescriptorPool& GetDescriptorPool() { return instance.descriptorPool; }
	static VkDescriptorSetLayout& GetDescriptorSetLayout() { return instance.descriptorSetLayout; }

private:
	VulkanRenderer();
	~VulkanRenderer();

	void InstanceInit(GLFWwindow* window);

	void InstanceCleanup();
	void CleanupSwapChain();



private:

	//VULKAN METHODS
	void CreateInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface(GLFWwindow* window);
	void CreateSwapChain();
	void CreateImageViews();
	void CreateDepthAttachment();
	void CreateFrameBuffers();
	void CreateSyncObjects();
	void CreateCommandPool();
	void CreateDescriptorPool();
	void CreateDescriptorSetLayout();

	void CreateFinalRenderPass();


	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkFormat FindDepthFormat();

private:
	VulkanInstance m_kInstance;
	VulkanSurface m_kSurface;
	VulkanPhysicalDevice m_kPhysicalDevice;
	VulkanDevice m_kDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue transferQueue;

	Swapchain m_kSwapwhain;
	FrameBufferAttachment depthAttachement;
	std::vector<VulkanFramebuffer> m_akSwapchainFrameBuffers;

	VkCommandPool commandPoolGraphics;
	VkCommandPool commandPoolTransfer;

	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;

	VulkanRenderPass m_kFinalRenderPass;

	//Synchronisation primitives
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

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

	int width;
	int height;
};

