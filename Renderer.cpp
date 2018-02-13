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

	DebugRenderingCommands::Instance()->Initialise();

	m_pkBlitShader = MaterialManager::Instance()->InstantiateFromFile("media/Materials/BlitMaterial.material");

	//init the default mesh (a cube)
	m_pkDefaultMesh = MeshManager::Instance()->InstantiateFromFile("media/meshes/ColoredCubeSmooth.ply");

	m_pkScreenQuad = MeshManager::Instance()->Instantiate();

	m_pkScreenQuad->m_akVertices.push_back(vec4(-1, -1, 0, 1));
	m_pkScreenQuad->m_akVertices.push_back(vec4(1, -1, 0, 1));
	m_pkScreenQuad->m_akVertices.push_back(vec4(-1, 1, 0, 1));
	m_pkScreenQuad->m_akVertices.push_back(vec4(1, 1, 0, 1));

	m_pkScreenQuad->m_akNormals.push_back(vec3(0, 0, 1));
	m_pkScreenQuad->m_akNormals.push_back(vec3(0, 0, 1));
	m_pkScreenQuad->m_akNormals.push_back(vec3(0, 0, 1));
	m_pkScreenQuad->m_akNormals.push_back(vec3(0, 0, 1));

	m_pkScreenQuad->m_akColor.push_back(vec4(1));
	m_pkScreenQuad->m_akColor.push_back(vec4(1));
	m_pkScreenQuad->m_akColor.push_back(vec4(1));
	m_pkScreenQuad->m_akColor.push_back(vec4(1));

	m_pkScreenQuad->m_akUVs.push_back(vec2(0, 0));
	m_pkScreenQuad->m_akUVs.push_back(vec2(1, 0));
	m_pkScreenQuad->m_akUVs.push_back(vec2(0, 1));
	m_pkScreenQuad->m_akUVs.push_back(vec2(1, 1));


	m_pkScreenQuad->m_aiIndices.push_back(0); m_pkScreenQuad->m_aiIndices.push_back(1); m_pkScreenQuad->m_aiIndices.push_back(2);
	m_pkScreenQuad->m_aiIndices.push_back(2); m_pkScreenQuad->m_aiIndices.push_back(1); m_pkScreenQuad->m_aiIndices.push_back(3);

	m_pkScreenQuad->LoadBuffersOnGraphicsCard();

	//for testing
	
	glEnable(GL_MULTISAMPLE);
	m_kFrameBuffer.Init();
	m_kFrameBuffer.Bind(GL_FRAMEBUFFER);

	//const I32 size = 512;
	m_kScreenTexture.Initialise();
	m_kScreenTexture.InitialiseStorage(m_window->getSize().x, m_window->getSize().y, 1, GL_RGBA32F);
	m_kFrameBuffer.AddTextureAttachement(GL_FRAMEBUFFER, m_kScreenTexture, GL_COLOR_ATTACHMENT0, 0);
	
	m_kDepthTexture.Initialise();
	m_kDepthTexture.InitialiseStorage(m_window->getSize().x, m_window->getSize().y, 1, GL_DEPTH_COMPONENT32);
	m_kFrameBuffer.AddTextureAttachement(GL_FRAMEBUFFER, m_kDepthTexture, GL_DEPTH_ATTACHMENT, 0);

	m_kDepthBuffer.Init(GL_DEPTH_COMPONENT, m_window->getSize().x,
		m_window->getSize().y);
	m_kFrameBuffer.AddRenderBufferAttachement(GL_FRAMEBUFFER, m_kDepthBuffer, GL_DEPTH_ATTACHMENT);

	m_kGlobalRendererSettings.m_kSkybox.Initialise();
}

void Renderer::Terminate()
{
	TerminateDefaultState();
}

void Renderer::TerminateDefaultState()
{
	m_kGlobalRendererSettings.m_kSkybox.Delete();

	//Off screen rendering
	m_kDepthBuffer.Delete();
	m_kDepthTexture.Delete();
	m_kScreenTexture.Delete();
	m_kFrameBuffer.Delete();

	//globals clean up
	MeshManager::Instance()->Destroy(m_pkScreenQuad);
	MeshManager::Instance()->Destroy(m_pkDefaultMesh);
	MaterialManager::Instance()->Destroy(m_pkBlitShader);
	DebugRenderingCommands::Instance()->Delete();
	//MaterialManager::Instance()->Destroy(m_pkLineShaderMaterial);	
	MaterialManager::Instance()->Destroy(m_pkDefaultMaterial);
	
}

void Renderer::Update()
{
	DebugRenderingCommands::Instance()->Reset();
	GraphicsSettings();
}

