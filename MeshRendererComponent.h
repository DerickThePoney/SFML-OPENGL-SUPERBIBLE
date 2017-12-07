#pragma once
#include "Material.h"
#include "MeshManager.h"

//TODO make an IComponent and have transform and meshrenderer part of it
class MeshRendererComponent 
{
public:
	MeshRendererComponent() : m_pkMesh(nullptr), m_pkMaterial(nullptr) {}

	void Init(Mesh* kMesh, Material* kMaterial);

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(cereal::make_nvp("MeshID", m_pkMesh->m_uiMeshID));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		UI32 uiMeshID;
		archive(cereal::make_nvp("MeshID", uiMeshID));

		m_pkMesh = MeshManager::Instance()->FindFromID(uiMeshID);
	}
	
	Mesh* m_pkMesh;
	Material* m_pkMaterial;
};