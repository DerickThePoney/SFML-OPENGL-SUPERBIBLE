#include "stdafx.h"
#include "OGLShader.h"


OGLShader::OGLShader()
{
}

OGLShader::~OGLShader()
{
	glDeleteShader(m_hShader);
}

bool OGLShader::InitFromFile(const std::string & filename, GLenum eType)
{
	return InitFromFile(filename.c_str(), eType);
}

bool OGLShader::InitFromFile(const char * filename, GLenum eType)
{
	FILE * fp;
	size_t filesize;
	char * data;

	fp = fopen(filename, "rb");

	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	data = new char[filesize + 1];

	if (!data)
	{
		std::cout << "Problem to allocate data for shader" << std::endl;
		return false;
	}

	fread(data, 1, filesize, fp);
	data[filesize] = 0;
	fclose(fp);

	bool res = InitFromMemory(data, eType, filename);

	delete[] data;

	return res;
}

bool OGLShader::InitFromMemory(const char * shaderCode, GLenum eType, const char* filename)
{
	m_eType = eType;

	m_hShader = glCreateShader(m_eType);

	if (!m_hShader)
	{
		std::cout << filename << ": Problem to allocate shader" << std::endl;
		return false;
	}

	glShaderSource(m_hShader, 1, &shaderCode, NULL);
	
	glCompileShader(m_hShader);

	GLint status = 0;
	glGetShaderiv(m_hShader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		char buffer[4096];
		glGetShaderInfoLog(m_hShader, 4096, NULL, buffer);
		std::cout << filename << ":" << buffer << "\n";

		glDeleteShader(m_hShader);
		return false;
	}
	

	return true;
}