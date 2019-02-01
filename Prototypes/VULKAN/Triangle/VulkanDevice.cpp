#include "stdafx.h"
#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanHelpers.h"


VulkanDevice::VulkanDevice()
{
}


VulkanDevice::~VulkanDevice()
{
}

void VulkanDevice::Initialise(VulkanPhysicalDevice& physicalDevice, 
							bool enableValidationLayers, 
							const std::vector<const char*> validationLayers, 
							const std::vector<const char*> deviceExtensions)
{
	//create queue create info
	QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();

	//find the unique queue families and the nb queues per unique queues
	std::map<int, int> nbQueues = {};

	//add the graphics queue
	nbQueues[indices.graphicsFamily] = (1);

	//add the transfer queue
	auto findIt = nbQueues.find(indices.transferFamily);
	if (findIt == nbQueues.end())
	{
		nbQueues[indices.transferFamily] = (1);
	}
	else
	{
		findIt->second += 1;
	}

	//add the present queue
	findIt = nbQueues.find(indices.presentFamily);
	if (findIt == nbQueues.end())
	{
		nbQueues[indices.presentFamily] = (1);
	}
	else
	{
		findIt->second += 1;
	}
	


	//Fill in the queue create info structures
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	std::vector<float> prio = {};

	for (auto it = nbQueues.begin(); it != nbQueues.end(); ++it)
	{
		if(prio.size() < it->second)
			for (size_t i = prio.size(); i < it->second; ++i) prio.push_back(1.0f);
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = it->first;
		queueCreateInfo.queueCount = it->second;
		queueCreateInfo.pQueuePriorities = prio.data();
		queueCreateInfos.push_back(queueCreateInfo);
	}

	//create used device features struct
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.tessellationShader = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;


	//create the logical device
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount = 0;
	}

	//creation call
	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_kDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to initialise the logical device!");
	}
}

void VulkanDevice::Destroy()
{
	vkDestroyDevice(m_kDevice, nullptr);
}

void VulkanDevice::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue& queue)
{
	vkGetDeviceQueue(m_kDevice, queueFamilyIndex, queueIndex, &queue);
}
