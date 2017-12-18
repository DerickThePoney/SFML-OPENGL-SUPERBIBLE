#pragma once
#include <vector>
#include "Utility.h"
#include "OGLRendering.h"

class Material
{
public:
	Material();
	~Material();

	bool InitFromFiles(const std::vector<std::string>& filenames, const std::vector<GLenum> eShaderTypes);
	bool InitMaterialFromRessource(const std::string& kFilename);
	void Delete();

	void Use();

	bool operator==(const Material& other) { return m_uiMaterialID == other.m_uiMaterialID; }

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kMeshFilename), cereal::make_nvp("MeshID", m_pkMesh->m_uiMeshID));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		UI32 uiMeshID;
		archive(CEREAL_NVP(m_kMeshFilename), cereal::make_nvp("MeshID", uiMeshID));

		if (m_kMeshFilename == "")
		{
			m_pkMesh = MeshManager::Instance()->FindFromID(uiMeshID);
		}
		else
		{
			m_pkMesh = MeshManager::Instance()->InstantiateFromFile(m_kMeshFilename);
		}
	}

	//id stuff
	static UI32 s_uiMaxMaterialID;
	UI32 m_uiMaterialID;
	std::string m_kFilename;

	//underlying handle
	OGLProgram m_kProgram;
		
};

