#include "stdafx.h"
#include "Skybox.h"

Skybox::Skybox()
	: m_kSkyboxAsset("media/skyboxes/default_skybox.skybox")
{
}

Skybox::~Skybox()
{
}

void Skybox::Initialise()
{
	//make cube mesh
	BMesh bmesh;
	MakeCube kMakeCube;
	kMakeCube.Apply(bmesh);

	m_pkMesh = MeshManager::Instance()->Instantiate();
	bmesh.BuildMesh(*m_pkMesh);

	m_pkMesh->LoadBuffersOnGraphicsCard();

	//Retrieve skybox data
	TRY_READ_DATA(m_kSkyboxAsset, m_kData);

	//cube map
	m_kCubeMap.Initialise();
	m_kCubeMap.InitialiseFromRessource(m_kData.m_akFilenames);

	m_pkSkyboxProgram = ProgramManager::Instance()->InstantiateFromRessource(m_kData.m_kProgram);
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

void Skybox::Inspect()
{
	ImGui::LabelText("Skybox asset file name", m_kSkyboxAsset.c_str());
	if (ImGui::Button("Save data"))
	{
		std::ofstream ofstr(m_kSkyboxAsset.c_str());
		cereal::XMLOutputArchive output(ofstr);
		output(m_kData);
	}

	ImGui::LabelText("Program", m_kData.m_kProgram.c_str());

	for (int i = 0; i < 6; ++i)
	{
		std::string file = OGLCubeMap::GetFilenameFromIndex(i) + " texture";
		ImGui::LabelText(file.c_str(), m_kData.m_akFilenames[i].c_str());
	}
}
