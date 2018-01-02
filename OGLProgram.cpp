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
		att.m_iLocation = glGetUniformLocation(m_hProgram, att.m_pcName);

		std::cout << "Uniform: " << i << "\t" << att.m_iLocation << "\t" << att.m_iLength << "\t" << att.m_iSize << "\t" << OGLUtilities::GLTypeEnumToCString(att.m_eType) << "\t" << att.m_pcName << std::endl;

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


void OGLProgram::InspectUniformProgramInformation(const ActiveProgramInformations & info)
{
	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	UseProgram();
	switch (info.m_eType)
	{
	case GL_FLOAT:
	{
		float valuef;
		glGetUniformfv(m_hProgram, info.m_iLocation, &valuef);
		if (ImGui::InputFloat(info.m_pcName, &valuef)) glUniform1f(info.m_iLocation, valuef);
		break;
	}
	case GL_FLOAT_VEC2:
	{
		vmath::vec2 valuef2;
		glGetUniformfv(m_hProgram, info.m_iLocation, valuef2.GetData());
		if (ImGui::InputFloat2(info.m_pcName, valuef2.GetData())) glUniform2fv(info.m_iLocation, 1, valuef2.GetData());
		break;
	}
	case GL_FLOAT_VEC3:
	{
		vmath::vec3 valuef2;
		glGetUniformfv(m_hProgram, info.m_iLocation, valuef2.GetData());
		if (ImGui::InputFloat3(info.m_pcName, valuef2.GetData())) 
			glUniform3fv(info.m_iLocation, 1, valuef2.GetData());
		break;
	}
	case GL_FLOAT_VEC4:
	{
		vmath::vec4 valuef2;
		glGetUniformfv(m_hProgram, info.m_iLocation, valuef2.GetData());
		if (ImGui::InputFloat3(info.m_pcName, valuef2.GetData())) glUniform4fv(info.m_iLocation, 1, valuef2.GetData());
		break;
	}
	case GL_FLOAT_MAT2: 
	case GL_FLOAT_MAT3: 
	case GL_FLOAT_MAT4: 
	case GL_FLOAT_MAT2x3: 
	case GL_FLOAT_MAT2x4: 
	case GL_FLOAT_MAT3x2: 
	case GL_FLOAT_MAT3x4: 
	case GL_FLOAT_MAT4x2: 
	case GL_FLOAT_MAT4x3: {break; }
	case GL_INT:
	{
		int value;
		glGetUniformiv(m_hProgram, info.m_iLocation, &value);
		if (ImGui::InputInt(info.m_pcName, &value)) glUniform1i(info.m_iLocation, value);
		break;
	}
	case GL_INT_VEC2:
	{
		vmath::ivec2 value;
		glGetUniformiv(m_hProgram, info.m_iLocation, value.GetData());
		if (ImGui::InputInt2(info.m_pcName, value.GetData())) glUniform2iv(info.m_iLocation, 1, value.GetData());
		break;
	}
	case GL_INT_VEC3:
	{
		vmath::ivec3 value;
		glGetUniformiv(m_hProgram, info.m_iLocation, value.GetData());
		if (ImGui::InputInt3(info.m_pcName, value.GetData())) glUniform3iv(info.m_iLocation, 1, value.GetData());
		break;
	}
	case GL_INT_VEC4:
	{
		vmath::ivec4 value;
		glGetUniformiv(m_hProgram, info.m_iLocation, value.GetData());
		if (ImGui::InputInt4(info.m_pcName, value.GetData())) glUniform4iv(info.m_iLocation, 1, value.GetData());
		break;
	}
	case GL_UNSIGNED_INT:
	{
		UI32 value;
		glGetUniformuiv(m_hProgram, info.m_iLocation, &value);
		if (ImGui::InputInt(info.m_pcName, (I32*)&value)) glUniform1ui(info.m_iLocation, value);
		break;
	}
	case GL_UNSIGNED_INT_VEC2:
	{
		vmath::uvec2 value;
		glGetUniformuiv(m_hProgram, info.m_iLocation, value.GetData());
		if (ImGui::InputInt2(info.m_pcName, (I32*) value.GetData())) glUniform2uiv(info.m_iLocation, 1, value.GetData());
		break;
	}
	case GL_UNSIGNED_INT_VEC3:
	{
		vmath::uvec3 value;
		glGetUniformuiv(m_hProgram, info.m_iLocation, value.GetData());
		if (ImGui::InputInt3(info.m_pcName, (I32*)value.GetData())) glUniform3uiv(info.m_iLocation, 1, value.GetData());
		break;
	}
	case GL_UNSIGNED_INT_VEC4:
	{
		vmath::uvec4 value;
		glGetUniformuiv(m_hProgram, info.m_iLocation, value.GetData());
		if (ImGui::InputInt4(info.m_pcName, (I32*)value.GetData())) glUniform4uiv(info.m_iLocation, 1, value.GetData());
		break;
	}
	/*case GL_DOUBLE:
	{
		F64 value;
		glGetUniformuiv(m_hProgram, index, &value);
		if (ImGui::InputInt(info.m_pcName, (I32*)&value)) glUniform1ui(index, value);
		break;
	}
	case GL_DOUBLE_VEC2:
	{
		float value;
		glGetUniformfv(m_hProgram, index, &value);
		if (ImGui::InputFloat(info.m_pcName, &value)) glUniform1f(index, value);
		break;
	}
	case GL_DOUBLE_VEC3:
	{
		float value;
		glGetUniformfv(m_hProgram, index, &value);
		if (ImGui::InputFloat(info.m_pcName, &value)) glUniform1f(index, value);
		break;
	}
	case GL_DOUBLE_VEC4:
	{
		float value;
		glGetUniformfv(m_hProgram, index, &value);
		if (ImGui::InputFloat(info.m_pcName, &value)) glUniform1f(index, value);
		break;
	}*/
	case GL_DOUBLE_MAT2: 
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4: 
	case GL_DOUBLE_MAT2x3: 
	case GL_DOUBLE_MAT2x4: 
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
	default:
		{break; }
	}

	glUseProgram(currentProgram);
}