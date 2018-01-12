#include "stdafx.h"
#include "Transform.h"

Transform::Transform(vec3 kPosition, vec3 kScale, quaternion kOrientation)
	: m_kLocalPosition(kPosition), m_kScale(kScale), m_kLocalOrientation(kOrientation), m_bUpToDate(false)
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
	mat4 kLocalRotation, kLocalTranslation, kScale;
	quaternionToMatrix(m_kLocalOrientation, kLocalRotation);

	kLocalTranslation = translate(m_kLocalPosition);

	kScale = scale(m_kScale);

	//compute the world space position
	m_kWorlspaceTransform = kLocalTranslation * kLocalRotation * kScale;

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

		float eulerAngles[3];
		vec4 angleAxis;
		vmath::QuaternionToEulerAngles(m_kLocalOrientation, eulerAngles[0], eulerAngles[1], eulerAngles[2]);
		vmath::ToAxisAngle(m_kLocalOrientation, angleAxis);

		eulerAngles[0] = vmath::degrees(eulerAngles[0]);
		eulerAngles[1] = vmath::degrees(eulerAngles[1]);
		eulerAngles[2] = vmath::degrees(eulerAngles[2]);

		/*std::cout << "Angle:" << vmath::degrees(angleAxis[0]) << "\tAxis:" << angleAxis[1] << "\t"
																			<< angleAxis[2] << "\t"
																			<< angleAxis[3] << "\t" << std::endl;*/

		if (ImGui::InputFloat3("Local orientation: ", eulerAngles))
		{
			eulerAngles[0] = vmath::radians(eulerAngles[0]);
			eulerAngles[1] = vmath::radians(eulerAngles[1]);
			eulerAngles[2] = vmath::radians(eulerAngles[2]);
			m_kLocalOrientation = vmath::QuaternionFromEulerAngles(eulerAngles[0], eulerAngles[1], eulerAngles[2]);
			m_bUpToDate = false;
		}

		if (ImGui::InputFloat3("Local Scale: ", m_kScale.GetData())) m_bUpToDate = false;

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
