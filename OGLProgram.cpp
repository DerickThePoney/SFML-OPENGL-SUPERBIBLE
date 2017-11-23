#include "stdafx.h"
#include "OGLProgram.h"


OGLProgram::OGLProgram()
{
}

OGLProgram::~OGLProgram()
{
}

bool OGLProgram::LinkProgram(OGLShader * uiShaders, int iShaderCount, bool bDeleteShaders)
{
	int i;

	m_hProgram = glCreateProgram();

	for (i = 0; i < iShaderCount; i++)
	{
		glAttachShader(m_hProgram, uiShaders[i]);
	}

	glLinkProgram(m_hProgram);

	GLint status;
	glGetProgramiv(m_hProgram, GL_LINK_STATUS, &status);

	if (!status)
	{
		char buffer[4096];
		glGetProgramInfoLog(m_hProgram, 4096, NULL, buffer);

		std::cout << (buffer) << "\n";

		glDeleteProgram(m_hProgram);
		return false;
	}

	if (bDeleteShaders)
	{
		delete[] uiShaders;
	}

	ExtractInformation();

	return true;
}

void OGLProgram::UseProgram()
{
	glUseProgram(m_hProgram);
}

void OGLProgram::DeleteProgram()
{
	glDeleteProgram(m_hProgram);
}

void OGLProgram::ExtractInformation()
{
	//get the vertex input location
	GLint value, maxlength;
	glGetProgramiv(m_hProgram, GL_ACTIVE_ATTRIBUTES, &value);
	glGetProgramiv(m_hProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxlength);

	for (int i = 0; i < value; ++i)
	{
		ActiveProgramVertexAttributes
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar* name = new GLchar[maxlength];
		glGetActiveAttrib(m_hProgram, i, maxlength, &length, &size, &type, name);

		std::cout << "Attrib: " << length << "\t" << size << "\t" << type << "\t" << name << std::endl;

		delete[] name;
	}


}
