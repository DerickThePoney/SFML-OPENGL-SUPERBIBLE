#include "stdafx.h"
#include "Ray.h"

Ray::Ray(const glm::vec3 & origin, const glm::vec3 & direction)
	: mOrigin(origin)
	, mDirection(glm::normalize(direction))	
{
}
