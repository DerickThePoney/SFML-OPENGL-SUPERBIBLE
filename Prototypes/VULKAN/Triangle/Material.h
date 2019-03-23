#pragma once
#include "spirv_common.hpp"
#include "spirv_cross.hpp"


struct ShadersFileType;
class VulkanShader;
enum SHADER_TYPE;

class Material
{
public:
	Material();
	~Material();

	void Destroy();
	void SetShaders(std::vector<ShadersFileType>& shaders, int iMaxInstances);

	VkDescriptorSet GetDescriptorSet();

private:
	void CreateDescriptorSetLayout();
	VkShaderStageFlagBits ConvertToUsableShaderStage(SHADER_TYPE type);

private:
	VkDescriptorPool m_kDescriptorPool;
	VkDescriptorSetLayout m_kDescriptorsLayout;
	int m_iMaxInstances;
	int m_iInstancesCreated;

	std::vector<ShadersFileType> m_kShadersTypes;
	std::vector<std::vector<uint32_t>> m_kShadersCode;
	std::vector<VulkanShader> m_kShaderModules;
	std::vector<VkWriteDescriptorSet> descrWrite;
};

