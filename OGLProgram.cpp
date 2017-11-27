#include "stdafx.h"
#include "OGLProgram.h"


OGLProgram::OGLProgram()
{
}

OGLProgram::~OGLProgram()
{
	for (std::size_t i = 0; i < m_akAttributesInfo.size(); ++i)
	{
		SAFE_DELETE_ARRAY(m_akAttributesInfo[i].m_pcName);
	}

	m_akAttributesInfo.clear();


	for (std::size_t i = 0; i < m_akAttributesInfo.size(); ++i)
	{
		SAFE_DELETE_ARRAY(m_akUniformsInfo[i].m_pcName);
	}

	m_akUniformsInfo.clear();
	
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

	//ExtractInformation();

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

	std::cout << "List of Active vertex attibutes" << std::endl;
	for (int i = 0; i < value; ++i)
	{
		ActiveProgramInformations att;
		att.m_pcName = new GLchar[maxlength];
		glGetActiveAttrib(m_hProgram, i, maxlength, &att.m_iLength, &att.m_iSize, &att.m_eType, att.m_pcName);

		std::cout << "Attrib: " << i << "\t" << att.m_iLength << "\t" << att.m_iSize << "\t" << OGLUtilities::GLTypeEnumToCString(att.m_eType) << "\t" << att.m_pcName << std::endl;

		m_akAttributesInfo.push_back(att);
	}

	//get the uniforms list
	glGetProgramiv(m_hProgram, GL_ACTIVE_UNIFORMS, &value); 
	glGetProgramiv(m_hProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlength);

	std::cout << "-----------------------------------------------------" << std::endl 
		<< "List of Active uniforms" << std::endl;
	for (int i = 0; i < value; ++i)
	{
		ActiveProgramInformations att;
		att.m_pcName = new GLchar[maxlength];
		glGetActiveUniform(m_hProgram, i, maxlength, &att.m_iLength, &att.m_iSize, &att.m_eType, att.m_pcName);

		std::cout << "Uniform: " << i << "\t" << att.m_iLength << "\t" << att.m_iSize << "\t" << OGLUtilities::GLTypeEnumToCString(att.m_eType) << "\t" << att.m_pcName << std::endl;

		m_akUniformsInfo.push_back(att);
	}

}
