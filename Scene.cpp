#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
	glDeleteTextures(1, &texture);
}

void Scene::Initialise()
{
	m_kCamera.Init(50, (float)800 / (float)600,
		0.1f,
		1000.0f);
	static I32 size = 16;
	char* data = new char[size * size * 4];
	memset(data, 150, size * size * 4);
	
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	OGLUtilities::GetErrors();

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, size, size);
	
	OGLUtilities::GetErrors();

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, data);

	OGLUtilities::GetErrors();

	delete[] data;
}

void Scene::Terminate()
{
	m_akRootObjects.clear();
}

void Scene::Update(double deltaTime)
{
	//update transformss
	m_kCamera.UpdateAllTransformsInHierarchy();
		
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->UpdateAllTransformsInHierarchy();
	}
	
	//update loop
	m_kCamera.Update(deltaTime);
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->Update(deltaTime);
	}

	//update transforms a second time to be sure that components did modify those too much...
	m_kCamera.UpdateAllTransformsInHierarchy();
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->UpdateAllTransformsInHierarchy();
	}

	//IM GUI Stuff
	ImGuiIO &io = ImGui::GetIO();


	ImGui::Begin("Hierarchy");
	static GameObject* node_clicked;

	m_kCamera.ImGUIHierarchy(node_clicked);
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->ImGUIHierarchy(node_clicked);
	}

	ImGui::End();


	ImGui::Begin("Inspector");

	if (node_clicked != nullptr)
	{
		node_clicked->Inspector();
	}

	ImGui::End();

	ImGui::Begin("Test with images");

	ImGui::Image((void *)(intptr_t)texture, ImVec2(512, 512), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImColor(255, 255, 255, 255));

	ImGui::End();

	static bool pOpen = true;
	ImGui::ShowTestWindow(&pOpen);
}

void Scene::OnResize(unsigned int width, unsigned int height)
{
	m_kCamera.OnResize(width, height);
}

void Scene::ExtractVisibleObjectList(std::vector<GameObject*>& akObjects)
{
	//no frustum cull for the moment
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		akObjects.push_back(m_akRootObjects[i]);
		ExtractVisibleObjectsInHierarchy(m_akRootObjects[i], akObjects);
	}
}

void Scene::ExtractVisibleObjectsInHierarchy(GameObject* base, std::vector<GameObject*>& akObjects)
{
	I32 childNb = base->GetNumberOfChildren();
	
	for (I32 j = 0; j < childNb; ++j)
	{
		akObjects.push_back(base->GetChild(j));
		ExtractVisibleObjectsInHierarchy(base->GetChild(j), akObjects);
	}
}


