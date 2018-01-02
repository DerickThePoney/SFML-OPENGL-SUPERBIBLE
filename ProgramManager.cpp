#include "stdafx.h"
#include "ProgramManager.h"

ProgramManager* Singleton<ProgramManager>::s_kInstance = nullptr;

ProgramManager::ProgramManager()
{
}

ProgramManager::~ProgramManager()
{
}

OGLProgram * ProgramManager::InstantiateFromRessource(const std::string & kFilename)
{
	auto itFind = m_akPrograms.find(kFilename);
	
	if(itFind != m_akPrograms.end())
	{
		return itFind->second;
	}

	return BuildProgramFromFile(kFilename);
}

bool ProgramManager::Destroy(OGLProgram * pkObj)
{
	for (auto it = m_akPrograms.begin(); it != m_akPrograms.end(); ++it)
	{
		if (it->second == pkObj)
		{
			pkObj->DeleteProgram();
			SAFE_DELETE(pkObj);
			m_akPrograms.erase(it);
			return true;
		}
	}

	return false;
}

OGLProgram* ProgramManager::BuildProgramFromFile(const std::string& kFilename)
{
	//"Compile" Material
	std::vector<ShaderInformation> infos;
	ShaderCompiler::instance.RetrieveShaders(infos, kFilename);

	//create shaders objects
	OGLShader* shaders = new OGLShader[infos.size()];
	bool res = true;

	//compile shaders
	for (std::size_t i = 0; i < infos.size(); ++i)
	{
		bool resShader = shaders[i].InitFromMemory(infos[i].m_kShaderCode.c_str(), infos[i].m_eShaderType, kFilename.c_str());
		if (!resShader)
		{
			return nullptr;
		}
	}

	//link the program. This deletes the shader objects, cause we don't need it no more :-)
	OGLProgram* program = new OGLProgram();
	res = program->LinkProgram(shaders, infos.size(), true);

	if (!res) // bail if link did not work
	{
		Delete();
		return nullptr;
	}

	m_akPrograms[kFilename] = program;

	return program;
}
