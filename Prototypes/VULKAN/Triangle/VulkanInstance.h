#pragma once


class VulkanInstance
{
	NONCOPYABLE(VulkanInstance);

public:
	VulkanInstance();
	~VulkanInstance();

	void Init(const char* pApplicationName,
		uint32_t uiApplicationVersion,
		const char* pEngineName,
		uint32_t uiEngineVersion,
		uint32_t uiAPIVersion,
		bool enableValidationLayers = false,
		const std::vector<const char*> validationLayers = {});
	
	void Destroy();

	operator VkInstance() { return m_kInstance; }

private:

	void SetupDebugCallback();

	bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);

	std::vector<const char*> getRequiredExtensions();

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);

	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData) {

		std::cerr << "validation layer: " << msg << std::endl;

		return VK_FALSE;
	}



private:
	VkInstance m_kInstance;
	VkDebugReportCallbackEXT m_kDebugCallback;
	bool m_bEnableValidationLayers;
};
