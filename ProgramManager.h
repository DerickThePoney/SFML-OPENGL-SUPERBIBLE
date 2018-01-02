#pragma once
#include "Singleton.h"
#include "OGLProgram.h"
#include "ShaderCompiler.h"

class ProgramManager : public Singleton<ProgramManager>
{
	friend class Singleton<ProgramManager>;
protected:
	ProgramManager();
	~ProgramManager();

public:
	OGLProgram* InstantiateFromRessource(const std::string& kFilename);
	bool Destroy(OGLProgram* pkObj);

private:
	OGLProgram* BuildProgramFromFile(const std::string& kFilename);

private:
	std::map<std::string, OGLProgram*> m_akPrograms;
};

