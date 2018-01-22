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
	//std::vector<ShaderInformation> akLocalShaderInformation;
	std::string version = "#version 430 core";
	InterpolatorInformation iinfo;
	UniformsInformation uinfo;
	m_iCurrent = 0;
	while (!IsAtEnd())
	{
		char c = Advance();
		switch (c)
		{
		case '#':
		{
			std::string nextWord = NextWord();
			if (nextWord == "sh")
			{
				ShaderInformation shinfo;
				if(ParseNextShader(shinfo, iinfo, uinfo) == 0)
				{ 
					akShaderInformation.push_back(shinfo);
				}
			}
			else if (nextWord ==  "version")
			{
				std::stringstream sstr;
				sstr << "#version " << NextLine();
				version = sstr.str();
			}
			break;
		}
		default:
		{}
		}
	}

	//add the informations
	for (UI32 i = 0; i < akShaderInformation.size(); ++i)
	{
		std::stringstream sstr;
		if (iinfo.m_kInterpolatorCode.size() > 0 && uinfo.m_kUniformsCode.size() > 0)
		{
			sstr << version << std::endl;
		}

		//add the interpolator
		if (iinfo.m_kInterpolatorCode.size() > 0)
		{
			switch(akShaderInformation[i].m_eShaderType)
			{
			case  GL_VERTEX_SHADER:
				sstr << "out ";
				break;
			case GL_FRAGMENT_SHADER:
				sstr << "in ";
				break;
			default:
				break;
			}
			sstr << iinfo.m_kInterpolatorCode << std::endl;
		}

		//add the uniforms
		if (uinfo.m_kUniformsCode.size() > 0)
		{
			sstr << uinfo.m_kUniformsCode << std::endl;
		}

		//add the code
		sstr << akShaderInformation[i].m_kShaderCode;
		akShaderInformation[i].m_kShaderCode = sstr.str();
	}
	
	return true;
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

std::string ShaderCompiler::NextLine()
{
	int thisChar = m_iCurrent;
	AdvanceToNext('\n');

	int size = m_iCurrent - thisChar;
	return m_kFileSource.substr(thisChar, size);
}

void ShaderCompiler::Rewind(int size)
{
	m_iCurrent -= size;
}

I32 ShaderCompiler::ParseNextShader(ShaderInformation& kShInfo, InterpolatorInformation& kIInfo, UniformsInformation& kUInfo)
{
	//look for next space
	Advance();
	int thisChar = m_iCurrent;	
	std::string kToken = NextWord();

	I32 type = -1;
	//L
	if (kToken == "VERTEX")
	{
		kShInfo.m_eShaderType = GL_VERTEX_SHADER;
		type = 0;
	}
	else if (kToken == "FRAGMENT")
	{
		kShInfo.m_eShaderType = GL_FRAGMENT_SHADER;
		type = 0;
	}
	else if (kToken == "INTERPOLATOR")
	{
		type = 1;
	}
	else if (kToken == "UNIFORMS")
	{
		type = 2;
	}

	thisChar = m_iCurrent;
	
	AdvanceToNext('#');
	while (NextWord() != "#sh" && !IsAtEnd()) { AdvanceToNext('#'); }

	if(!IsAtEnd()) Rewind(3);

	switch (type)
	{
	case 0:
		kShInfo.m_kShaderCode = m_kFileSource.substr(thisChar, m_iCurrent - thisChar);
		break;
	case 1:
		kIInfo.m_kInterpolatorCode = m_kFileSource.substr(thisChar, m_iCurrent - thisChar);
		break;
	case 2:
		kUInfo.m_kUniformsCode = m_kFileSource.substr(thisChar, m_iCurrent - thisChar);
		break;
	}
	
	
	return type;
}
