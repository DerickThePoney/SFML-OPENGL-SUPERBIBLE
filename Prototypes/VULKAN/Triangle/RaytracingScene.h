#pragma once
#include "IScene.h"
#include "Raytracer.h"
#include "FullscreenQuadImageRendering.h"
class Sphere;

class RaytracingScene :
	public IScene
{
public:
	RaytracingScene();
	virtual ~RaytracingScene();

	// Hérité via IScene
	virtual void InitialiseFromFile(const std::string & filename) override;
	virtual void Destroy() override;
	virtual void Draw(VulkanCommandBuffer & buffer, const Frustum & frustum) override;
	virtual void Update(float deltaTime) override;
	virtual void DrawOverlays() override;

private:
	std::vector<Sphere> mSpheres;
	Raytracer mRaytracer;
	FullscreenQuadImageRendering mImageRenderer;
};

