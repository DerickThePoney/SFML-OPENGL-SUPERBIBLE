#include "stdafx.h"
#include "MeshRendererComponent.h"

TYPE_SYSTEM_RTTI_OBJECT(MeshRendererComponent, IComponent);

void MeshRendererComponent::Init(Mesh* kMesh, Material* kMaterial)
{
	m_pkMaterial = kMaterial;
	m_pkMesh = kMesh;
}

void MeshRendererComponent::InitMeshFromRessource(const std::string & kFilename)
{
	m_kMeshFilename = kFilename;
	m_pkMesh = MeshManager::Instance()->InstantiateFromFile(m_kMeshFilename);
}

void MeshRendererComponent::Inspect()
{
	static bool showWorldSpace = false;
	/*if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::InputFloat3("Local position: ", m_kLocalPosition.GetData())) m_bUpToDate = false;
		if (ImGui::InputFloat4("Local orientation: ", m_kLocalOrientation.GetData())) m_bUpToDate = false;
		ImGui::Checkbox("Show world space matrix", &showWorldSpace);
		if (showWorldSpace)
		{
			if (ImGui::InputFloat3("column 0", m_kWorlspaceTransform[0].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 1", m_kWorlspaceTransform[1].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 2", m_kWorlspaceTransform[2].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 3", m_kWorlspaceTransform[3].GetData())) m_bUpToDate = false;
		}
	}*/

	if (ImGui::CollapsingHeader("MeshRendererComponent"))
	{
		if(m_pkMesh)	ImGui::LabelText("Mesh ID", "%d", m_pkMesh->m_uiMeshID);
		//ImGui::LabelText("Material ID", "%d", m_kMaterial->m_uiMaterialID);		
		
		if (m_pkMaterial)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			// Node
			std::stringstream sstr;
			sstr << "Material (" << m_pkMaterial->m_kFilename.substr(m_pkMaterial->m_kFilename.find_last_of('/') + 1) << ")";
			bool node_open = ImGui::TreeNodeEx(sstr.str().c_str(), node_flags);
			
			if (node_open)
			{
				m_pkMaterial->Inspect();
				ImGui::TreePop();
			}
		}
	}
}

void MeshRendererComponent::Clone(std::shared_ptr<IComponent> pkComponent)
{
	std::shared_ptr<MeshRendererComponent> pkData = std::dynamic_pointer_cast<MeshRendererComponent>(pkComponent);
	if (pkData == nullptr) return;


	m_kMeshFilename = pkData->m_kMeshFilename;
	m_kMaterialFilename = pkData->m_kMaterialFilename;
	
	m_pkMesh = pkData->m_pkMesh;

	if (pkData->m_pkMaterial->m_kMaterialData.m_bPerInstanceData)
	{
		m_pkMaterial = MaterialManager::Instance()->InstantiateFromFile(m_kMaterialFilename);
	}
	else
	{
		m_pkMaterial = pkData->m_pkMaterial;
	}
}

