#include "stdafx.h"
#include "VulkanInstance.h"

VulkanInstance::VulkanInstance()
	: m_bEnableValidationLayers(false)
{
	callbacks.pfnAllocation = &Allocator::Allocate;
	callbacks.pfnReallocation = &Allocator::Reallocate;
	callbacks.pfnFree = &Allocator::Free;
	callbacks.pUserData = nullptr;
}

VulkanInstance::~VulkanInstance()
{
}

void VulkanInstance::Init(const char* pApplicationName,
	uint32_t uiApplicationVersion,
	const char* pEngineName,
	uint32_t uiEngineVersion,
	uint32_t uiAPIVersion,
	bool enableValidationLayers,
	const std::vector<const char*> validationLayers)
{
	//check validation layers
	m_bEnableValidationLayers = enableValidationLayers;
	if (m_bEnableValidationLayers && !checkValidationLayerSupport(validationLayers)) {
		throw std::runtime_error("validation layers requested, but not available!");
	}


	//this application informations
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = pApplicationName;
	appInfo.applicationVersion = uiApplicationVersion;
	appInfo.pEngineName = pEngineName;
	appInfo.engineVersion = uiEngineVersion;
	appInfo.apiVersion = uiAPIVersion;

	//creation info for the instance
	VkInstanceCreateInfo instInfo = {};
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pApplicationInfo = &appInfo;

	//get the required extension to draw onto the surface
	auto extensions = getRequiredExtensions();
	instInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instInfo.ppEnabledExtensionNames = extensions.data();

	if (m_bEnableValidationLayers) {
		instInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		instInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		instInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&instInfo, &callbacks, &m_kInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Instance!");
	}

	//Handle Validation layers callback
	SetupDebugCallback();

	//Enumerate extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensionsProps(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsProps.data());

	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : extensionsProps) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}
}

void VulkanInstance::Destroy()
{
	if (m_bEnableValidationLayers)
	{
		DestroyDebugReportCallbackEXT(m_kInstance, m_kDebugCallback, &callbacks);
	}
	vkDestroyInstance(m_kInstance, &callbacks);
}

void VulkanInstance::SetupDebugCallback()
{
	if (!m_bEnableValidationLayers) return;
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(m_kInstance, &createInfo, &callbacks, &m_kDebugCallback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
}

bool VulkanInstance::checkValidationLayerSupport(const std::vector<const char*> validationLayers)
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

std::vector<const char*> VulkanInstance::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_bEnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

VkResult VulkanInstance::CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugReportCallbackEXT * pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanInstance::DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks * pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

void * Allocator::Allocate(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	void* res = _aligned_malloc(size, alignment);
	std::cout << "Allocating " << size << " bytes with alignement " << alignment << " into " << res << " with scope ";

	switch (allocationScope)
	{
	case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
		std::cout << "VK_SYSTEM_ALLOCATION_SCOPE_COMMAND";
		break;
	case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT:
		std::cout << "VK_SYSTEM_ALLOCATION_SCOPE_OBJECT";
		break;
	case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
		std::cout << "VK_SYSTEM_ALLOCATION_SCOPE_CACHE";
		break;
	case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
		std::cout << "VK_SYSTEM_ALLOCATION_SCOPE_DEVICE";
		break;
	case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
		std::cout << "VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE";
		break;
	}

	std::cout << std::endl;
	return res;
}

void * Allocator::Reallocate(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	std::cout << "Reacclocating " << pOriginal << " to size " << size << " bytes with alignement " << alignment << std::endl;
	if (pOriginal == NULL) return Allocate(pUserData, size, alignment, allocationScope);
	if (size == 0)
	{
		Free(pUserData, pOriginal);
		return NULL;
	}

	size_t originalSize = _aligned_msize(pOriginal, alignment, 0);
	void* newPtr = Allocate(pUserData, size, alignment, allocationScope);
	memcpy(newPtr, pOriginal, std::min(originalSize, size));
	Free(pUserData, pOriginal);

	return newPtr;
}

void Allocator::Free(void * pUserData, void * pMemory)
{
	std::cout << "Free " << pMemory << std::endl;
	_aligned_free(pMemory);
}
