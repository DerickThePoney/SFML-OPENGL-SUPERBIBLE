#pragma once

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;
	int transferFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0 && transferFamily >= 0;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


struct VertexData
{
	VertexData() : pos(glm::vec3(0)), normal(glm::vec3(0)), color(glm::vec3(1)), texCoord(glm::vec2(0)){}
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;	

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}


	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = { {}, {}, {}, {}};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexData, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexData, normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(VertexData, color);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(VertexData, texCoord);

		

		return attributeDescriptions;
	}

};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 lightSpace;
};

struct UniformBufferOffscreen {
	glm::mat4 depthMVP;
};


class VulkanSurface;

class VulkanHelperFunctions
{
public:
	static bool IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface& surface, const std::vector<const char*> deviceExtensions);
	static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VulkanSurface& surface);
	static bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions);
	static SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice device, VulkanSurface& surface);
};