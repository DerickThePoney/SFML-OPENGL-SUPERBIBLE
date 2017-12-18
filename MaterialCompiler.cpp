#include "stdafx.h"
#include "MaterialCompiler.h"

MaterialCompiler MaterialCompiler::instance = MaterialCompiler();

bool MaterialCompiler::RetrieveShaders(std::vector<MaterialInformation>& akShaderInformation, const std::string & kFilename)
{
	
	
	{
		std::ifstream ifstr(kFilename);
		std::stringstream sstr;
		sstr << ifstr.rdbuf();

		m_kFileSource = sstr.str();
	}

	//look for pragma
	m_iCurrent = 0;
	while (!IsAtEnd())
	{
		char c = Advance();
		switch (c)
		{
		case '#':
		{
			MaterialInformation info;
			ParseNextShader(info);
			akShaderInformation.push_back(info);
			break;
		}
		default:
		{}
		}
	}

	return false;
}

MaterialCompiler::MaterialCompiler()
{
}


MaterialCompiler::~MaterialCompiler()
{
}

char MaterialCompiler::Advance()
{
	return m_kFileSource[m_iCurrent++];
}

char MaterialCompiler::Peek()
{
	return m_kFileSource[m_iCurrent];
}

void MaterialCompiler::AdvanceToNext(char c)
{
	while (!IsAtEnd() && Peek() != c) { Advance(); }
}

bool MaterialCompiler::IsAtEnd()
{
	return m_iCurrent >= m_kFileSource.size();
}

bool MaterialCompiler::ParseNextShader(MaterialInformation& kInfo)
{
	//look for next space
	int thisChar = m_iCurrent;
	AdvanceToNext(' ');

	int size = m_iCurrent - thisChar;
	std::string kToken = m_kFileSource.substr(thisChar, size);

	if (kToken == "VERTEX")
	{
		kInfo.m_eShaderType = GL_VERTEX_SHADER;
	}
	else if (kToken == "FRAGMENT")
	{
		kInfo.m_eShaderType = GL_FRAGMENT_SHADER;
	}
	thisChar = m_iCurrent;
	
	AdvanceToNext('#');
	kInfo.m_kShaderCode = m_kFileSource.substr(thisChar, m_iCurrent - thisChar);

	return true;
}
