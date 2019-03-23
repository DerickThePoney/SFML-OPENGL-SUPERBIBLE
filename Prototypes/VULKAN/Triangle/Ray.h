#pragma once

class Ray
{
public:
	Ray() {}
	Ray(const glm::vec3& origin, const glm::vec3& direction);

	const glm::vec3& Origin() const { return mOrigin; }
	const glm::vec3& Direction() const { return mDirection; }

	void SetOrigin(const glm::vec3& origin) { mOrigin = origin; }
	void SetDirection(const glm::vec3& direction) { mDirection = glm::normalize(direction); }

protected:
	glm::vec3 mOrigin;
	glm::vec3 mDirection;
};