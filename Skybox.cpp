#include "stdafx.h"
#include "Skybox.h"

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
}

void Skybox::Initialise()
{
	BMesh bmesh;
	MakeCube kMakeCube;
	kMakeCube.Apply(bmesh);

	m_pkMesh = MeshManager::Instance()->Instantiate();
	bmesh.BuildMesh(*m_pkMesh);

	m_pkMesh->LoadBuffersOnGraphicsCard();
	
	m_kCubeMap.Initialise();
	m_kCubeMap.InitialiseFromRessource("media/textures/skybox/");

	m_pkSkyboxProgram = ProgramManager::Instance()->InstantiateFromRessource("media/shaders/SkyboxShader.shader");
}

void Skybox::Delete()
{

	ProgramManager::Instance()->Destroy(m_pkSkyboxProgram);
	m_kCubeMap.Delete();
	MeshManager::Instance()->Destroy(m_pkMesh);
}

void Skybox::Draw()
{
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	m_pkSkyboxProgram->UseProgram();

	m_kCubeMap.Bind();
	m_pkMesh->BindForDrawing();

	//draw call
	glDrawElements(GL_TRIANGLES, m_pkMesh->m_aiIndices.size(), GL_UNSIGNED_INT, 0);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}
