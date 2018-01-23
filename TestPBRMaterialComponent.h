#pragma once
#include "IComponent.h"

class TestPBRMaterialComponent : public IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;

public:
	TestPBRMaterialComponent(GameObject* pkParent = nullptr);
	~TestPBRMaterialComponent();

	virtual void Init() override;
	virtual void Inspect() override;

	virtual void Clone(std::shared_ptr<IComponent> pkComponent) override;

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(m_kPrefabFile),
			CEREAL_NVP(m_kTranslation),
			CEREAL_NVP(m_fRoughnessIncrement),
			CEREAL_NVP(m_fMetallicIncrement),
			CEREAL_NVP(m_kAlbedo)
		);
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(
			CEREAL_NVP(m_kPrefabFile),
			CEREAL_NVP(m_kTranslation),
			CEREAL_NVP(m_fRoughnessIncrement),
			CEREAL_NVP(m_fMetallicIncrement)
		);
		DEARCHIVE_WITH_DEFAULT(m_kAlbedo, vec3(1));
	}

private:
	void CreateChildren();
	void ChildrenParameters();

private:
	std::string m_kPrefabFile;
	vec3 m_kTranslation;
	vec3 m_kAlbedo;
	F32 m_fRoughnessIncrement;
	F32 m_fMetallicIncrement;
};

CEREAL_REGISTER_TYPE(TestPBRMaterialComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponent, TestPBRMaterialComponent);
