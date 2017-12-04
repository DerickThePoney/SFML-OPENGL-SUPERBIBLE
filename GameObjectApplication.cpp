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
	BMesh kBMesh;
	MakeCube kMakeCube(vec3(0.2f));
	kMakeCube.Apply(kBMesh);
	kBMesh.BuildMesh(m_kMesh);
	m_kMesh.LoadBuffersOnGraphicsCard();

	LoadAndCompileProgram();

	
	//init objects and hierarchy
	kGameObjects[0].SetName("BaseObject");
	kGameObjects[1].SetName("Middle");
	kGameObjects[2].SetName("LeafObject");

	kGameObjects[0].AddChild(&kGameObjects[1]);
	kGameObjects[1].AddChild(&kGameObjects[2]);

	//init positional stuff
	kGameObjects[0].m_kTransform.SetLocalPosition(vec3(0, 0, 20));
	kGameObjects[1].m_kTransform.SetLocalPosition(vec3(1, 0, 0));
	kGameObjects[2].m_kTransform.SetLocalPosition(vec3(1, 0, 0));

	kGameObjects[0].m_kTransform.SetLocalOrientation(quaternion(1, 0, 0, 0));

	//init rendering stuff
	kGameObjects[0].m_kMeshRenderer.Init(&m_kMesh, &m_kMaterial);
	kGameObjects[1].m_kMeshRenderer.Init(&m_kMesh, &m_kMaterial);
	kGameObjects[2].m_kMeshRenderer.Init(&m_kMesh, &m_kMaterial);
	
	//Init Scene
	m_kScene.Initialise();
	m_kScene.AddRootObject(kGameObjects);

	//test scene serialisation
	cereal::XMLOutputArchive output(std::cout);
	output(cereal::make_nvp("My first scene", m_kScene));
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
