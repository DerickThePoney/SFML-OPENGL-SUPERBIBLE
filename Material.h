#pragma once
#include <vector>
#include "Utility.h"
#include "MaterialCompiler.h"
#include "OGLRendering.h"

class Material
{
	friend class MaterialManager;
protected:
	Material();
	~Material();

public:
	bool InitFromFiles(const std::vector<std::string>& filenames, const std::vector<GLenum> eShaderTypes);
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

private:
	

public:

	//id stuff
	static UI32 s_uiMaxMaterialID;
	UI32 m_uiMaterialID;
	std::string m_kFilename;

	//underlying handle
	OGLProgram m_kProgram;
		
};

