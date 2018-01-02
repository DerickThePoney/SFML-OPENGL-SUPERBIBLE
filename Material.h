#pragma once
#include <vector>
#include "Utility.h"
#include "ShaderCompiler.h"
#include "OGLRendering.h"
#include "ProgramManager.h"


struct MaterialData
{
	std::string m_kShaderFilename;
	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kShaderFilename));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(m_kShaderFilename));
	}
};


class Material
{
	friend class MaterialManager;
protected:
	Material();
	~Material();

public:
	bool InitMaterialFromRessource(const std::string& kFilename);
	void Delete();

	void Use();

	bool operator==(const Material& other) { return m_uiMaterialID == other.m_uiMaterialID; }

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kFilename), cereal::make_nvp("MaterialID", s_uiMaxMaterialID));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(m_kFilename));

		if(m_kFilename != "") InitMaterialFromRessource(m_kFilename);		
	}

public:

	//id stuff
	static UI32 s_uiMaxMaterialID;
	UI32 m_uiMaterialID;
	std::string m_kFilename;

	MaterialData m_kMaterialData;

	//underlying handle
	OGLProgram* m_pkProgram;
		
};

