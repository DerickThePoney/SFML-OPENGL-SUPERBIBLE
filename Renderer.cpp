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

	LoadSettings();

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
	m_pkDefaultMaterial = MaterialManager::Instance()->InstantiateFromFile("media/Materials/DefaultMaterial.material");

	m_hiProjectionDataIndex = glGetUniformBlockIndex(*m_pkDefaultMaterial->m_pkProgram, "ProjectionData");

	//init the default mesh (a cube)
	m_pkDefaultMesh = MeshManager::Instance()->InstantiateFromFile("media/meshes/default_cube_mesh.bin");
}

void Renderer::Terminate()
{
	TerminateDefaultState();
}

void Renderer::TerminateDefaultState()
{
	MeshManager::Instance()->Destroy(m_pkDefaultMesh);
	MaterialManager::Instance()->Destroy(m_pkDefaultMaterial);
}

void Renderer::Update()
{
	GraphicsSettings();
}

void Renderer::Render(std::vector<GameObjectRenderData>& kVisibleObjectsList, Camera& kCamera)
{
	/** Here we should keep a list of OGL State... **/

	ApplyGraphicsSettings();

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

		vec4 objectWorldPos = kVisibleObjectsList[i].m_pkTransform->GetWorldspacePosition();
		vec4 cameraWorldPos = kCamera.m_kTransform.GetWorldspacePosition();

		vec4 direction = objectWorldPos - cameraWorldPos;
		Line l(vec3(cameraWorldPos[0], cameraWorldPos[1], cameraWorldPos[2]), vec3(direction[0], direction[1], direction[2]));

		ApplyDefaultState();
		//set modelview and proj matrix
		glUniformMatrix4fv(0, 1, GL_FALSE, mat4::identity());
		l.Draw(1);
	}

	//Render ImGUI
	ImGui::Render();

	//swap buffers
	m_window->display();
}



void Renderer::ApplyDefaultState()
{
	//default material
	m_pkDefaultMaterial->Use();
	m_auiRenderingState[MATERIAL] = m_pkDefaultMaterial->m_uiMaterialID;

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
	else
	{
		//if no material make sure the default is at least
		m_pkDefaultMaterial->Use();
		m_auiRenderingState[MATERIAL] = m_pkDefaultMaterial->m_uiMaterialID;
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

void Renderer::ApplyGraphicsSettings()
{
	(m_kGlobalRendererSettings.bCullFaces) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	
	if (m_kGlobalRendererSettings.bCullFaces)
	{
		(m_kGlobalRendererSettings.bIsCCW) ? glFrontFace(GL_CCW) : glFrontFace(GL_CW);
		if (m_kGlobalRendererSettings.bCullBack && m_kGlobalRendererSettings.bCullFront)
		{
			glCullFace(GL_FRONT_AND_BACK);
		}
		else if (m_kGlobalRendererSettings.bCullBack && !m_kGlobalRendererSettings.bCullFront)
		{
			glCullFace(GL_BACK);
		}
		else if (!m_kGlobalRendererSettings.bCullBack && m_kGlobalRendererSettings.bCullFront)
		{
			glCullFace(GL_FRONT);
		}
		else if (!m_kGlobalRendererSettings.bCullBack && !m_kGlobalRendererSettings.bCullFront)
		{
			glDisable(GL_CULL_FACE);
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, m_kGlobalRendererSettings.ePolygonMode);

}

void Renderer::GraphicsSettings()
{
	bool modif = false;
	ImGui::Begin("GraphicsSettings");

	if (ImGui::Button("Save settings"))
	{
		SaveSettings();
	}

	ImGui::SameLine();
	if (ImGui::Button("Reload settings"))
	{
		LoadSettings();
	}

	
	//bool cullFaces = glIsEnabled(GL_CULL_FACE);
	if (ImGui::Checkbox("Faces culling", &m_kGlobalRendererSettings.bCullFaces)) modif = true;

	if (m_kGlobalRendererSettings.bCullFaces)
	{
		ImGui::Indent(10);
		
		if (ImGui::Checkbox("Is Counterclock Wise", &m_kGlobalRendererSettings.bIsCCW)) modif = true; 

		if (ImGui::Checkbox("Cull Front", &m_kGlobalRendererSettings.bCullFront)) modif = true;
		if (ImGui::Checkbox("Cull Back", &m_kGlobalRendererSettings.bCullBack)) modif = true;
		
		ImGui::Unindent(10);
	}

	static char* polygonModes[3] = { "GL_POINT" , "GL_LINE" , "GL_FILL" };
	int index = 0;
	switch (m_kGlobalRendererSettings.ePolygonMode)
	{
	case GL_POINT:
		index = 0;
		break;
	case GL_LINE:
		index = 1;
		break;
	case GL_FILL:
		index = 2;
		break;
	default:
		break;
	}

	if (ImGui::Combo("Polygon mode", &index, polygonModes, 3))
	{
		switch (index)
		{
		case 0:
			m_kGlobalRendererSettings.ePolygonMode = GL_POINT;
			break;
		case 1:
			m_kGlobalRendererSettings.ePolygonMode = GL_LINE;
			break;
		case 2:
			m_kGlobalRendererSettings.ePolygonMode = GL_FILL;
			break;
		}

		modif = true;
	}
	
	ImGui::End();

	if (modif) ApplyGraphicsSettings();
}

void Renderer::SaveSettings()
{
	std::ofstream ofstr("settings/GraphicsSettings.xml");
	cereal::XMLOutputArchive output(ofstr);
	output(m_kGlobalRendererSettings);
}

void Renderer::LoadSettings()
{
	try
	{
		std::ifstream ifstr("settings/GraphicsSettings.xml");

		if (!ifstr.good())
		{
			SaveSettings();
		}
		else
		{
			//Get Graphics settings
			cereal::XMLInputArchive output(ifstr);
			output(m_kGlobalRendererSettings);
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
