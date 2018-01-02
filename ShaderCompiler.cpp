#include "stdafx.h"
#include "ShaderCompiler.h"

ShaderCompiler ShaderCompiler::instance = ShaderCompiler();

bool ShaderCompiler::RetrieveShaders(std::vector<ShaderInformation>& akShaderInformation, const std::string & kFilename)
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
			if (NextWord() == "sh")
			{
				ShaderInformation info;
				ParseNextShader(info);
				akShaderInformation.push_back(info);
			}
			break;
		}
		default:
		{}
		}
	}

	return false;
}

ShaderCompiler::ShaderCompiler()
{
}


ShaderCompiler::~ShaderCompiler()
{
}

char ShaderCompiler::Advance()
{
	return m_kFileSource[m_iCurrent++];
}

char ShaderCompiler::Peek()
{
	return m_kFileSource[m_iCurrent];
}

void ShaderCompiler::AdvanceToNext(char c)
{
	while (!IsAtEnd() && Peek() != c) { Advance(); }
}

bool ShaderCompiler::IsAtEnd()
{
	return m_iCurrent >= (I32)m_kFileSource.size();
}

std::string ShaderCompiler::NextWord()
{
	int thisChar = m_iCurrent;
	AdvanceToNext(' ');

	int size = m_iCurrent - thisChar;
	return m_kFileSource.substr(thisChar, size);
}

void ShaderCompiler::Rewind(int size)
{
	m_iCurrent -= size;
}

bool ShaderCompiler::ParseNextShader(ShaderInformation& kInfo)
{
	//look for next space
	Advance();
	int thisChar = m_iCurrent;	
	std::string kToken = NextWord();

	//L
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
	while (NextWord() != "#sh" && !IsAtEnd()) { AdvanceToNext('#'); }

	if(!IsAtEnd()) Rewind(3);

	kInfo.m_kShaderCode = m_kFileSource.substr(thisChar, m_iCurrent - thisChar);
	
	return true;
}
