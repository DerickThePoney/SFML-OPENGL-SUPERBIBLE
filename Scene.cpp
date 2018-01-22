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

void Scene::Initialise(UI32 width, UI32 height)
{
	//m_kCamera.Init(width, height);

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
	//m_kCamera.Update(deltaTime);
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->Update(deltaTime);
	}

	//update transforms a second time to be sure that components did modify those too much...
	TransformsUpdate();

	//IM GUI Stuff
	IMGuiStuffs();
}

void Scene::OnPreRender()
{
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		m_akRootObjects[i]->OnPreRender();
	}

}

void Scene::OnResize(unsigned int width, unsigned int height)
{
	//m_kCamera.OnResize(width, height);
}

void Scene::ExtractVisibleObjectList(std::vector<MeshRendererComponent*>& akObjects)
{
	//no frustum cull for the moment
	for (std::size_t i = 0; i < m_akRootObjects.size(); ++i)
	{
		MeshRendererComponent* pkComponent = m_akRootObjects[i]->FindComponent<MeshRendererComponent>().get();
		if(pkComponent != nullptr)	akObjects.push_back(pkComponent);
		ExtractVisibleObjectsInHierarchy(m_akRootObjects[i], akObjects);
	}
}

void Scene::ExtractVisibleObjectsInHierarchy(GameObject* base, std::vector<MeshRendererComponent*>& akObjects)
{
	I32 childNb = base->GetNumberOfChildren();
	
	for (I32 j = 0; j < childNb; ++j)
	{
		MeshRendererComponent* pkComponent = base->GetChild(j)->FindComponent<MeshRendererComponent>().get();
		if (pkComponent != nullptr)	akObjects.push_back(pkComponent);
		ExtractVisibleObjectsInHierarchy(base->GetChild(j), akObjects);
	}
}

void Scene::TransformsUpdate()
{
	//m_kCamera.UpdateAllTransformsInHierarchy();
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
		try
		{
			std::ofstream ofstr(m_kFilename);
			//Scene testLoading;
			cereal::XMLOutputArchive output(ofstr);
			output(*this);
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	//objects inspection
	//m_kCamera.ImGUIHierarchy(node_clicked);
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

	m_kTexture.Inspect();
	
	ImGui::End();

	static bool pOpen = true;
	ImGui::ShowTestWindow(&pOpen);
}


