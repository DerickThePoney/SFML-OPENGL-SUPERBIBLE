#include "stdafx.h"
#include "RaytracingScene.h"
#include "Sphere.h"

RaytracingScene::RaytracingScene()
{
}


RaytracingScene::~RaytracingScene()
{
}

void RaytracingScene::InitialiseFromFile(const std::string & filename)
{
	{
		Sphere s;
		s.center = glm::vec3(10.0f, 10.0f, -50.0f);
		s.radius = 1.0f;
		mSpheres.push_back(std::move(s));
	}

	{
		VkExtent2D extent = VulkanRenderer::GetSurfaceExtent();
		RaytracingSettings settings;
		settings.width = extent.width;
		settings.height = extent.height;
		settings.imageAspectRatio = (float)extent.width / (float)extent.height;
		settings.FoV = glm::radians(50.0f);
		settings.tanfFOVby2 = tanf(settings.FoV / 2.0f);
	
		mRaytracer.Initialize(std::move(settings));
	}

	{
		mImageRenderer.Initialise(mRaytracer.GetVulkanImageView());
	}
}

void RaytracingScene::Destroy()
{
	mRaytracer.Destroy();
	mImageRenderer.Destroy();
}

void RaytracingScene::Draw(VulkanCommandBuffer & buffer, const Frustum & frustum)
{
	mImageRenderer.Draw(buffer);
}

void RaytracingScene::Update(float deltaTime)
{
	mRaytracer.TraceScene(mSpheres);
}

void RaytracingScene::DrawOverlays()
{
}
