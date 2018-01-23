#pragma once
#include "CameraManager.h"
#include "IComponent.h"
//using namespace vmath;



class CameraComponent : public IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;
public:
	CameraComponent(GameObject* pkParent = nullptr);
	~CameraComponent();
	
	void Init();
	void InitCamera();

	void ReleaseCamera();

	virtual void OnPreRender() override;

	virtual void Inspect() override;
	// Hérité via IComponent
	virtual void Clone(std::shared_ptr<IComponent> pkComponent) override;

	template<class Archive>
	void save(Archive & archive) const 
	{	
		if (m_pkCameraStruct == nullptr)
		{
			archive(cereal::make_nvp("IsUSED", false));
		}
		else
		{
			archive(cereal::make_nvp("IsUSED", true));
			if (CameraManager::main == m_pkCameraStruct)
			{
				archive(cereal::make_nvp("IsMAIN", true));
			}
			else
			{
				archive(cereal::make_nvp("IsMAIN", false));
			}

			archive(
				cereal::make_nvp("m_eType", m_pkCameraStruct->m_eType),
				cereal::make_nvp("focal", m_pkCameraStruct->focal),
				cereal::make_nvp("fNearPlane", m_pkCameraStruct->fNearPlane),
				cereal::make_nvp("fFarPlane", m_pkCameraStruct->fFarPlane),
				cereal::make_nvp("m_top", m_pkCameraStruct->m_top),
				cereal::make_nvp("m_bottom", m_pkCameraStruct->m_bottom)
			);


		}
	}

	template<class Archive>
	void load(Archive & archive)
	{
		bool bIsUsed, bIsMain;
		archive(cereal::make_nvp("IsUSED", bIsUsed));

		if (bIsUsed)
		{
			if (m_pkCameraStruct == nullptr) InitCamera();

			archive(cereal::make_nvp("IsMAIN", bIsMain));

			if (bIsMain) CameraManager::main = m_pkCameraStruct;

			archive(
				cereal::make_nvp("m_eType", m_pkCameraStruct->m_eType),
				cereal::make_nvp("focal", m_pkCameraStruct->focal),
				cereal::make_nvp("fNearPlane", m_pkCameraStruct->fNearPlane),
				cereal::make_nvp("fFarPlane", m_pkCameraStruct->fFarPlane),
				cereal::make_nvp("m_top", m_pkCameraStruct->m_top),
				cereal::make_nvp("m_bottom", m_pkCameraStruct->m_bottom)
				);
		}
		else
		{
			if (m_pkCameraStruct != nullptr)
			{
				ReleaseCamera();
			}
		}

		
	}

	Camera* RetrieveCamera() { return m_pkCameraStruct; }

private:
	Camera* m_pkCameraStruct;
};

CEREAL_REGISTER_TYPE(CameraComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponent, CameraComponent);