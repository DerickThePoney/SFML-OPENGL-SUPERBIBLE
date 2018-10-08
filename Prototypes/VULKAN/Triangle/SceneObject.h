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
	VulkanMesh mesh;
	AABB boundingBox;
	BoundingSphere boundingSphere;
	int matID;
};

