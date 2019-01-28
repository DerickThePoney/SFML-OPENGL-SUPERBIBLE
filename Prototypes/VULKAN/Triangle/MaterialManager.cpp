#include "stdafx.h"
#include "MaterialManager.h"
#include "VulkanRenderer.h"

MaterialManager& MaterialManager::instance = MaterialManager();

MaterialManager & MaterialManager::Get()
{
	return instance;
}

MaterialManager::MaterialManager()
{
}


MaterialManager::~MaterialManager()
{
}

void MaterialManager::Initialise()
{
	CreateProjectionBuffer();
}

void MaterialManager::Destroy()
{
	for (auto it = m_akReservedUniformBuffers.begin(); it != m_akReservedUniformBuffers.end(); ++it)
	{
		it->second.Free();
	}
}

bool MaterialManager::GetReservedUniformBufferWithName(const std::string & bufferName, VulkanBuffer & buffer)
{
	auto itFind = m_akReservedUniformBuffers.find(bufferName);
	
	if (itFind != m_akReservedUniformBuffers.end())
	{
		buffer = itFind->second;
		return true;
	}
	return false;
}

void MaterialManager::CreateProjectionBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	VulkanBuffer uniformBuffer;
	uniformBuffer.Init(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_akReservedUniformBuffers["UniformBufferObject"] = uniformBuffer;
	
	/*bufferSize = sizeof(UniformBufferOffscreen);
	uniformBufferOffscreen.Init(VulkanRenderer::GetPhysicalDevice(), VulkanRenderer::GetDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);*/
}
