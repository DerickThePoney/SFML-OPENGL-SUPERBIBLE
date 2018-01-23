#pragma once
#include "LightingManager.h"
#include "IComponent.h"

//TODO make an IComponent and have transform and meshrenderer part of it
class LightComponent : public IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;

public:
	LightComponent(GameObject* pkParent = nullptr);
	~LightComponent();

	void Init();
	void InitLight();
	
	virtual void OnPreRender() override;

	virtual void Inspect() override;

	// Hérité via IComponent
	virtual void Clone(std::shared_ptr<IComponent> pkComponent) override;

	template<class Archive>
	void save(Archive & archive) const
	{
		if (m_pkLight == nullptr)
		{
			archive(cereal::make_nvp("IsUSED", false));
		}
		else
		{
			archive(cereal::make_nvp("IsUSED", true));
			
			archive(
				cereal::make_nvp("m_eLight", m_pkLight->m_eLight),
				cereal::make_nvp("m_kLightColor", m_pkLight->m_kLightColor),
				cereal::make_nvp("m_fLightStrength", m_pkLight->m_fLightStrength),
				cereal::make_nvp("m_fRange", m_pkLight->m_fRange),
				cereal::make_nvp("m_fConeSize", m_pkLight->m_fConeSize)
			);
		}
	}

	template<class Archive>
	void load(Archive& archive)
	{
		bool bIsUsed;
		archive(cereal::make_nvp("IsUSED", bIsUsed));
		if (bIsUsed)
		{
			if (m_pkLight == nullptr) InitLight();

			archive(
				cereal::make_nvp("m_eLight", m_pkLight->m_eLight),
				cereal::make_nvp("m_kLightColor", m_pkLight->m_kLightColor),
				cereal::make_nvp("m_fLightStrength", m_pkLight->m_fLightStrength),
				cereal::make_nvp("m_fRange", m_pkLight->m_fRange),
				cereal::make_nvp("m_fConeSize", m_pkLight->m_fConeSize)
			);
		}
		else
		{
			if (m_pkLight != nullptr)
			{
				ReleaseLight();
			}
		}
	}

private:
	void ReleaseLight();

private:

	Light* m_pkLight;


	

};

CEREAL_REGISTER_TYPE(LightComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponent, LightComponent);