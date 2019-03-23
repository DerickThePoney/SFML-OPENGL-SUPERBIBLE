#include "stdafx.h"
#include "Intersections.h"
#include "AABB.h"
#include "Sphere.h"
#include "Plane.h"
#include "Frustum.h"
#include "Ray.h"

bool Intersect(const AABB& first, const AABB& second)
{
	//x separation
	if (first.min.x > second.max.x || second.min.x > first.max.x)
	{
		return false;
	}

	//y separation
	if (first.min.y > second.max.y || second.min.y > first.max.y)
	{
		return false;
	}

	//z separation
	if (first.min.z > second.max.z || second.min.z > first.max.z)
	{
		return false;
	}

	return true;
}

float Intersect(const AABB& box, const Plane& plane)
{
	glm::vec3 diagMin, diagMax;

	//set min/max  for x
	if (plane.normal.x >= 0)
	{
		diagMin.x = box.min.x;
		diagMax.x = box.max.x;
	}
	else
	{
		diagMin.x = box.max.x;
		diagMax.x = box.min.x;
	}

	//set min/max  for y
	if (plane.normal.y >= 0)
	{
		diagMin.y = box.min.y;
		diagMax.y = box.max.y;
	}
	else
	{
		diagMin.y = box.max.y;
		diagMax.y = box.min.y;
	}

	//set min/max for z
	if (plane.normal.z >= 0)
	{
		diagMin.z = box.min.z;
		diagMax.z = box.max.z;
	}
	else
	{
		diagMin.z = box.max.z;
		diagMax.z = box.min.z;
	}

	//min on positive side of plane box on positive side
	float test = glm::dot(plane.normal, diagMin) + plane.offset;
	if (test >= 0.0f)
		return test;

	//min on nonpos, max on nonneg, intersection
	test = glm::dot(plane.normal, diagMax) + plane.offset;
	if (test >= 0.0f)
		return 0.0f;
	else
		return test;
}

float Intersect(const Sphere & sphere, const Plane & plane)
{
	return (plane.normal.x * sphere.center.x) + (plane.normal.y * sphere.center.y) + (plane.normal.z * sphere.center.z) + plane.offset + sphere.radius;
}

bool Intersect(const AABB& box, const Frustum& frustum)
{
	for (int i = 0; i < 6; ++i)
	{
		const Plane& plane = frustum.GetPlane(i);
		float intersectValue = Intersect(box, plane);
		if (intersectValue < 0.0f) return false;
	}
	return true;
}

bool Intersect(const Sphere & sphere, const Frustum & frustum)
{
	for (auto i = 0; i < 6; i++)
	{
		const Plane& plane = frustum.GetPlane(i);
		if (Intersect(sphere, plane) <= 0)
		{
			return false;
		}
	}
	return true;
}

bool Intersect(const Sphere & sphere, const Ray & ray)
{
	// intermediate Value
	glm::vec3 w = sphere.center - ray.Origin();
	float wsq = glm::dot(w, w);
	float proj = glm::dot(w, ray.Direction());
	float rsq = sphere.radius * sphere.radius;

	if (proj < 0.0f&& wsq > rsq)
		return false;

	return wsq - proj * proj <= rsq;
}
