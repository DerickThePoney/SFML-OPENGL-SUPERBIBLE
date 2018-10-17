#pragma once
#include "VulkanMesh.h"
#include "AABB.h"
#include "BoundingSphere.h"

class SceneObject
{
public:
	SceneObject();
	~SceneObject();
	
	glm::mat4 transform;
	size_t meshIDX;
	//size_t pipelineIDX;
	size_t materialIDX;
	//VulkanMesh mesh;
	//size_t boundingBox;
	//size_t boundingSphere;
	//int matID;
};

