#pragma once
#include "MaterialManager.h"
#include "MeshManager.h"

//TODO make an IComponent and have transform and meshrenderer part of it
class MeshRendererComponent 
{
public:
	MeshRendererComponent() : m_pkMesh(nullptr), m_pkMaterial(nullptr), m_kMeshFilename(""), m_kMaterialFilename("") {}
	~MeshRendererComponent() { }

	void Init(Mesh* kMesh, Material* kMaterial);

	void InitMeshFromRessource(const std::string& kFilename);

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kMeshFilename), cereal::make_nvp("MeshID", m_pkMesh->m_uiMeshID), CEREAL_NVP(m_kMaterialFilename));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		UI32 uiMeshID;
		archive(CEREAL_NVP(m_kMeshFilename), cereal::make_nvp("MeshID", uiMeshID), CEREAL_NVP(m_kMaterialFilename));

		if (m_kMeshFilename == "")
		{
			m_pkMesh = MeshManager::Instance()->FindFromID(uiMeshID);
		}
		else
		{
			m_pkMesh = MeshManager::Instance()->InstantiateFromFile(m_kMeshFilename);
		}

		if (m_kMaterialFilename != "")
		{
			m_pkMaterial = MaterialManager::Instance()->InstantiateFromFile(m_kMaterialFilename);
		}
	}

public:
	std::string m_kMeshFilename;
	std::string m_kMaterialFilename;
	Mesh* m_pkMesh;
	Material* m_pkMaterial;
};