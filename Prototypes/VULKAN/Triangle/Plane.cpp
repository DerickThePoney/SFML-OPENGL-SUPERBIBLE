#include "stdafx.h"
#include "Plane.h"


Plane::Plane(const glm::vec4 & data)
	: normal(glm::vec3(data.x, data.y, data.z)), offset(data.w)
{
}

Plane::Plane(float a, float b, float c, float d)
	: normal(glm::normalize(glm::vec3(a,b,c))), offset(d)
{
}

Plane::~Plane()
{
}
