#pragma once
struct MeshData;
class BoundingSphere
{
public:
	BoundingSphere();
	~BoundingSphere();

	void Set(const MeshData & data, const glm::mat4& objectToWorld);

	glm::vec3 center;
	float radius;
};

