#include "stdafx.h"
#include "FreeCameraComponent.h"
#include "GameObject.h"

TYPE_SYSTEM_RTTI_OBJECT(FreeCameraComponent, IComponent);

FreeCameraComponent::FreeCameraComponent(GameObject * pkParent)
	: IComponent(pkParent), m_pkCamera(nullptr), m_fForwardSpeed(1), m_fLateralSpeed(1), m_fZoomSpeed(1)
{
}

FreeCameraComponent::~FreeCameraComponent()
{
}

void FreeCameraComponent::Init()
{
	m_pkCamera = m_pkParent->FindComponent<CameraComponent>();
}

void FreeCameraComponent::Update(double deltaTime)
{
	if (m_pkCamera == nullptr)
	{
		m_pkCamera = m_pkParent->FindComponent<CameraComponent>();
		if (m_pkCamera == nullptr) return;
	}

	HandleTranslations(deltaTime);
	HandleZoom(deltaTime);
}

void FreeCameraComponent::Inspect()
{
	if (ImGui::CollapsingHeader(GetType().GetName().c_str()))
	{
		ImGui::SliderFloat("Forward speed", &m_fForwardSpeed, 0, 20);
		ImGui::SliderFloat("Lateral speed", &m_fLateralSpeed, 0, 20);
		ImGui::SliderFloat("Scroll speed", &m_fZoomSpeed, 0, 100);
	}
}

void FreeCameraComponent::Clone(std::shared_ptr<IComponent> pkComponent)
{
	std::shared_ptr<FreeCameraComponent> pkData = std::dynamic_pointer_cast<FreeCameraComponent>(pkComponent);
	if (pkData == nullptr) return;

	m_fForwardSpeed = pkData->m_fForwardSpeed;
	m_fLateralSpeed = pkData->m_fLateralSpeed;
	m_fZoomSpeed = pkData->m_fZoomSpeed;
}

void FreeCameraComponent::HandleTranslations(double deltaTime)
{
	//check forward
	F32 fForwardRequest = 0;
	if (InputManager::Instance()->IsKeyboardButtonPressed(sf::Keyboard::Key::Up))
	{
		fForwardRequest += 1;
	}

	if (InputManager::Instance()->IsKeyboardButtonPressed(sf::Keyboard::Key::Down))
	{
		fForwardRequest += -1;
	}


	//check left and right
	F32 fLateralRequest = 0;
	if (InputManager::Instance()->IsKeyboardButtonPressed(sf::Keyboard::Key::Right))
	{
		fLateralRequest = 1;
	}

	if (InputManager::Instance()->IsKeyboardButtonPressed(sf::Keyboard::Key::Left))
	{
		fLateralRequest = -1;
	}


	//
	if (fForwardRequest != 0 || fLateralRequest != 0)
	{
		vec3 translate(fLateralRequest * m_fLateralSpeed * (deltaTime / 1000),
			0,
			-fForwardRequest * m_fForwardSpeed * (deltaTime / 1000));

		m_pkParent->m_kTransform.TranslateInLocalSpace(translate);
	}
}

void FreeCameraComponent::HandleZoom(double deltaTime)
{
	float fZoomRequest = InputManager::Instance()->GetMouseWheelDelta();

	if (fZoomRequest != 0)
	{
		Camera* pkCameraStruct = m_pkCamera->RetrieveCamera();
		pkCameraStruct->focal += -fZoomRequest * m_fZoomSpeed * deltaTime / 1000;
		if (pkCameraStruct->focal < 5) pkCameraStruct->focal = 5;
		if (pkCameraStruct->focal > 180) pkCameraStruct->focal = 180;

		pkCameraStruct->ComputeProjection();
	}
}
