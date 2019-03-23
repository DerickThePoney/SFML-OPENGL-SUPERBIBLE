#pragma once
#include "VulkanRenderer.h"
class Sphere;
class Ray;
class RaytracingScene;

struct RaytracingSettings
{
	int32_t width;
	int32_t height;
	float imageAspectRatio;

	glm::vec3 eyePosition;
	glm::vec3 eyeDirection;
	float FoV;
	float tanfFOVby2;
	float nearPlane;
	float farPlane;


	RaytracingScene* scenePtr;
};

struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	void Initialize(RaytracingSettings&& settings);
	void Destroy();

	void TraceScene(const std::vector<Sphere>& spheres);

	VulkanImageView GetVulkanImageView() const { return mImageView; }


private:
	void GenerateRayForPixel(float x, float y, Ray& ray);
	void ShadePixel(int x, int y);

private:
	RaytracingSettings settings;
	
	VulkanImage mImage;
	VulkanImageView mImageView;
	Color* mImageDataPtr;
};

