#include "stdafx.h"
#include "OGLCubeMap.h"

OGLCubeMap::OGLCubeMap()
	:m_bIsStorageInitialised(false)
{
}

OGLCubeMap::~OGLCubeMap()
{
}

void OGLCubeMap::Initialise()
{
	glGenTextures(1, &m_hiTexture);
}

void OGLCubeMap::InitialiseFromRessource(const std::string & pathName)
{
	I32 nrChannels;
	UC8* data;
	for (GLuint i = 0; i < 6; i++)
	{
		std::string filename = pathName + GetFilenameFromIndex(i);

		data = stbi_load(filename.c_str(), &m_iWidth, &m_iHeight, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, m_iWidth, m_iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << filename << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void OGLCubeMap::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_hiTexture);
}

void OGLCubeMap::Delete()
{
	glDeleteTextures(1, &m_hiTexture);
}

std::string OGLCubeMap::GetFilenameFromIndex(I32 index)
{
	switch (index)
	{
	case 0:
		return "iceflats_rt.tga";
		break;
	case 1:
		return "iceflats_lf.tga";
		break;
	case 2:
		return "iceflats_up.tga";
		break;
	case 3:
		return "iceflats_dn.tga";
		break;
	case 4:
		return "iceflats_ft.tga";
		break;
	case 5:
		return "iceflats_bk.tga";
		break;
	}

	return "";
}
