#pragma once

enum SHADER_TYPE
{
	VERTEX,
	TESSELATION_CONTROL,
	TESSELATION_EVALUATION,
	GEOMETRY,
	FRAGMENT,
	COMPUTE
};

class VulkanDevice;
class VulkanShader
{
	//NONCOPYABLE(VulkanShader);

public:
	VulkanShader();
	~VulkanShader();

	void Initialise(VulkanDevice& kDevice, const SHADER_TYPE& type, const std::string& filename, const size_t filesize, const std::vector<uint32_t>& shaderCode);
	void Initialise(VulkanDevice& kDevice, const SHADER_TYPE& type, const std::string& filename);
	void Destroy(VulkanDevice& kDevice);

	operator VkShaderModule() { return m_kShaderModule; }

private:

	void Reflect(std::vector<uint32_t> shaderCode);

	VkShaderModule m_kShaderModule;
	std::string m_kFilename;
	SHADER_TYPE m_eShaderType;
};

