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


	for (std::size_t i = 0; i < m_akUniformBlockInfo.size(); ++i)
	{
		SAFE_DELETE_ARRAY(m_akUniformBlockInfo[i].m_pcName);
		SAFE_DELETE_ARRAY(m_akUniformBlockInfo[i].m_piUniformsIndexes);
	}

	m_akUniformBlockInfo.clear();



	for (std::size_t i = 0; i < m_akUniformsInfo.size(); ++i)
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

const std::vector<ActiveProgramInformations>& OGLProgram::GetVectorAttributesInformation()
{
	return m_akAttributesInfo;
}

const std::vector<ActiveProgramInformations>& OGLProgram::GetUniformsInformation()
{
	return m_akUniformsInfo;
}

const std::vector<ActiveUniformBlockInformation>& OGLProgram::GetUniformBlocksInformation()
{
	return m_akUniformBlockInfo;
}

void OGLProgram::ExtractInformation()
{
	//get the vertex input location
	GLint value, maxlength;
	glGetProgramiv(m_hProgram, GL_ACTIVE_ATTRIBUTES, &value);
	glGetProgramiv(m_hProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxlength);

	std::cout << "-----------------------------------------------------" << std::endl;
	std::cout << "List of Active vertex attibutes" << std::endl;
	for (I32 i = 0; i < value; ++i)
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
	for (I32 i = 0; i < value; ++i)
	{
		ActiveProgramInformations att;
		att.m_pcName = new GLchar[maxlength];
		glGetActiveUniform(m_hProgram, i, maxlength, &att.m_iLength, &att.m_iSize, &att.m_eType, att.m_pcName);

		std::cout << "Uniform: " << i << "\t" << att.m_iLength << "\t" << att.m_iSize << "\t" << OGLUtilities::GLTypeEnumToCString(att.m_eType) << "\t" << att.m_pcName << std::endl;

		m_akUniformsInfo.push_back(att);
	}

	//get the uniforms blocks
	std::cout << "-----------------------------------------------------" << std::endl
		<< "List of Active uniform blocks" << std::endl;
	glGetProgramiv(m_hProgram, GL_ACTIVE_UNIFORM_BLOCKS, &value);
	for (I32 i = 0; i < value; ++i)
	{
		ActiveUniformBlockInformation actUB;
		glGetActiveUniformBlockiv(m_hProgram, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &maxlength);

		actUB.m_pcName = new GLchar[maxlength];
		actUB.m_iLength = maxlength;
		glGetActiveUniformBlockName(m_hProgram, i, maxlength, NULL, actUB.m_pcName);
		glGetActiveUniformBlockiv(m_hProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &maxlength);
		
		actUB.m_iNbUniform = maxlength;
		actUB.m_piUniformsIndexes = new GLint[maxlength];
		glGetActiveUniformBlockiv(m_hProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, actUB.m_piUniformsIndexes);

		std::cout << "Uniform Blocks: " << i << "\t" << actUB.m_iLength << "\t" << actUB.m_pcName << std::endl;

		for (I32 j = 0; j < actUB.m_iNbUniform; ++j)
		{
			m_akUniformsInfo[actUB.m_piUniformsIndexes[j]].m_bIsFromBlock = true;
			std::cout << "\t" << actUB.m_piUniformsIndexes[j] << std::endl;
		}

		m_akUniformBlockInfo.push_back(actUB);
	}
}
