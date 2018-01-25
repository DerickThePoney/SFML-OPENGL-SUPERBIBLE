#pragma once

#include "CameraComponent.h"

//TODO make an IComponent and have transform and meshrenderer part of it
class FreeCameraComponent : public IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;

public:
	FreeCameraComponent(GameObject* pkParent = nullptr);
	~FreeCameraComponent();

	virtual void Init();
	virtual void Update(double deltaTime);

	// Hérité via IComponent
	virtual void Inspect() override;
	virtual void Clone(std::shared_ptr<IComponent> pkComponent) override;

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(m_fForwardSpeed),
			CEREAL_NVP(m_fLateralSpeed),
			CEREAL_NVP(m_fZoomSpeed)
		);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(
			CEREAL_NVP(m_fForwardSpeed),
			CEREAL_NVP(m_fLateralSpeed),
			CEREAL_NVP(m_fZoomSpeed)
		);
	}

private:
	void HandleTranslations(double deltaTime);
	void HandleZoom(double deltaTime);
	void HandleCameraRotation(double deltaTime);

private:
	std::shared_ptr<CameraComponent> m_pkCamera;

	F32 m_fForwardSpeed;
	F32 m_fLateralSpeed;
	F32 m_fZoomSpeed;
	F32 m_fAngularRotationSpeed;
};

CEREAL_REGISTER_TYPE(FreeCameraComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponent, FreeCameraComponent);