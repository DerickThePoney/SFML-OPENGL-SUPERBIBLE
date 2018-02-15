#include "stdafx.h"
#include "TerrainComponent.h"
#include "GameObjectManager.h"

TYPE_SYSTEM_RTTI_OBJECT(TerrainComponent, IComponent);

TerrainComponent::TerrainComponent(GameObject * pkParent)
	: m_fSize(50.0f), m_iResolution(50), m_bUseHeightMap(false), m_pkMesh(nullptr), m_pkTerrainMaterial(nullptr), IComponent(pkParent)
{
}

TerrainComponent::~TerrainComponent()
{
	MeshManager::Instance()->Destroy(m_pkMesh);
	MaterialManager::Instance()->Destroy(m_pkTerrainMaterial);
}

void TerrainComponent::Init()
{
	BuildTerrain();
}



void TerrainComponent::Inspect()
{
	ImGui::SliderFloat("Terrain Size", &m_fSize, 10, 200);
	ImGui::SliderInt("Terrain resolution", &m_iResolution, 2, 2048);
	ImGui::Checkbox("Use heightmap", &m_bUseHeightMap);

	if (ImGui::Button("Rebuild terrain"))
	{
		BuildTerrain();
	}
}

void TerrainComponent::Clone(std::shared_ptr<IComponent> pkComponent)
{
	std::shared_ptr<TerrainComponent> pkData = std::dynamic_pointer_cast<TerrainComponent>(pkComponent);
	if (pkData == nullptr) return;

	m_fSize = pkData->m_fSize;
	m_iResolution = pkData->m_iResolution;
	m_bUseHeightMap = pkData->m_bUseHeightMap;

	Init();
}

void TerrainComponent::BuildTerrain()
{
	//build up a grid for now :-)
	MakeGrid grid(50, 50);
	BMesh bmesh;
	grid.Apply(bmesh);

	if (m_pkMesh != nullptr)
	{
		MeshManager::Instance()->Destroy(m_pkMesh);
	}

	m_pkMesh = MeshManager::Instance()->Instantiate();

	bmesh.BuildMesh(*m_pkMesh);
	m_pkMesh->LoadBuffersOnGraphicsCard();

	std::shared_ptr<MeshRendererComponent> renderer = m_pkParent->FindComponent<MeshRendererComponent>();
	if (renderer == nullptr)
	{
		renderer = m_pkParent->AddComponent<MeshRendererComponent>();
	}

	if (m_pkTerrainMaterial == nullptr)
	{
		m_pkTerrainMaterial = MaterialManager::Instance()->InstantiateFromFile("media/Materials/StandardUnlitMeshMaterial.material");
	}
	
	renderer->m_pkMesh = m_pkMesh;
	renderer->m_pkMaterial = m_pkTerrainMaterial;
}
