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

#include "IScene.h"
// Framebuffer for offscreen rendering
struct FramebufferAttachment
{
	VulkanImage image;
	VulkanImageView view;
};

struct FramebufferData
{
	VulkanFramebuffer frameBuffer;
	std::vector<FramebufferAttachment> attachements;
};

/*class RenderPassData 
{
	void Init();
	void Cleanup();

	void SetSize(uint32_t width, uint32_t height);
	void SetFramebufferAttachements(const std::vector<FramebufferData>& framebuffersData);
	void SetFramebufferNumber(uint32_t framebuffersNumber);

	VkSemaphore& GetSemaphore(uint32_t index);
	VulkanFramebuffer& GetFramebuffer(uint32_t index);
	VulkanRenderPass& GetRenderPass();
	uint32_t GetFramebuferNumber();

private:
	uint32_t width = 4096, height = 4096;
	std::vector<FramebufferData> framebuffers;
	VulkanRenderPass renderPass;
	VulkanCommandBuffer commandBuffer;
	
	// Semaphore used to synchronize between offscreen and final scene render pass
	std::vector<VkSemaphore> semaphore;
};*/

struct RenderPassData
{
	VulkanRenderPass renderPass;
	std::vector<VulkanFramebuffer> framebuffers;
	std::vector<VkSemaphore> vulkanSemaphore;
};

struct Swapchain
{
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<FramebufferAttachment> swapchainAttachments;
};

class VulkanRenderer
{
	//friend class RenderPassData;

private:
	static VulkanRenderer instance;

public:
	static void Init(GLFWwindow* window);
	static void Cleanup();
	static void RecreateSwapChain(GLFWwindow* window);
	static void EnsureDeviceIdle();

	static void PrepareFrame();
	static void PrepareCommandBuffer(IScene* scene, const Frustum& frustum);
	static void DrawFrame();
	static void EndFrame();

	static void Inspect();

	

	
	static VulkanInstance& GetInstance() { return instance.m_kInstance; }
	static VulkanDevice& GetDevice() { return instance.m_kDevice; }
	static VulkanPhysicalDevice& GetPhysicalDevice() { return instance.m_kPhysicalDevice; }
	static VkSwapchainKHR& GetSwapChain() { return instance.m_kSwapwhain.swapChain; }
	static VkFormat& GetSurfaceFormat() { return instance.m_kSwapwhain.swapChainImageFormat; }
	static VkExtent2D& GetSurfaceExtent() { return instance.m_kSwapwhain.swapChainExtent; }
	static VkQueue& GetGraphicsQueue() { return instance.graphicsQueue; }
	static VkQueue& GetTransferQueue() { return instance.transferQueue; }
	static VkQueue& GetPresentQueue() { return instance.presentQueue; }
	static VkCommandPool& GetGraphicsPool() { return instance.commandPoolGraphics; }
	static VkCommandPool& GetTranferPool() { return instance.commandPoolTransfer; }
	static VulkanRenderPass& GetRenderPass(uint32_t index) { return instance.m_kFinalRenderPass; }
	static VulkanRenderPass& GetOverlayRenderPass() { return instance.m_kOverlaysRenderPass; }
	static VkDescriptorPool& GetDescriptorPool() { return instance.descriptorPool; }
	static VkDescriptorSetLayout& GetDescriptorSetLayout() { return instance.descriptorSetLayout; }
	static VkFence& GetFence(uint32_t index) { return instance.inFlightFences[index]; }
	static VkSemaphore& GetImageAvailableSemaphore(uint32_t index) { return instance.imageAvailableSemaphores[index]; }
	static VkSemaphore& GetRenderFinishedSemaphores(uint32_t index) { return instance.renderFinishedSemaphores[index]; }
	static std::vector<VulkanFramebuffer>& GetSwapchainBuffers() { return instance.m_akSwapchainFrameBuffers; }
	static void AddRenderPass(VulkanRenderPass& renderPass, std::vector<VulkanFramebuffer>& framebuffers, bool bCreateSemaphore);
	static void SetFinalRenderPassAndFramebuffers(VulkanRenderPass& renderPass, std::vector<VulkanFramebuffer>& framebuffers);

private:
	VulkanRenderer();
	~VulkanRenderer();

	void InstanceInit(GLFWwindow* window);

	void InstanceCleanup();
	void CleanupSwapChain();
	void CleanupAdditionalRenderPasses();
	void InstanceRecreateSwapChain(GLFWwindow* window);
	void InstanceEnsureDeviceIdle();

	void InstancePrepareFrame();
	void InstancePrepareCommandBuffer(IScene* scene, const Frustum& frustum);
	void RenderOverlays();
	void InstanceDrawFrame();
	void InstanceEndFrame();

	void InstanceInspect();



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
	void CreateCommandBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSetLayout();

	void CreateFinalRenderPass();
	void CreateOverlaysRenderPass();


	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkFormat FindDepthFormat();

	void InspectSparseProperties(const VkPhysicalDeviceProperties& props);
	void InspectLimits(const VkPhysicalDeviceProperties& props);
	void InspectFeatures(const VkPhysicalDeviceFeatures& feats);
	void InspectSampleCounts(const VkSampleCountFlags flags, std::stringstream& sstr);

private:
	GLFWwindow* window;

	VulkanInstance m_kInstance;
	VulkanSurface m_kSurface;
	VulkanPhysicalDevice m_kPhysicalDevice;
	VulkanDevice m_kDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue transferQueue;

	Swapchain m_kSwapwhain;
	FramebufferAttachment depthAttachement;
	std::vector<VulkanFramebuffer> m_akSwapchainFrameBuffers;

	VkCommandPool commandPoolGraphics;
	VkCommandPool commandPoolTransfer;

	std::vector<VulkanCommandBuffer> commandBuffers;

	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<RenderPassData> m_akAdditionalRenderPasses;
	VulkanRenderPass m_kFinalRenderPass;
	VulkanRenderPass m_kOverlaysRenderPass;

	//Synchronisation primitives
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_assistant_layer"
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

	//flags
	bool m_bDrawOverlays;
};

