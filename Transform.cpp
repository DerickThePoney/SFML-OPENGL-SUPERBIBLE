#include "stdafx.h"
#include "Transform.h"

Transform::Transform(vec3 kPosition, quaternion kOrientation)
	: m_kLocalPosition(kPosition), m_kLocalOrientation(kOrientation), m_bUpToDate(false)
{
}

Transform::~Transform()
{
}

void Transform::UpdateWorldSpaceTransform(Transform * pkParent)
{
	if(m_bUpToDate)
	{
		return;
	}

	//set local transform matrix
	mat4 kLocalRotation, kLocalTranslation;
	quaternionToMatrix(m_kLocalOrientation, kLocalRotation);

	kLocalTranslation = translate(m_kLocalPosition);

	//compute the world space position
	m_kWorlspaceTransform = kLocalTranslation * kLocalRotation;

	if (pkParent != nullptr)
	{
		m_kWorlspaceTransform = pkParent->m_kWorlspaceTransform * m_kWorlspaceTransform;
	}
	m_bUpToDate = true;
}

void Transform::Inspect()
{
	static bool showWorldSpace = false;
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::InputFloat3("Local position: ", m_kLocalPosition.GetData())) m_bUpToDate = false;
		if (ImGui::InputFloat4("Local orientation: ", m_kLocalOrientation.GetData())) m_bUpToDate = false;
		ImGui::Checkbox("Show world space matrix", &showWorldSpace);
		if(showWorldSpace)
		{
			if (ImGui::InputFloat3("column 0", m_kWorlspaceTransform[0].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 1", m_kWorlspaceTransform[1].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 2", m_kWorlspaceTransform[2].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 3", m_kWorlspaceTransform[3].GetData())) m_bUpToDate = false;
		}
	}
}
