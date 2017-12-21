#include "stdafx.h"
#include "OGLTexture2D.h"


OGLTexture2D::OGLTexture2D()
	:m_bIsStorageInitialised(false)
{
}


OGLTexture2D::~OGLTexture2D()
{
}

void OGLTexture2D::Initialise()
{
	glGenTextures(1, &m_hiTexture);
}

void OGLTexture2D::InitialiseStorage(I32 iWidth, I32 iHeight, I32 iLevels, GLenum eSamplerFormat)
{
	if (m_bIsStorageInitialised)
	{
		Delete();
		Initialise();
		m_bIsStorageInitialised = false;
	}

	Bind();

	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iNbLevels = iLevels;
	m_eSamplerFormat = eSamplerFormat;

	glTexStorage2D(GL_TEXTURE_2D, m_iNbLevels, eSamplerFormat, m_iWidth, m_iHeight);

	m_bIsStorageInitialised = true;
}

void OGLTexture2D::SetData(I32 iLevel, I32 iXOffset, I32 iYOffset, I32 iWidth, I32 iHeight, GLenum eDataFormat, GLenum eDataType, void * data, bool bGenerateMipMaps)
{
	if (!m_bIsStorageInitialised)
	{
		//if not init then init with default GL_RGBA32F sampler format
		InitialiseStorage(iXOffset + iWidth, iYOffset + iHeight, iLevel+1, GL_RGBA32F); 
	}

	Bind();

	//TODO MIPMAPPING
	glTexSubImage2D(GL_TEXTURE_2D, iLevel, iXOffset, iYOffset, iWidth, iHeight, eDataFormat, eDataType, data);
}

void OGLTexture2D::Bind(GLint iUnit)
{
	glBindTexture(GL_TEXTURE_2D, m_hiTexture);
}


void OGLTexture2D::Delete()
{
	glDeleteTextures(1, &m_hiTexture);
}

void OGLTexture2D::Inspect()
{
	static ImVec2 bleft = ImVec2(0, 0);
	static ImVec2 tright = ImVec2(1, 1);

	ImGui::InputFloat2("bottom left", (float*)&bleft);
	ImGui::InputFloat2("top right", (float*)&tright);

	//ivec2 tSize = m_kTexture.GetTextureSize();

	ImGui::Image((void *)(intptr_t)m_hiTexture, ImVec2(512, 512), bleft, tright, ImVec4(1, 1, 1, 1), ImColor(255, 255, 255, 255));

}