void Renderer::Render(std::vector<GameObjectRenderData>& kVisibleObjectsList, Camera* pkCamera)
{
	/** Here we should keep a list of OGL State... **/

	ApplyGraphicsSettings();

	//init the state the default state
	ApplyDefaultState();

	auto sz = m_window->getSize();

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	m_kFrameBuffer.BindForDrawing(GL_DRAW_FRAMEBUFFER);
	//glViewport(0, 0, sz.x, sz.y);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	//draw
	static const GLfloat one = 1.0f;

	//clear screen
	glClearColor(m_kGlobalRendererSettings.kClearColor[0], m_kGlobalRendererSettings.kClearColor[1], 
		m_kGlobalRendererSettings.kClearColor[2], m_kGlobalRendererSettings.kClearColor[3]);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Load the camera projections
	pkCamera->LoadProjectionOnGraphics(0);

	//Bind the light settings
	LightingManager::Instance()->PrepareDataForRenderer();

	//traverse the objects
	for (std::size_t i = 0; i < kVisibleObjectsList.size(); ++i)
	{
		ApplyGameObjectRenderData(kVisibleObjectsList[i]);

		//set modelview and proj matrix
		glUniformMatrix4fv(0, 1, GL_FALSE, kVisibleObjectsList[i].m_pkTransform->GetWorldSpaceTransform());
		//glUniformMatrix4fv(1, 1, GL_FALSE, vmath::invert(kVisibleObjectsList[i].m_pkTransform->GetWorldSpaceTransform()));

		//draw call
		glDrawElements(GL_TRIANGLES, kVisibleObjectsList[i].m_pkMeshRenderer->m_pkMesh->m_aiIndices.size(), GL_UNSIGNED_INT, 0);
	}

	m_kGlobalRendererSettings.m_kSkybox.Draw();

	if (kVisibleObjectsList.size() > 0)
	{
		//draw a line onto the screen
		vec4 objectWorldPos = kVisibleObjectsList[1].m_pkTransform->GetWorldspacePosition();
		vec4 cameraWorldPos = kVisibleObjectsList[0].m_pkTransform->GetWorldspacePosition();

		vec4 direction = objectWorldPos - cameraWorldPos;
		DebugRenderingCommands::Instance()->DrawLine(cameraWorldPos, objectWorldPos, vec4(0,0,1,1));

		objectWorldPos = kVisibleObjectsList[2].m_pkTransform->GetWorldspacePosition();
		direction = objectWorldPos - cameraWorldPos;
		DebugRenderingCommands::Instance()->DrawLine(cameraWorldPos, objectWorldPos, vec4(1,0,0,1));

	}

	//Draw rendering commands
	DebugRenderingCommands::Instance()->RenderDebugCommands(pkCamera);

	m_kFrameBuffer.UnBind(GL_DRAW_FRAMEBUFFER);
	glViewport(0, 0, sz.x, sz.y);
	
	if (m_kGlobalRendererSettings.bBlit)
	{
		m_kFrameBuffer.Bind(GL_READ_FRAMEBUFFER);
		glBlitFramebuffer(0, 0, sz.x, sz.y, 0, 0, sz.x, sz.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		m_kFrameBuffer.UnBind(GL_READ_FRAMEBUFFER);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glClear(GL_COLOR_BUFFER_BIT);
		//glClearBufferfv(GL_COLOR, 0, vec4(1,0,0,1));
		//glClearBufferfv(GL_DEPTH, 0, &one);
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//glViewport(0, 0, sz.x, sz.y);
		m_pkBlitShader->Use();
		m_auiRenderingState[MATERIAL] = m_pkBlitShader->m_uiMaterialID;
		//glDisable(GL_DEPTH_TEST);
		m_kScreenTexture.Bind();
		m_pkScreenQuad->BindForDrawing();
		glDrawElements(GL_TRIANGLES, m_pkScreenQuad->m_aiIndices.size(), GL_UNSIGNED_INT, 0);
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
		if (data.m_pkMeshRenderer->m_pkMaterial->m_uiMaterialID != m_auiRenderingState[MATERIAL] ||
			data.m_pkMeshRenderer->m_pkMaterial->m_kMaterialData.m_bPerInstanceData)
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
//			data.m_pkMeshRenderer->m_pkMesh->ClearLocalData();
		}
	}
}

void Renderer::OnResize(unsigned int width, unsigned int height)
{
	m_kFrameBuffer.Bind(GL_FRAMEBUFFER);
	
	m_kScreenTexture.InitialiseStorage(width, height, 1, GL_RGBA32F);
	m_kFrameBuffer.AddTextureAttachement(GL_FRAMEBUFFER, m_kScreenTexture, GL_COLOR_ATTACHMENT0, 0);

	m_kDepthTexture.InitialiseStorage(width, height, 1, GL_DEPTH_COMPONENT32);
	m_kFrameBuffer.AddTextureAttachement(GL_FRAMEBUFFER, m_kDepthTexture, GL_DEPTH_ATTACHMENT, 0);

	m_kDepthBuffer.Delete();
	m_kDepthBuffer.Init(GL_DEPTH_COMPONENT, width, height);
	m_kFrameBuffer.AddRenderBufferAttachement(GL_FRAMEBUFFER, m_kDepthBuffer, GL_DEPTH_ATTACHMENT);

	m_kFrameBuffer.UnBind(GL_FRAMEBUFFER);

}

void Renderer::ApplyGraphicsSettings()
{
	//depth testing
	if (m_kGlobalRendererSettings.bDepthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}


	//faces culling
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

	ImGui::LabelText("Hardware vendor","%s", m_kRendererData.m_kHardwareVendor);
	ImGui::LabelText("Renderer name", "%s", m_kRendererData.m_kRendererName);

	if (ImGui::Checkbox("VSync", &m_kGlobalRendererSettings.m_bVSync)) SetVerticalSynchronisation();

	
	if(ImGui::ColorEdit4("Clear color", m_kGlobalRendererSettings.kClearColor.GetData())) modif = true;

	if(ImGui::Checkbox("Depth testing", &m_kGlobalRendererSettings.bDepthTesting)) modif = true;

	ImGui::Checkbox("Blit ", &m_kGlobalRendererSettings.bBlit);

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

	static bool bShowSkybox = false;
	ImGui::Checkbox("Show skybox", &bShowSkybox);

	if(bShowSkybox)
	{
		m_kGlobalRendererSettings.m_kSkybox.Inspect();
	}

	ImGui::End();

	if (modif) ApplyGraphicsSettings();
}

void Renderer::SetVerticalSynchronisation()
{
	m_window->setVerticalSyncEnabled(m_kGlobalRendererSettings.m_bVSync);
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

	SetVerticalSynchronisation();
}
