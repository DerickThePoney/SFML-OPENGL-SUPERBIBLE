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

	void Draw(mat4 viewMatrix, mat4 projMatrix);
private:
	OGLBuffer m_kMesh;
	OGLVertexArray m_kVAO;
	OGLCubeMap m_kCubeMap;
	OGLProgram* m_pkSkyboxProgram;
};