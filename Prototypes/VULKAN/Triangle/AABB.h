#pragma once

struct MeshData;
class AABB
{
public:
	AABB();
	~AABB();

	void Set(const MeshData& data, const glm::mat4& objectToWorld);

	glm::vec3 min;
	glm::vec3 max;
};

