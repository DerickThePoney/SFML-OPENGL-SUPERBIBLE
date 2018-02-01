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
	const std::vector<vec4> skyboxVertices = {
		// positions          
		vec4(-1.0f,  1.0f, -1.0f, 1.0f),
		vec4(-1.0f, -1.0f, -1.0f,1.0f),
		vec4(1.0f, -1.0f, -1.0f,1.0f),
		vec4(1.0f, -1.0f, -1.0f,1.0f),
		vec4(1.0f,  1.0f, -1.0f,1.0f),
		vec4(-1.0f,  1.0f, -1.0f,1.0f),

		vec4(-1.0f, -1.0f,  1.0f,1.0f),
		vec4(-1.0f, -1.0f, -1.0f,1.0f),
		vec4(-1.0f,  1.0f, -1.0f,1.0f),
		vec4(-1.0f,  1.0f, -1.0f,1.0f),
		vec4(-1.0f,  1.0f,  1.0f,1.0f),
		vec4(-1.0f, -1.0f,  1.0f,1.0f),

		vec4(1.0f, -1.0f, -1.0f,1.0f),
		vec4(1.0f, -1.0f,  1.0f,1.0f),
		vec4(1.0f,  1.0f,  1.0f,1.0f),
		vec4(1.0f,  1.0f,  1.0f,1.0f),
		vec4(1.0f,  1.0f, -1.0f,1.0f),
		vec4(1.0f, -1.0f, -1.0f,1.0f),

		vec4(-1.0f, -1.0f,  1.0f,1.0f),
		vec4(-1.0f,  1.0f,  1.0f,1.0f),
		vec4(1.0f,  1.0f,  1.0f,1.0f),
		vec4(1.0f,  1.0f,  1.0f,1.0f),
		vec4(1.0f, -1.0f,  1.0f,1.0f),
		vec4(-1.0f, -1.0f,  1.0f,1.0f),

		vec4(-1.0f,  1.0f, -1.0f,1.0f),
		vec4(1.0f,  1.0f, -1.0f,1.0f),
		vec4(1.0f,  1.0f,  1.0f,1.0f),
		vec4(1.0f,  1.0f,  1.0f,1.0f),
		vec4(-1.0f,  1.0f,  1.0f,1.0f),
		vec4(-1.0f,  1.0f, -1.0f,1.0f),

		vec4(-1.0f, -1.0f, -1.0f,1.0f),
		vec4(-1.0f, -1.0f,  1.0f,1.0f),
		vec4(1.0f, -1.0f, -1.0f,1.0f),
		vec4(1.0f, -1.0f, -1.0f,1.0f),
		vec4(-1.0f, -1.0f,  1.0f,1.0f),
		vec4(1.0f, -1.0f,  1.0f,1.0f)
	};

	m_kVAO.Init();
	m_kMesh.Init(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(vec4), (vec4*)skyboxVertices.data(), 0);
	m_kMesh.Bind(GL_ARRAY_BUFFER);
	m_kVAO.SetAttribute(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	m_kVAO.UnBind();

	m_kCubeMap.Initialise();
	m_kCubeMap.InitialiseFromRessource("media/texture/ame_iceflats/");

	m_pkSkyboxProgram = ProgramManager::Instance()->InstantiateFromRessource("media/shaders/SkyboxShader.shader");
}

void Skybox::Delete()
{
	ProgramManager::Instance()->Destroy(m_pkSkyboxProgram);
	m_kCubeMap.Delete();
	m_kMesh.Delete();
	m_kVAO.Delete();
}

void Skybox::Draw(mat4 viewMatrix, mat4 projMatrix)
{
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	m_pkSkyboxProgram->UseProgram();
	glUniformMatrix4fv(0, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(1, 1, GL_FALSE, projMatrix);

	m_kVAO.Bind();
	m_kMesh.Bind(GL_ARRAY_BUFFER);
	m_kCubeMap.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	m_kVAO.UnBind();
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}
