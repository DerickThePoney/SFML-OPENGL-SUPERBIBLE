#pragma once
#include <vector>
#include "Utility.h"
#include "OGLRendering.h"

class Material
{
public:
	Material();
	~Material();

	bool InitFromFile(const std::vector<std::string>& filenames, const std::vector<GLenum> eShaderTypes);
	void Delete();

	void Use();

	bool operator==(const Material& other) { return m_uiMaterialID == other.m_uiMaterialID; }

	//id stuff
	static UI32 s_uiMaxMaterialID;
	UI32 m_uiMaterialID;

	//underlying handle
	OGLProgram m_kProgram;
		
};

