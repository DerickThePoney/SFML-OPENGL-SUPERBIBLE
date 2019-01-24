#include "stdafx.h"
#include "VulkanHelpers.h"

#include "VulkanSurface.h"

bool VulkanHelperFunctions::IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface & surface, 
	VkPhysicalDeviceProperties& properties, VkPhysicalDeviceFeatures& features, 
	const std::vector<const char*> deviceExtensions)
{
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);

	std::cout << "Device name: " << properties.deviceName << std::endl;

	QueueFamilyIndices indices = FindQueueFamilies(device, surface);

	bool extensionsSupported = CheckDeviceExtensionSupport(device, deviceExtensions);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupportDetails(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}


	return indices.isComplete() && extensionsSupported && swapChainAdequate && features.samplerAnisotropy;
}

QueueFamilyIndices VulkanHelperFunctions::FindQueueFamilies(VkPhysicalDevice device, VulkanSurface & surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	int i = 0;
	for (auto& queueFamily : queueFamilyProperties)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		//specific transfer queue
		if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) > 0 && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			indices.transferFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	if (indices.transferFamily < 0) indices.transferFamily = indices.graphicsFamily;

	return indices;
}

bool VulkanHelperFunctions::CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions)
{
	//check the number of extensions
	uint32_t extensionsCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, extensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : extensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanHelperFunctions::QuerySwapChainSupportDetails(VkPhysicalDevice device, VulkanSurface & surface)
{
	SwapChainSupportDetails details;

	//get capabilities of swap chain
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//get supported formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//get supported present modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
