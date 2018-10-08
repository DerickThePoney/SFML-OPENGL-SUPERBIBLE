#include "stdafx.h"
#include "BoundingSphere.h"
#include "MeshData.h"


BoundingSphere::BoundingSphere()
{
}


BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::Set(const MeshData & data, const glm::mat4 & objectToWorld)
{
	center = objectToWorld * glm::vec4(data.vertices[0].pos, 1.0f);
	for (size_t i = 1; i < data.vertices.size(); ++i)
	{
		center += glm::vec3(objectToWorld * glm::vec4(data.vertices[i].pos, 1.0f));
	}
	center /= data.vertices.size();

	glm::vec4 center4 = glm::vec4(center, 1.0f);
	radius = 0;
	for (size_t i = 1; i < data.vertices.size(); ++i)
	{
		float dist = glm::length(objectToWorld * glm::vec4(data.vertices[i].pos, 1.0f) - center4);
		if (radius < dist)
			radius = dist;
	}
}
