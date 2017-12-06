#include "stdafx.h"
#include "GameObjectApplication.h"


GameObjectApplication::GameObjectApplication()
{
}


GameObjectApplication::~GameObjectApplication()
{
}

void GameObjectApplication::Initialise()
{
	//Init the Renderer System
	m_kRenderer.Init(&m_window);

	//create the mesh
	m_pkMesh = MeshManager::Instance()->Instantiate();
	BMesh kBMesh;
	MakeCube kMakeCube(vec3(0.2f));
	kMakeCube.Apply(kBMesh);
	kBMesh.BuildMesh(*m_pkMesh);
	m_pkMesh->LoadBuffersOnGraphicsCard();

	LoadAndCompileProgram();

	{
		std::ifstream ifstr("MyFirstScene.xml");
		Scene testLoading;
		cereal::XMLInputArchive output(ifstr);
		output(testLoading);
	}
	
	//init objects and hierarchy
	m_pkGameObjects = GameObjectManager::Instance()->Instantiate();
	m_pkGameObjects->SetName("BaseObject");

	GameObject* middleObject = GameObjectManager::Instance()->Instantiate();
	middleObject->SetName("Middle");

	GameObject* leafObject = GameObjectManager::Instance()->Instantiate();
	leafObject->SetName("LeafObject");

	m_pkGameObjects->AddChild(middleObject);
	middleObject->AddChild(leafObject);

	//init positional stuff
	m_pkGameObjects->m_kTransform.SetLocalPosition(vec3(0, 0, 20));
	middleObject->m_kTransform.SetLocalPosition(vec3(1, 0, 0));
	leafObject->m_kTransform.SetLocalPosition(vec3(1, 0, 0));

	m_pkGameObjects->m_kTransform.SetLocalOrientation(quaternion(1, 0, 0, 0));

	//init rendering stuff
	m_pkGameObjects->m_kMeshRenderer.Init(m_pkMesh, &m_kMaterial);
	middleObject->m_kMeshRenderer.Init(m_pkMesh, &m_kMaterial);
	leafObject->m_kMeshRenderer.Init(m_pkMesh, &m_kMaterial);
	
	//Init Scene
	m_kScene.Initialise();
	m_kScene.AddRootObject(m_pkGameObjects);

	//test scene serialisation
	std::ofstream ofstr("MyFirstScene.xml");
	cereal::XMLOutputArchive output(ofstr);
	output(cereal::make_nvp("MyFirstScene", m_kScene));
}

void GameObjectApplication::Update(double deltaTime)
{
	m_kScene.Update(deltaTime);
}



void GameObjectApplication::Render(double currentTime)
{
	std::vector<GameObject*> visibleObjectList;
	std::vector<GameObjectRenderData> visibleDataList;

	// scene culling
	m_kScene.ExtractVisibleObjectList(visibleObjectList);

	//extract render data
	for (size_t i = 0; i < visibleObjectList.size(); ++i)
	{
		GameObjectRenderData data;
		data.m_pkMeshRenderer = &visibleObjectList[i]->m_kMeshRenderer;
		data.m_pkTransform = &visibleObjectList[i]->m_kTransform;
		visibleDataList.push_back(data);
	}
	
	//render
	m_kRenderer.Render(visibleDataList, m_kScene.GetCamera());
}

void GameObjectApplication::Terminate()
{
	m_kScene.Terminate();
	MeshManager::Instance()->Destroy(m_pkMesh);
	MeshManager::Delete();
	GameObjectManager::Delete();
	ImGui::SFML::Shutdown();
	m_kMaterial.Delete();
	m_kRenderer.Terminate();
}

void GameObjectApplication::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
	m_kScene.OnResize(width, height);
}

void GameObjectApplication::LoadAndCompileProgram()
{
	std::vector<std::string> shaders; std::vector<GLenum> shadersTypes;
	shaders.push_back("media/shaders/StandardMeshShader.vert.glsl"); shadersTypes.push_back(GL_VERTEX_SHADER);
	shaders.push_back("media/shaders/StandardMeshShader.frag.glsl"); shadersTypes.push_back(GL_FRAGMENT_SHADER);
	
	m_kMaterial.InitFromFile(shaders, shadersTypes);

	GLuint m_hiProjectionDataIndex = glGetUniformBlockIndex(m_kMaterial.m_kProgram, "ProjectionData");
}
