#include "stdafx.h"
#include "Material.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanShader.h"
#include "VulkanRenderer.h"
#include "MaterialManager.h"

Material::Material()
	: m_iInstancesCreated(0)
{
}


Material::~Material()
{
	//Destroy();
}

void Material::Destroy()
{
	for (size_t i = 0; i < m_kShaderModules.size(); ++i)
	{
		m_kShaderModules[i].Destroy(VulkanRenderer::GetDevice());
	}

	vkDestroyDescriptorSetLayout(VulkanRenderer::GetDevice(), m_kDescriptorsLayout, nullptr);
	vkDestroyDescriptorPool(VulkanRenderer::GetDevice(), m_kDescriptorPool, nullptr);
}

void Material::SetShaders(std::vector<ShadersFileType>& shaders, int iMaxInstances)
{
	m_iMaxInstances = iMaxInstances;
	m_kShadersTypes.resize(shaders.size());
	m_kShadersCode.resize(shaders.size());
	m_kShaderModules.resize(shaders.size());
	//m_kShaderReflectionData.resize(shaders.size());

	for (size_t i = 0; i < shaders.size(); ++i)
	{
		m_kShadersTypes[i].filepath = shaders[i].filepath;
		m_kShadersTypes[i].type = shaders[i].type;

		/** READ THE FILE **/
		std::ifstream file(m_kShadersTypes[i].filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		size_t fileSize = (size_t)file.tellg();
		m_kShadersCode[i].resize(fileSize / sizeof(uint32_t));

		file.seekg(0);
		file.read(reinterpret_cast<char*>(m_kShadersCode[i].data()), fileSize);

		file.close();

		m_kShaderModules[i].Initialise(VulkanRenderer::GetDevice(), m_kShadersTypes[i].type, m_kShadersTypes[i].filepath, fileSize, m_kShadersCode[i]);

		//Reflect(i);
	}	

	CreateDescriptorSetLayout();
}

void Material::CreateDescriptorSetLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::map<VkDescriptorType, VkDescriptorPoolSize> poolSizes;

	for (size_t i = 0; i < m_kShadersCode.size(); ++i)
	{
		spirv_cross::Compiler comp(m_kShadersCode[i]);
		spirv_cross::ShaderResources res = comp.get_shader_resources();
		
		//do the buffers
		for (size_t j = 0; j < res.uniform_buffers.size(); ++j)
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = comp.get_decoration(res.uniform_buffers[j].id, spv::DecorationBinding);
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.descriptorCount = 1;
			binding.stageFlags = ConvertToUsableShaderStage(m_kShadersTypes[i].type);
			binding.pImmutableSamplers = nullptr;

			bindings.push_back(binding);

			VulkanBuffer buf;
			bool found = MaterialManager::Get().GetReservedUniformBufferWithName(res.uniform_buffers[j].name, buf);
			VkDescriptorBufferInfo info;
			info.offset = 0; 
			info.range = sizeof(UniformBufferObject);
			info.buffer = buf;

			VkWriteDescriptorSet d = {};
			d.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			d.dstSet = VK_NULL_HANDLE;
			d.dstBinding = binding.binding;
			d.dstArrayElement = 0;
			d.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			d.descriptorCount = 1;
			d.pBufferInfo = nullptr;// (found) ? &info : nullptr;

			descrWrite.push_back(d);
		}

		if (res.uniform_buffers.size() > 0)
		{
			auto itFind = poolSizes.find(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

			if (itFind == poolSizes.end())
			{
				VkDescriptorPoolSize p = {};
				p.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				p.descriptorCount = res.uniform_buffers.size();
				poolSizes[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] = p;
			}
			else
			{
				poolSizes[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER].descriptorCount += res.uniform_buffers.size();
			}
		}

		//do the combined image sampler
		for (size_t j = 0; j < res.sampled_images.size(); ++j)
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = comp.get_decoration(res.sampled_images[j].id, spv::DecorationBinding);
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.descriptorCount = 1;
			binding.stageFlags = ConvertToUsableShaderStage(m_kShadersTypes[i].type);
			binding.pImmutableSamplers = nullptr;

			bindings.push_back(binding);

			VkWriteDescriptorSet d = {};
			d.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			d.dstSet = VK_NULL_HANDLE;
			d.dstBinding = binding.binding;
			d.dstArrayElement = 0;
			d.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			d.descriptorCount = 1;
			d.pBufferInfo = nullptr;
			descrWrite.push_back(d);
		}


		if (res.sampled_images.size() > 0)
		{
			auto itFind = poolSizes.find(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

			if (itFind == poolSizes.end())
			{
				VkDescriptorPoolSize p = {};
				p.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				p.descriptorCount = res.sampled_images.size();
				poolSizes[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] = p;
			}
			else
			{
				poolSizes[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER].descriptorCount += res.sampled_images.size();
			}
		}


		std::vector <spirv_cross::SpecializationConstant> consts = comp.get_specialization_constants();
		for (size_t j = 0; j < consts.size(); ++j)
		{
			//print(c.id); // The ID of the spec constant, useful for further reflection.
			//print(c.constant_id); // 20

			const spirv_cross::SPIRConstant &value = comp.get_constant(consts[j].id);
			//print(value.scalar_i32()); // 40
			//print(comp.get_name(c.id)); // Const
		}
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(VulkanRenderer::GetDevice(), &layoutInfo, nullptr, &m_kDescriptorsLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	std::vector<VkDescriptorPoolSize> actualSizes;
	for (auto it = poolSizes.begin(); it != poolSizes.end(); ++it)
	{
		actualSizes.push_back(it->second);
	}


	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(actualSizes.size());
	poolInfo.pPoolSizes = actualSizes.data();
	poolInfo.maxSets = m_iMaxInstances;

	if (vkCreateDescriptorPool(VulkanRenderer::GetDevice(), &poolInfo, nullptr, &m_kDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VkShaderStageFlagBits Material::ConvertToUsableShaderStage(SHADER_TYPE type)
{
	switch (type)
	{
	case VERTEX:
		return VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case TESSELATION_CONTROL:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case TESSELATION_EVALUATION:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case GEOMETRY:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case FRAGMENT:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case COMPUTE:
		throw std::runtime_error("Compute shader impossible in graphics pipeline");
		return VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	default:
		return VK_SHADER_STAGE_ALL;
	}
}
