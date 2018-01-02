#include "stdafx.h"
#include "OGLTexture2D.h"


OGLTexture2D::OGLTexture2D()
	:m_bIsStorageInitialised(false), m_eTextureFilter(GL_LINEAR), m_eWrapMode(GL_REPEAT)//, m_eMagFilter(GL_LINEAR)
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

	SetSamplingParameters();

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

	static char* filterModes[6] = { "GL_NEAREST" , "GL_LINEAR", "GL_NEAREST_MIPMAP_NEAREST" , "GL_LINEAR_MIPMAP_NEAREST" , "GL_NEAREST_MIPMAP_LINEAR" , "GL_LINEAR_MIPMAP_LINEAR" };
	int index = 0;
	switch (m_eTextureFilter)
	{
	case GL_NEAREST:
		index = 0;
		break;
	case GL_LINEAR:
		index = 1;
		break;
	case GL_NEAREST_MIPMAP_NEAREST:
		index = 2;
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		index = 3;
		break;
	case GL_NEAREST_MIPMAP_LINEAR:
		index = 4;
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		index = 5;
		break;
	default:
		break;
	}

	if (ImGui::Combo("Filter", &index, filterModes, 6))
	{
		switch (index)
		{
		case 0:
			m_eTextureFilter = GL_NEAREST;
			break;
		case 1:
			m_eTextureFilter = GL_LINEAR;
			break;
		case 2:
			m_eTextureFilter = GL_NEAREST_MIPMAP_NEAREST;
			break;
		case 3:
			m_eTextureFilter = GL_LINEAR_MIPMAP_NEAREST;
			break;
		case 4:
			m_eTextureFilter = GL_NEAREST_MIPMAP_LINEAR;
			break;
		case 5:
			m_eTextureFilter = GL_LINEAR_MIPMAP_LINEAR;
			break;
		}

		SetSamplingParameters();
	}


	static char* wrapModes[5] = { "GL_CLAMP_TO_EDGE" , "GL_CLAMP_TO_BORDER", "GL_MIRRORED_REPEAT" , "GL_REPEAT" , "GL_MIRROR_CLAMP_TO_EDGE"  };
	index = 0;
	switch (m_eWrapMode)
	{
	case GL_CLAMP_TO_EDGE:
		index = 0;
		break;
	case GL_CLAMP_TO_BORDER:
		index = 1;
		break;
	case GL_MIRRORED_REPEAT:
		index = 2;
		break;
	case GL_REPEAT:
		index = 3;
		break;
	case GL_MIRROR_CLAMP_TO_EDGE:
		index = 4;
		break;
	default:
		break;
	}

	if (ImGui::Combo("Wrap Mode", &index, wrapModes, 5))
	{
		switch (index)
		{
		case 0:
			m_eWrapMode = GL_CLAMP_TO_EDGE;
			break;
		case 1:
			m_eWrapMode = GL_CLAMP_TO_BORDER;
			break;
		case 2:
			m_eWrapMode = GL_MIRRORED_REPEAT;
			break;
		case 3:
			m_eWrapMode = GL_REPEAT;
			break;
		case 4:
			m_eWrapMode = GL_MIRROR_CLAMP_TO_EDGE;
			break;
		}

		SetSamplingParameters();
	}

	ImGui::Image((void *)(intptr_t)m_hiTexture, ImVec2(512, 512), bleft, tright, ImVec4(1, 1, 1, 1), ImColor(255, 255, 255, 255));

}

void OGLTexture2D::SetSamplingParameters()
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_eTextureFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_eTextureFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_eWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, m_eWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_eWrapMode);
}
