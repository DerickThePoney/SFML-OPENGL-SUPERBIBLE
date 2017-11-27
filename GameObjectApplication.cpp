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
	m_kRenderer.Init();

	//create the mesh
	BMesh kBMesh;
	MakeCube kMakeCube(vec3(0.2f));
	kMakeCube.Apply(kBMesh);
	kBMesh.BuildMesh(m_kMesh);
	m_kMesh.LoadBuffersOnGraphicsCard();

	LoadAndCompileProgram();

	m_kCamera.Init(50, (float)m_window.getSize().x / (float)m_window.getSize().y,
		0.1f,
		1000.0f);

	
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

	kGameObjects[0].m_kTransform.SetLocalOrientation(quaternion(-1, 0, 0, 0));

	m_kCamera.UpdateAllTransformsInHierarchy();
	kGameObjects[0].UpdateAllTransformsInHierarchy();

	//init rendering stuff
	kGameObjects[0].m_kMeshRenderer.m_kMesh = m_kMesh;
	kGameObjects[0].m_kMeshRenderer.m_kMaterial = m_kMaterial;
	kGameObjects[1].m_kMeshRenderer.m_kMesh = m_kMesh;
	kGameObjects[1].m_kMeshRenderer.m_kMaterial = m_kMaterial;
	kGameObjects[2].m_kMeshRenderer.m_kMesh = m_kMesh;
	kGameObjects[2].m_kMeshRenderer.m_kMaterial = m_kMaterial;
}

void GameObjectApplication::Update(double deltaTime)
{

	m_kCamera.UpdateAllTransformsInHierarchy();
	kGameObjects[0].UpdateAllTransformsInHierarchy();

	ImGuiIO &io = ImGui::GetIO();
	

	ImGui::Begin("Hierarchy");
	static GameObject* node_clicked;

	m_kCamera.ImGUIHierarchy(node_clicked);
	kGameObjects[0].ImGUIHierarchy(node_clicked);

	ImGui::End();


	ImGui::Begin("Inspector");

	if (node_clicked != nullptr)
	{
		node_clicked->Inspector();
	}

	ImGui::End();


}



void GameObjectApplication::Render(double currentTime)
{
	std::vector<GameObjectRenderData> visibleList;

	GameObjectRenderData data1;
	data1.m_pkTransform = &kGameObjects[0].m_kTransform;
	data1.m_pkMeshRenderer = &kGameObjects[0].m_kMeshRenderer;
	GameObjectRenderData data2;
	data2.m_pkTransform = &kGameObjects[1].m_kTransform;
	data2.m_pkMeshRenderer = &kGameObjects[1].m_kMeshRenderer;
	GameObjectRenderData data3;
	data3.m_pkTransform = &kGameObjects[2].m_kTransform;
	data3.m_pkMeshRenderer = &kGameObjects[2].m_kMeshRenderer;

	visibleList.push_back(data1);
	visibleList.push_back(data2);
	visibleList.push_back(data3);

	m_kRenderer.Render(visibleList, m_kCamera);

	m_window.display();
}

void GameObjectApplication::Terminate()
{
	ImGui::SFML::Shutdown();
	m_kMaterial.Delete();
	m_kRenderer.Terminate();
}

void GameObjectApplication::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
	m_kCamera.OnResize(width, height);
}

void GameObjectApplication::LoadAndCompileProgram()
{
	std::vector<std::string> shaders; std::vector<GLenum> shadersTypes;
	shaders.push_back("media/shaders/StandardMeshShader.vert.glsl"); shadersTypes.push_back(GL_VERTEX_SHADER);
	shaders.push_back("media/shaders/StandardMeshShader.frag.glsl"); shadersTypes.push_back(GL_FRAGMENT_SHADER);
	
	m_kMaterial.InitFromFile(shaders, shadersTypes);

	GLuint m_hiProjectionDataIndex = glGetUniformBlockIndex(m_kMaterial.m_kProgram, "ProjectionData");
}
