#pragma once
#include <vector>
#include "Utility.h"
#include "ShaderCompiler.h"
#include "OGLRendering.h"
#include "ProgramManager.h"
#include "MaterialUniformData.h"


struct MaterialData
{
	std::string m_kShaderFilename;
	bool m_bPerInstanceData;
	bool m_bBlendColors;
	bool m_bBlendSeparate;
	GLenum m_eSRCBlend;
	GLenum m_eDSTBlend;
	GLenum m_eSRCBlendAlpha;
	GLenum m_eDSTBlendAlpha;
	GLenum m_eBlendFunc;

	std::vector<IUniformDataContainerPtr> m_akDataForShader;

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kShaderFilename));
		archive(
			CEREAL_NVP(m_bPerInstanceData),
			CEREAL_NVP(m_bBlendColors),
			CEREAL_NVP(m_bBlendSeparate),
			CEREAL_NVP(m_eSRCBlend),
			CEREAL_NVP(m_eDSTBlend),
			CEREAL_NVP(m_eSRCBlendAlpha),
			CEREAL_NVP(m_eDSTBlendAlpha),
			CEREAL_NVP(m_eBlendFunc)
		);

		archive(CEREAL_NVP(m_akDataForShader));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(m_kShaderFilename));
		DEARCHIVE_WITH_DEFAULT(m_bPerInstanceData, false);
		DEARCHIVE_WITH_DEFAULT(m_bBlendColors, false);
		DEARCHIVE_WITH_DEFAULT(m_bBlendSeparate, false);
		DEARCHIVE_WITH_DEFAULT(m_eSRCBlend, GL_ZERO);
		DEARCHIVE_WITH_DEFAULT(m_eDSTBlend, GL_ONE);
		DEARCHIVE_WITH_DEFAULT(m_eSRCBlendAlpha, GL_ZERO);
		DEARCHIVE_WITH_DEFAULT(m_eDSTBlendAlpha, GL_ONE);
		DEARCHIVE_WITH_DEFAULT(m_eBlendFunc, GL_FUNC_ADD);
		
		DEARCHIVE_WITH_DEFAULT(m_akDataForShader, std::vector<std::shared_ptr<IUniformDataContainer>>());
	}
};


class Material
{
	friend class MaterialManager;
protected:
	Material();
	Material(const Material& other);
	~Material();

public:
	bool InitMaterialFromRessource(const std::string& kFilename);
	void Delete();

	void Use();

	bool operator==(const Material& other) { return m_uiMaterialID == other.m_uiMaterialID; }

	bool LoadProgram();

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(m_kFilename),
			cereal::make_nvp("MaterialID", s_uiMaxMaterialID)
		);
	}

	template<class Archive>
	void load(Archive& archive)
	{
		DEARCHIVE_WITH_DEFAULT(m_kFilename, "");

		if(m_kFilename != "") InitMaterialFromRessource(m_kFilename);		
	}

	template <class T>
	bool SetData(const std::string& kPropertyName, T& data)
	{
		for (UI32 i = 0; i < m_kMaterialData.m_akDataForShader.size(); ++i)
		{
			if (m_kMaterialData.m_akDataForShader[i]->GetName() == kPropertyName)
			{
				std::dynamic_pointer_cast<DataContainer<T>>(m_kMaterialData.m_akDataForShader[i])->SetValue(data);
				return true;
			}
		}
		return false;
	}

private:
	I32 BlendFuncToIndex(GLenum blendFunc);
	GLenum IndexToBlendFunc(I32 blendFunc);

	void PrepareDataForContainers();

	bool IsAlreadyMapped(const ActiveProgramInformations& info);
	void Map(const ActiveProgramInformations& info);

	void LoadDataInGPU();
	void InspectData();

public:

	//id stuff
	static UI32 s_uiMaxMaterialID;
	UI32 m_uiMaterialID;
	std::string m_kFilename;

	MaterialData m_kMaterialData;

	//underlying handle
	OGLProgram* m_pkProgram;
		
};

