#include "stdafx.h"
#include "AABB.h"
#include "MeshData.h"


AABB::AABB()
	: min(glm::vec3(0)), max(glm::vec3(0))
{
}


AABB::~AABB()
{
}

void AABB::Set(const MeshData & data, const glm::mat4& objectToWorld)
{
	glm::vec4 vec = objectToWorld * glm::vec4(data.vertices[0].pos, 1.0f);
	min = vec;
	max = vec;
	for (size_t i = 1; i < data.vertices.size(); ++i)
	{
		vec = objectToWorld * glm::vec4(data.vertices[i].pos, 1.0f);
		//x
		if (vec.x < min.x)
		{
			min.x = vec.x;
		}

		if (vec.x > max.x)
		{
			max.x = vec.x;
		}

		//y
		if (vec.y < min.y)
		{
			min.y = vec.y;
		}

		if (vec.y > max.y)
		{
			max.y = vec.y;
		}

		//z
		if (vec.z < min.z)
		{
			min.z = vec.z;
		}

		if (vec.z > max.z)
		{
			max.z = vec.z;
		}
	}
}
