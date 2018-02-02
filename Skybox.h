#pragma once

#include "OGLRendering.h"
#include "MeshManager.h"
#include "ProgramManager.h"


class Skybox
{
public:
	Skybox();
	~Skybox();

	void Initialise();
	void Delete();

	void Draw();
private:
	Mesh* m_pkMesh;
	OGLCubeMap m_kCubeMap;
	OGLProgram* m_pkSkyboxProgram;
};