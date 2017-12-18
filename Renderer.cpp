#include "stdafx.h"
#include "Renderer.h"


Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::Init(sf::RenderWindow* m_window)
{
	this->m_window = m_window;
	ExtractGLContextInformation();
	InitDefaultState();
}

void Renderer::ExtractGLContextInformation()
{
	glGetIntegerv(GL_MAJOR_VERSION, &m_kRendererData.m_iGLMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &m_kRendererData.m_iGLMinorVersion);
	m_kRendererData.m_kHardwareVendor = glGetString(GL_VENDOR);
	m_kRendererData.m_kRendererName = glGetString(GL_RENDERER);
}

void Renderer::InitDefaultState()
{
	//init the default shaders
	std::vector<std::string> shaders; std::vector<GLenum> shadersTypes;
	shaders.push_back("media/shaders/DefaultShader.vert.glsl"); shadersTypes.push_back(GL_VERTEX_SHADER);
	shaders.push_back("media/shaders/DefaultShader.frag.glsl"); shadersTypes.push_back(GL_FRAGMENT_SHADER);

	m_kDefaultMaterial.InitFromFiles(shaders, shadersTypes);

	m_hiProjectionDataIndex = glGetUniformBlockIndex(m_kDefaultMaterial.m_kProgram, "ProjectionData");

	//init the default mesh (a cube)
	m_pkDefaultMesh = MeshManager::Instance()->InstantiateFromFile("media/meshes/default_cube_mesh.bin");
	/*m_pkDefaultMesh = MeshManager::Instance()->Instantiate();
	BMesh bmesh;
	MakeCube makeCube;
	makeCube.Apply(bmesh);
	
	bmesh.BuildMesh(*m_pkDefaultMesh);*/
	m_pkDefaultMesh->LoadBuffersOnGraphicsCard();
}

void Renderer::Terminate()
{
	TerminateDefaultState();
}

void Renderer::TerminateDefaultState()
{
	MeshManager::Instance()->Destroy(m_pkDefaultMesh);
	m_kDefaultMaterial.Delete();
}

void Renderer::Render(std::vector<GameObjectRenderData>& kVisibleObjectsList, Camera& kCamera)
{
	/** Here we should keep a list of OGL State... **/

	//init the state the default state
	ApplyDefaultState();

	
	//draw
	static const GLfloat one = 1.0f;

	//clear screen
	glClearBufferfv(GL_COLOR, 0, vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, &one);
	
	// Load the camera projections
	kCamera.LoadProjectionOnGraphics(0);

	//traverse the objects
	for (std::size_t i = 0; i < kVisibleObjectsList.size(); ++i)
	{
		ApplyGameObjectRenderData(kVisibleObjectsList[i]);

		//set modelview and proj matrix
		glUniformMatrix4fv(0, 1, GL_FALSE, kVisibleObjectsList[i].m_pkTransform->GetWorldSpaceTransform());

		//draw call
		glDrawElements(GL_TRIANGLES, kVisibleObjectsList[i].m_pkMeshRenderer->m_pkMesh->m_aiIndices.size(), GL_UNSIGNED_INT, 0);
	}

	//Render ImGUI
	ImGui::Render();

	//swap buffers
	m_window->display();
}



void Renderer::ApplyDefaultState()
{
	//default material
	m_kDefaultMaterial.Use();
	m_auiRenderingState[MATERIAL] = m_kDefaultMaterial.m_uiMaterialID;

	//default mesh
	m_pkDefaultMesh->BindForDrawing();
	m_auiRenderingState[MESH] = m_pkDefaultMesh->m_uiMeshID;
}

void Renderer::ApplyGameObjectRenderData(GameObjectRenderData & data)
{
	if (data.m_pkMeshRenderer->m_pkMaterial != nullptr)
	{
		if (data.m_pkMeshRenderer->m_pkMaterial->m_uiMaterialID != m_auiRenderingState[MATERIAL])
		{
			data.m_pkMeshRenderer->m_pkMaterial->Use();
			m_auiRenderingState[MATERIAL] = data.m_pkMeshRenderer->m_pkMaterial->m_uiMaterialID;
		}
	}

	if (data.m_pkMeshRenderer->m_pkMesh != nullptr)
	{
		if (data.m_pkMeshRenderer->m_pkMesh->m_uiMeshID != m_auiRenderingState[MESH])
		{
			data.m_pkMeshRenderer->m_pkMesh->BindForDrawing();
			m_auiRenderingState[MESH] = data.m_pkMeshRenderer->m_pkMesh->m_uiMeshID;
		}
	}
}
