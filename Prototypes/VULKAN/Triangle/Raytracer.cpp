#include "stdafx.h"
#include "Raytracer.h"
#include "Ray.h"
#include "Sphere.h"
#include "Intersections.h"
#include "Timer.h"


Raytracer::Raytracer()
{
}


Raytracer::~Raytracer()
{
}

void Raytracer::Initialize(RaytracingSettings&& settings)
{
	this->settings = settings;

	mImage.Init(
		settings.width, settings.height,
		VkFormat::VK_FORMAT_R8G8B8A8_UNORM,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		false
	);
	mImageDataPtr = new Color[settings.width * settings.height];
	mImageView = mImage.CreateImageView(VkFormat::VK_FORMAT_R8G8B8A8_UNORM, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);
	memset(mImageDataPtr, 0, settings.width * settings.height * sizeof(Color));
}

void Raytracer::Destroy()
{
	mImageView.Destroy(VulkanRenderer::GetDevice());
	mImage.Free();
	delete[] mImageDataPtr;
}

void Raytracer::TraceScene(const std::vector<Sphere>& spheres)
{
	{
		VkEngine::Timer<std::chrono::microseconds::period> t("Raytracing");
		int hitnumber = 0;
		for (int y = 0; y < settings.height; ++y)
		{
			for (int x = 0; x < settings.width; ++x)
			{
				Ray r;
				GenerateRayForPixel((float)x, (float)y, r);
				for (size_t k = 0; k < spheres.size(); ++k)
				{
					if (Intersect(spheres[k], r))
					{
						++hitnumber;
						ShadePixel(y, x);
					}
				}
			}
		}

		std::cout << "sphere hits " << hitnumber << " pixels, i.e " << 100.0f * hitnumber / (settings.width * settings.height) << "% of all pixels\n";
	}
	{
		VkEngine::Timer < std::chrono::microseconds::period> t("Data copying");
		mImage.CopyNewData(settings.width, settings.height, mImageDataPtr, settings.width * settings.height * sizeof(Color));
	}
}

void Raytracer::GenerateRayForPixel(float x, float y, Ray & ray)
{
	float Px = (2.0f * ((x + 0.5f) / (float)settings.width) - 1.0f) * settings.tanfFOVby2 * settings.imageAspectRatio;
	float Py = (1.0f - 2.0f * ((y + 0.5f) / (float)settings.height)) * settings.tanfFOVby2;
	ray.SetOrigin(glm::vec3(0));
	ray.SetDirection(glm::vec3(Px, Py, -1)); // note that this just equal to Vec3f(Px, Py, -1); 
}

void Raytracer::ShadePixel(int x, int y)
{
	mImageDataPtr[x*settings.width + y] = Color{ 0xFF, 0xFF , 0xFF , 0xFF };
}
