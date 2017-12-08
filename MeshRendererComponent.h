#pragma once
#include "Material.h"
#include "MeshManager.h"

//TODO make an IComponent and have transform and meshrenderer part of it
class MeshRendererComponent 
{
public:
	MeshRendererComponent() : m_pkMesh(nullptr), m_pkMaterial(nullptr), m_kMeshFilename("") {}

	void Init(Mesh* kMesh, Material* kMaterial);

	void InitMeshFromRessource(const std::string& kFilename);

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

public:
	std::string m_kMeshFilename;
	Mesh* m_pkMesh;
	Material* m_pkMaterial;
};