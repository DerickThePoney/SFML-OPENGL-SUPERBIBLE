#pragma once

#include "OGLRendering.h"
#include "MeshManager.h"
#include "ProgramManager.h"

struct SkyboxData
{
	SkyboxData()
		: m_akFilenames(std::vector<std::string>({
		"media/textures/skybox/right.jpg",
		"media/textures/skybox/left.jpg",
		"media/textures/skybox/top.jpg",
		"media/textures/skybox/bottom.jpg",
		"media/textures/skybox/front.jpg",
		"media/textures/skybox/back.jpg"
	})), m_kProgram("media/shaders/SkyboxShader.shader")
	{}

	std::vector<std::string> m_akFilenames;
	std::string m_kProgram;

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(m_akFilenames),
			CEREAL_NVP(m_kProgram)
		);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		DEARCHIVE_WITH_DEFAULT(m_akFilenames, 
			std::vector<std::string>({
										"media/textures/skybox/right.jpg",
										"media/textures/skybox/left.jpg",
										"media/textures/skybox/top.jpg",
										"media/textures/skybox/bottom.jpg",
										"media/textures/skybox/front.jpg",
										"media/textures/skybox/back.jpg"
									})
		);

		DEARCHIVE_WITH_DEFAULT(m_kProgram,
				"media/shaders/SkyboxShader.shader"
		);
	}
};

class Skybox
{
public:
	Skybox();
	~Skybox();

	void Initialise();
	void Delete();

	void Draw();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(m_kSkyboxAsset)
		);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		DEARCHIVE_WITH_DEFAULT(m_kSkyboxAsset, "media/skyboxes/default_skybox.skybox");
	}

	void Inspect();
private:
	std::string m_kSkyboxAsset;
	SkyboxData m_kData;

	Mesh* m_pkMesh;
	OGLCubeMap m_kCubeMap;
	OGLProgram* m_pkSkyboxProgram;
};