#include "stdafx.h"
#include "VulkanShader.h"
#include "VulkanDevice.h"


VulkanShader::VulkanShader()
{
}


VulkanShader::~VulkanShader()
{
}

void VulkanShader::Initialise(VulkanDevice& kDevice, const SHADER_TYPE & type, const std::string & filename)
{
	/** CHACHE THE DATA **/
	m_eShaderType = type;
	m_kFilename = filename;

	/** READ THE FILE **/
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	/** CREATE THE SHADER MODULE **/
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = buffer.size();
	shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	if (vkCreateShaderModule(kDevice, &shaderCreateInfo, nullptr, &m_kShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create shader module!");
	}
}

void VulkanShader::Destroy(VulkanDevice& kDevice)
{
	vkDestroyShaderModule(kDevice, m_kShaderModule, nullptr);
}

