#include "stdafx.h"
#include "Material.h"

const GLuint Material::AttribIndexes[] = { 0,1,4 };

Material::Material()
{
}


Material::~Material()
{
}

void Material::Initialise(const GLuint& hiProgram, const GLuint& hiTexture)
{
	m_hiProgram = hiProgram;
	m_hiTexture = hiTexture;
}

void Material::Use()
{
	glBindTexture(GL_TEXTURE_2D, m_hiTexture);
	glUseProgram(m_hiProgram);
}

GLuint Material::GetAttribLocation(const ShaderAttrib & eAttrib)
{
	switch (eAttrib)
	{
	case VERTEX:
		return Material::AttribIndexes[0];
		break;
	case NORMAL:
		return Material::AttribIndexes[1];
		break;
	case UVs:
		return Material::AttribIndexes[2];
		break;
	default:
		return -1;
		break;
	}
}
