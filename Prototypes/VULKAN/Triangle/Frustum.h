#pragma once
#include "Plane.h"
class Frustum
{
	enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
public:
	Frustum();
	~Frustum();

	void Set(const glm::mat4& projection);

	const Plane& GetPlane(int idx) const { return planes[idx]; }


private:
	Plane planes[6];
};

