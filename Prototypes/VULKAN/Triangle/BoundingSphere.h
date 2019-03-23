#pragma once
#include "Sphere.h"

struct MeshData;
class BoundingSphere : public Sphere
{
public:
	BoundingSphere();
	~BoundingSphere();

	void Set(const MeshData & data, const glm::mat4& objectToWorld);
};

