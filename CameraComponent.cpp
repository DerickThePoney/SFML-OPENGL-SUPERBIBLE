#include "stdafx.h"
#include "CameraComponent.h"
#include "GameObject.h"

TYPE_SYSTEM_RTTI_OBJECT(CameraComponent, IComponent);

CameraComponent::CameraComponent(GameObject * pkParent)
	: m_pkCameraStruct(nullptr), IComponent(pkParent)
{
}

CameraComponent::~CameraComponent()
{
	ReleaseCamera();
}

void CameraComponent::Init()
{
	m_pkCameraStruct = CameraManager::Instance()->Instantiate();
}

void CameraComponent::ReleaseCamera()
{
	CameraManager::Instance()->Destroy(m_pkCameraStruct);
	m_pkCameraStruct = nullptr;
}

void CameraComponent::OnPreRender()
{
	mat4 transform = m_pkParent->m_kTransform.GetWorldSpaceTransform();
	m_pkCameraStruct->ComputeLookAt(transform);
}

void CameraComponent::Inspect()
{
	if (ImGui::CollapsingHeader(GetType().GetName().c_str()))
	{
		bool bHasChanged = false;

		//Camera type
		const char* listbox_items[] = { "Orthographic", "Perspective" };
		int listbox_item_current = (int)m_pkCameraStruct->m_eType;
		if (ImGui::ListBox("Type of camera", &listbox_item_current, listbox_items, 2, 2))
		{
			m_pkCameraStruct->m_eType = (CameraType)listbox_item_current;
			bHasChanged = true;
		}

		//set the data
		if (m_pkCameraStruct->m_eType == ORTHOGRAPHIC)
		{
			float size = m_pkCameraStruct->m_top - m_pkCameraStruct->m_bottom;

			if (ImGui::DragFloat("Size", &size))
			{
				m_pkCameraStruct->m_top = size / 2;
				m_pkCameraStruct->m_bottom = -size / 2;
				m_pkCameraStruct->m_left = m_pkCameraStruct->m_bottom * m_pkCameraStruct->aspectRatio;
				m_pkCameraStruct->m_right = m_pkCameraStruct->m_top * m_pkCameraStruct->aspectRatio;
				bHasChanged = true;
			}
		}
		else
		{
			if (ImGui::DragFloat("Focal", &m_pkCameraStruct->focal)) bHasChanged = true;
		}

		if (ImGui::DragFloat("Near plane", &m_pkCameraStruct->fNearPlane)) bHasChanged = true;
		if (ImGui::DragFloat("Far plane", &m_pkCameraStruct->fFarPlane)) bHasChanged = true;

		if (bHasChanged)
		{
			m_pkCameraStruct->ComputeProjection();
		}
	}

}
