#pragma once

class VulkanBuffer;
class MaterialManager
{
public:
	static MaterialManager& Get();

private:
	static MaterialManager& instance;

	MaterialManager();
	~MaterialManager();
	

public:
	void Initialise();
	void Destroy();

	bool GetReservedUniformBufferWithName(const std::string& bufferName, VulkanBuffer& buffer);


private:
	void CreateProjectionBuffer();
private:
	std::unordered_map<std::string, VulkanBuffer> m_akReservedUniformBuffers;
};

