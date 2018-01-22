#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Init(UI32 width, UI32 height, vmath::mat4& t)
{
	aspectRatio = (float)width / (float)height;
	m_left = m_bottom * aspectRatio;
	m_right = m_top* aspectRatio;

	ComputeProjection();

	ComputeLookAt(t);

	InitialiseProjectionBuffer();
}

void Camera::OnResize(UI32 width, UI32 height)
{
	float size = m_top - m_bottom;
	aspectRatio = (float)width / (float)height;

	m_top = size / 2;
	m_bottom = -size / 2;
	m_left = m_bottom * aspectRatio;
	m_right = m_top* aspectRatio;

	ComputeProjection();
}

void Camera::LoadProjectionOnGraphics(GLuint bufferIndex)
{
	vmath::mat4 proj[2];
	proj[0] = m_kLookAtMatrix;
	proj[1] = m_kProjectionMatrice;

	m_kProjectionBuffer.UpdateData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), (void*)proj, GL_WRITE_ONLY);

	m_kProjectionBuffer.BindToUniformBindingPoint(bufferIndex);
}

void Camera::Inspect()
{
	if (ImGui::CollapsingHeader("CameraObject"))
	{
		bool hasChanged = false;

		const char* listbox_items[] = { "Orthographic", "Perspective" };
		int listbox_item_current = (int)m_eType;
		if (ImGui::ListBox("Type of CameraObject", &listbox_item_current, listbox_items, 2, 2))
		{
			m_eType = (CameraType)listbox_item_current;
			hasChanged = true;
		}


		if (m_eType == ORTHOGRAPHIC)
		{
			float size = m_top - m_bottom;

			if (ImGui::DragFloat("Size", &size))
			{
				m_top = size / 2;
				m_bottom = -size / 2;
				m_left = m_bottom * aspectRatio;
				m_right = m_top * aspectRatio;
				hasChanged = true;
			}
		}
		else
		{
			if (ImGui::DragFloat("Focal", &focal)) hasChanged = true;
		}

		if (ImGui::DragFloat("Near plane", &fNearPlane)) hasChanged = true;
		if (ImGui::DragFloat("Far plane", &fFarPlane)) hasChanged = true;

		if (hasChanged)
		{
			ComputeProjection();
		}
	}
}

void Camera::ComputeLookAt(vmath::mat4& t)
{
	vmath::vec3 eye(t[3][0], t[3][1], t[3][2]);

	vmath::vec4 center4 = t * vmath::vec4(0, 0, -1, 0);
	vmath::vec3 center = eye + vmath::vec3(center4[0], center4[1], center4[2]);

	vmath::vec4 up4 = t * vmath::vec4(0, 1, 0, 0);
	vmath::vec3 up = vmath::vec3(up4[0], up4[1], up4[2]);

	m_kLookAtMatrix = vmath::lookat(eye, center, up);
}

void Camera::ComputeProjection()
{
	switch (m_eType)
	{
	case ORTHOGRAPHIC:
		m_kProjectionMatrice = vmath::ortho(m_left,
			m_right,
			m_bottom,
			m_top,
			fNearPlane,
			fFarPlane);
		break;
	case PERSPECTIVE:
		m_kProjectionMatrice = vmath::perspective(focal,
			aspectRatio,
			fNearPlane,
			fFarPlane);
		/*m_kProjectionMatrice = vmath::frustum(-5,
		5,
		-5,
		5,
		10,
		100);*/


		break;
	}
}

void Camera::InitialiseProjectionBuffer()
{
	vmath::mat4 proj[2];
	proj[0] = m_kLookAtMatrix;
	proj[1] = m_kProjectionMatrice;
	m_kProjectionBuffer.Init(GL_UNIFORM_BUFFER, sizeof(vmath::mat4) * 2, proj, GL_MAP_WRITE_BIT);
}
