#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
	for (size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		GameObjectManager::Instance()->Destroy(m_akRootObjects[i]);
	}

	m_kTexture.Delete();
}

void Scene::Initialise()
{
	m_kCamera.Init(50, (float)800 / (float)600,
		0.1f,
		1000.0f);

	//Just make a simple texture
	static I32 size = 16;
	char* data = new char[size * size * 4];
	//memset(data, 255, size * size * 4);

	for (I32 i = 0; i < size*size * 4; ++i)
	{
		char value = rand() & 0xFF;
		data[i] = value;
	}
	
	m_kTexture.Initialise();
	m_kTexture.InitialiseStorage(size, size, 1, GL_RGBA32F);
	m_kTexture.SetData(0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, data, false);
	
	delete[] data;
}

void Scene::Terminate()
{
	m_akRootObjects.clear();
}

void Scene::Update(double deltaTime)
{
	//update transformss
	TransformsUpdate();
	
	//update loop
	m_kCamera.Update(deltaTime);
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->Update(deltaTime);
	}

	//update transforms a second time to be sure that components did modify those too much...
	TransformsUpdate();

	//IM GUI Stuff
	IMGuiStuffs();
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

void Scene::TransformsUpdate()
{
	m_kCamera.UpdateAllTransformsInHierarchy();
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->UpdateAllTransformsInHierarchy();
	}
}

void Scene::IMGuiStuffs()
{
	ImGuiIO &io = ImGui::GetIO();


	ImGui::Begin("Hierarchy");
	static GameObject* node_clicked = nullptr;
	
	//Add -- remove -- and all that
	if (ImGui::Button("+"))
	{
		GameObject* newObj = GameObjectManager::Instance()->Instantiate();
		newObj->SetName("New Game Object");
		if (node_clicked != nullptr)
		{
			node_clicked->AddChild(newObj);
		}
		else
		{
			m_akRootObjects.push_back(newObj);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		if (node_clicked != nullptr)
		{
			//if(node_clicked->)
			//node_clicked->AddChild(newObj);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
	}

	//objects inspection
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

	ImGui::Image((void *)(intptr_t)m_kTexture, ImVec2(512, 512), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImColor(255, 255, 255, 255));

	ImGui::End();

	static bool pOpen = true;
	ImGui::ShowTestWindow(&pOpen);
}


