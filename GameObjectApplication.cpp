#include "stdafx.h"
#include "GameObjectApplication.h"


GameObjectApplication::GameObjectApplication()
	: m_pkMesh(nullptr)
{
}


GameObjectApplication::~GameObjectApplication()
{
}

void GameObjectApplication::Initialise()
{
	Application::Initialise();
	CameraManager::Instance()->Initialise();
	LightingManager::Instance()->Initialise();

	UI32 width = m_window.getSize().x;
	UI32 height = m_window.getSize().y;

	//Init the Renderer System
	m_kRenderer.Init(&m_window);

	//create the mesh
	LoadAndCompileProgram();

	try
	{
		std::ifstream ifstr("media/scenes/MyFirstScene.xml");
		//Scene testLoading;
		cereal::XMLInputArchive output(ifstr);
		output(m_kScene);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	
	//Initialise the scene
	m_kScene.Initialise(width, height);
	mat4 id = vmath::mat4::identity();
	CameraManager::Instance()->LateInit(width, height, id);
}

void GameObjectApplication::Update(double deltaTime)
{
	Application::Update(deltaTime);
	m_kRenderer.Update();
	m_kScene.Update(deltaTime);
}



void GameObjectApplication::Render(double currentTime)
{
	//Prepare for rendering
	m_kScene.OnPreRender();

	//
	std::vector<MeshRendererComponent*> visibleObjectList;
	std::vector<GameObjectRenderData> visibleDataList;

	// scene culling
	m_kScene.ExtractVisibleObjectList(visibleObjectList);

	//extract render data
	for (size_t i = 0; i < visibleObjectList.size(); ++i)
	{
		if (visibleObjectList[i]->m_pkMesh != nullptr) // no data to display...
		{
			GameObjectRenderData data;
			data.m_pkMeshRenderer = visibleObjectList[i];
			data.m_pkTransform = &visibleObjectList[i]->m_pkParent->m_kTransform;
			visibleDataList.push_back(data);
		}
	}
	
	//render
	m_kRenderer.Render(visibleDataList, m_kScene.GetCamera());
}

void GameObjectApplication::Terminate()
{
	m_kScene.Terminate();
	MeshManager::Instance()->Destroy(m_pkMesh);
	MeshManager::Delete();
	MaterialManager::Delete();
	GameObjectManager::Delete();
	ImGui::SFML::Shutdown();
	//m_kMaterial.Delete();
	m_kRenderer.Terminate();
	LightingManager::Delete();
	CameraManager::Delete();
	Application::Terminate();
}

void GameObjectApplication::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
	m_kScene.OnResize(width, height);
	m_kRenderer.OnResize(width, height);
	CameraManager::Instance()->OnResize(width, height);
}

void GameObjectApplication::LoadAndCompileProgram()
{
	/*std::vector<std::string> shaders; std::vector<GLenum> shadersTypes;
	shaders.push_back("media/shaders/StandardMeshShader.vert.glsl"); shadersTypes.push_back(GL_VERTEX_SHADER);
	shaders.push_back("media/shaders/StandardMeshShader.frag.glsl"); shadersTypes.push_back(GL_FRAGMENT_SHADER);
	
	m_kMaterial.InitFromFiles(shaders, shadersTypes);

	GLuint m_hiProjectionDataIndex = glGetUniformBlockIndex(m_kMaterial.m_kProgram, "ProjectionData");*/
}
