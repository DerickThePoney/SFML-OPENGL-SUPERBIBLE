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
	Bind();
	I32 nrChannels;
	UC8* data;

	std::string filename = pathName + GetFilenameFromIndex(0);

	data = stbi_load(filename.c_str(), &m_iWidth, &m_iHeight, &nrChannels, 0);
	if (data)
	{
		// init the texture storage
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, m_iWidth, m_iHeight);

		//input the first data
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, m_iWidth, m_iHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
		Delete();
	}

	for (GLuint i = 1; i < 6; i++)
	{
		filename = pathName + GetFilenameFromIndex(i);

		data = stbi_load(filename.c_str(), &m_iWidth, &m_iHeight, &nrChannels, 0);

		if (data)
		{
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, m_iWidth, m_iHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
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
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
		return "right.jpg";
		break;
	case 1:
		return "left.jpg";
		break;
	case 2:
		return "top.jpg";
		break;
	case 3:
		return "bottom.jpg";
		break;
	case 4:
		return "front.jpg";
		break;
	case 5:
		return "back.jpg";
		break;
	}

	return "";
}
