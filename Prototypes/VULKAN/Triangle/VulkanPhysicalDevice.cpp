#include "stdafx.h"
#include "VulkanPhysicalDevice.h"

#include "VulkanHelpers.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

VulkanPhysicalDevice::VulkanPhysicalDevice()
{
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

void VulkanPhysicalDevice::PickDevice(VulkanInstance & instance, VulkanSurface & surface, const std::vector<const char*> deviceExtensions)
{
	//enumerate devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Unable to find a suitable device for VULKAN");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	//check for suitable device
	for (auto& device : devices)
	{
		if (VulkanHelperFunctions::IsDeviceSuitable(device, surface, deviceExtensions))
		{
			m_kPhysicalDevice = device;
			m_kIndices = VulkanHelperFunctions::FindQueueFamilies(device, surface);
			m_kSwapChainSupportDetails = VulkanHelperFunctions::QuerySwapChainSupportDetails(device, surface);
			break;
		}
	}

	if (m_kPhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Unable to find a suitable device for VULKAN");
	}
}

void VulkanPhysicalDevice::ResetSwapchainSupportDetails(VulkanSurface & surface)
{
	m_kSwapChainSupportDetails = VulkanHelperFunctions::QuerySwapChainSupportDetails(m_kPhysicalDevice, surface);
}

uint32_t VulkanPhysicalDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_kPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

VkFormat VulkanPhysicalDevice::FindSupportedFormats(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(m_kPhysicalDevice, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

}
