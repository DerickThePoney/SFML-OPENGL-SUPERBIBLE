#pragma once
#include "MaterialManager.h"
#include "MeshManager.h"
#include "IComponent.h"
//TODO make an IComponent and have transform and meshrenderer part of it
class MeshRendererComponent : public IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;

public:
	MeshRendererComponent(GameObject* pkParent = nullptr) : m_pkMesh(nullptr), m_pkMaterial(nullptr), m_kMeshFilename(""), m_kMaterialFilename(""), IComponent(pkParent) {}
	~MeshRendererComponent() { }

	void Init(Mesh* kMesh, Material* kMaterial);
	void InitMeshFromRessource(const std::string& kFilename);

	void Inspect();

	// Hérité via IComponent
	virtual void Clone(std::shared_ptr<IComponent> pkComponent) override;

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kMeshFilename), cereal::make_nvp("MeshID", (m_pkMesh != nullptr)?m_pkMesh->m_uiMeshID : -1), CEREAL_NVP(m_kMaterialFilename));

		if (m_pkMaterial != nullptr)
		{
			if (m_pkMaterial->m_kMaterialData.m_bPerInstanceData)
			{
				archive(cereal::make_nvp("InstanceData", m_pkMaterial->m_kMaterialData.m_akDataForShader));
			}
		}
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

			if (m_pkMaterial->m_kMaterialData.m_bPerInstanceData)
			{
				std::vector<IUniformDataContainerPtr> akDataForShader;
				DEARCHIVE_WITH_DEFAULT_FROM_NVP(akDataForShader, "InstanceData", std::vector<std::shared_ptr<IUniformDataContainer>>());
				if (akDataForShader.size() > 0)
				{
					m_pkMaterial->m_kMaterialData.m_akDataForShader = akDataForShader;
				}
			}
		}
	}

public:
	std::string m_kMeshFilename;
	std::string m_kMaterialFilename;
	Mesh* m_pkMesh;
	Material* m_pkMaterial;

	
};

CEREAL_REGISTER_TYPE(MeshRendererComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponent, MeshRendererComponent);