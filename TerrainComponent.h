#pragma once
#include "IComponent.h"
#include "MeshManager.h"
#include "MaterialManager.h"

class TerrainComponent : public IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;

public:

	TerrainComponent(GameObject* pkParent = nullptr);
	~TerrainComponent();

	virtual void Init() override;
	virtual void Inspect() override;

	virtual void Clone(std::shared_ptr<IComponent> pkComponent) override;

	template<class Archive>
	void save(Archive & archive) const
	{
		
	}

	template<class Archive>
	void load(Archive& archive)
	{

	}

private:
	void BuildTerrain();


private:
	F32 m_fSize;
	I32 m_iResolution;
	bool m_bUseHeightMap;


	Mesh* m_pkMesh; 
	Material* m_pkTerrainMaterial;
};

CEREAL_REGISTER_TYPE(TerrainComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponent, TerrainComponent);
