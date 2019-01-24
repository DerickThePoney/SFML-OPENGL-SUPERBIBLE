#include "stdafx.h"
#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "spirv_cross.hpp"


VulkanShader::VulkanShader()
{
}


VulkanShader::~VulkanShader()
{
}

void VulkanShader::Initialise(VulkanDevice & kDevice, const SHADER_TYPE & type, const std::string & filename, const size_t filesize, const std::vector<uint32_t>& shaderCode)
{
	/** CHACHE THE DATA **/
	m_eShaderType = type;
	m_kFilename = filename;

	/** CREATE THE SHADER MODULE **/
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = filesize;
	shaderCreateInfo.pCode = shaderCode.data();//reinterpret_cast<uint32_t*>(buffer.data());

	if (vkCreateShaderModule(kDevice, &shaderCreateInfo, nullptr, &m_kShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create shader module!");
	}
}

void VulkanShader::Initialise(VulkanDevice& kDevice, const SHADER_TYPE & type, const std::string & filename)
{
	/** CHACHE THE DATA **/
	m_eShaderType = type;
	m_kFilename = filename;

	/** READ THE FILE **/
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open shader file! " + filename);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

	file.close();

	//std::vector<uint32_t> data(fileSize / sizeof(uint32_t));
	//memcpy(data.data(), buffer.data(), fileSize);


	//Reflect(buffer);

	/** CREATE THE SHADER MODULE **/
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = fileSize;
	shaderCreateInfo.pCode = buffer.data();//reinterpret_cast<uint32_t*>(buffer.data());

	if (vkCreateShaderModule(kDevice, &shaderCreateInfo, nullptr, &m_kShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create shader module!");
	}
}

void VulkanShader::Destroy(VulkanDevice& kDevice)
{
	vkDestroyShaderModule(kDevice, m_kShaderModule, nullptr);
}

void VulkanShader::Reflect(std::vector<uint32_t> shaderCode)
{
	spirv_cross::Compiler comp(std::move(shaderCode));

	spirv_cross::ShaderResources resources = comp.get_shader_resources();
}

