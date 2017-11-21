#pragma once
#include <vector>
#include "Utility.h"

enum ShaderAttrib
{
	VERTEX,
	NORMAL,
	UVs
};

class Material
{
public:
	Material();
	~Material();

	void Initialise(const GLuint& hiProgram, const GLuint& hiTexture);
	void Use();

	GLuint GetAttribLocation(const ShaderAttrib& eAttrib);
private:
	GLuint m_hiProgram;
	GLuint m_hiTexture;
	static const GLuint AttribIndexes[];
};

