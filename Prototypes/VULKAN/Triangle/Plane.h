#pragma once
class Plane
{
public:
	Plane(const glm::vec4& data);
	Plane(float a = 1.0f , float b = 1.0f, float c = 1.0f, float d = 0.0f);
	~Plane();

	glm::vec3 normal;
	float offset;
};

