#include "stdafx.h"
#include "Transform.h"

Transform::Transform(vec3 kPosition, float kScale, quaternion kOrientation)
	: localPosition({ kPosition, kScale, kOrientation }), m_bUpToDate(false)
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

	if (pkParent == nullptr)
	{
		worldPosition = localPosition;
		return;
	}

	worldPosition.m_fScale = pkParent->worldPosition.m_fScale * localPosition.m_fScale;
	worldPosition.m_kOrientation = normalize(localPosition.m_kOrientation * pkParent->worldPosition.m_kOrientation);
	worldPosition.m_kPosition = localPosition.m_kPosition + vmath::Rotate(localPosition.m_kOrientation, localPosition.m_fScale * pkParent->worldPosition.m_kPosition);

	m_bUpToDate = true;
}

const mat4 Transform::GetWorldSpaceTransform() const
{
	return vmath::translate(worldPosition.m_kPosition) * worldPosition.m_kOrientation.asMatrix() * vmath::scale(worldPosition.m_fScale);
}

void Transform::Inspect()
{
	static bool showWorldSpace = false;
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::InputFloat3("Local position: ", localPosition.m_kPosition.GetData())) m_bUpToDate = false;

		float eulerAngles[3];
		vec4 angleAxis;
		vmath::QuaternionToEulerAngles(localPosition.m_kOrientation, eulerAngles[0], eulerAngles[1], eulerAngles[2]);
		vmath::ToAxisAngle(localPosition.m_kOrientation, angleAxis);

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
			localPosition.m_kOrientation = vmath::QuaternionFromEulerAngles(eulerAngles[0], eulerAngles[1], eulerAngles[2]);
			m_bUpToDate = false;
		}

		if (ImGui::InputFloat("Local Scale: ", &localPosition.m_fScale)) m_bUpToDate = false;

		/*ImGui::Checkbox("Show world space matrix", &showWorldSpace);
		if(showWorldSpace)
		{
			if (ImGui::InputFloat3("column 0", m_kWorlspaceTransform[0].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 1", m_kWorlspaceTransform[1].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 2", m_kWorlspaceTransform[2].GetData())) m_bUpToDate = false;
			if (ImGui::InputFloat3("column 3", m_kWorlspaceTransform[3].GetData())) m_bUpToDate = false;
		}*/
	}
}
