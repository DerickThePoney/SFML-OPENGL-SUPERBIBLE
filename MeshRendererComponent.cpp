#include "stdafx.h"
#include "MeshRendererComponent.h"

void MeshRendererComponent::Init(Mesh kMesh, Material kMaterial)
{
	m_kMaterial = kMaterial;
	m_kMesh = kMesh;
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
		ImGui::LabelText("Mesh ID", "%d", m_kMesh.m_uiMeshID);
		ImGui::LabelText("Material ID", "%d", m_kMaterial.m_uiMaterialID);		
	}
}

