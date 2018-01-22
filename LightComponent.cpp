#include "stdafx.h"
#include "LightComponent.h"
#include "GameObject.h"

TYPE_SYSTEM_RTTI_OBJECT(LightComponent, IComponent);

LightComponent::LightComponent(GameObject * pkParent)
	: m_pkLight(nullptr), IComponent(pkParent)
{
}

LightComponent::~LightComponent()
{
	ReleaseLight();
}

void LightComponent::Init()
{
	if (m_pkLight != nullptr) return;
	m_pkLight = LightingManager::Instance()->GetPointerToNextUnusedLight();
	m_pkLight->m_eLight = DIRECTIONAL_LIGHT;
	m_pkLight->m_kLightColor = vec4(1, 1, 1, 1);
	m_pkLight->m_fLightStrength = 1;
	m_pkLight->m_fConeSize = 10;
	m_pkLight->m_fRange = 1;
}

void LightComponent::OnPreRender()
{
	if (m_pkLight == nullptr) return;

	m_pkLight->m_kPosition = m_pkParent->m_kTransform.GetWorldspacePosition();
	m_pkLight->m_kDirection = m_pkParent->m_kTransform.GetForwardVector();
}

void LightComponent::Inspect()
{
	if (ImGui::CollapsingHeader(GetType().GetName().c_str()))
	{
		if (m_pkLight == nullptr)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "There are too many lights already in the scene!");
		}
		else
		{
			const C8* pcLightTypes[] = { "Direction", "Point", "Spot" };
			I32 lightType = (I32)m_pkLight->m_eLight - 1;
			if (ImGui::Combo("Light type", &lightType, pcLightTypes, 3))
			{
				m_pkLight->m_eLight = (LIGHT_TYPE)(lightType + 1);
			}

			ImGui::ColorEdit4("Light color", m_pkLight->m_kLightColor.GetData(), false);
			ImGui::SliderFloat("Light strength", &m_pkLight->m_fLightStrength, 0, 20);

			switch (m_pkLight->m_eLight)
			{
			case SPOT_LIGHT:
				ImGui::SliderAngle("Light cone", &m_pkLight->m_fConeSize, 0, 90);
			case POINT_LIGHT:
				ImGui::SliderFloat("Light range", &m_pkLight->m_fRange, 0, 100);
				break;
			default:
				break;
			}
		}
	}
}

void LightComponent::ReleaseLight()
{
	if (m_pkLight != nullptr)
	{
		m_pkLight->m_eLight = UNUSED;
		m_pkLight = nullptr;
	}
}
