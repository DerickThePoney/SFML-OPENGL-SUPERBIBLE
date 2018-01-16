#include "stdafx.h"
#include "DebugRenderingCommands.h"

DebugRenderingCommands* Singleton<DebugRenderingCommands>::s_kInstance = nullptr;

DebugRenderingCommands::DebugRenderingCommands()
{
}

DebugRenderingCommands::~DebugRenderingCommands()
{
	m_kLineCommands.Terminate();
	m_uiLineCommandsNb = 0;
}

void DebugRenderingCommands::Initialise()
{
	m_kLineCommands.InitRender();
	m_uiLineCommandsNb = 0;
}

void DebugRenderingCommands::Reset()
{
	m_kLineCommands.Clear();
	m_uiLineCommandsNb = 0;
}

I32 DebugRenderingCommands::DrawLine(vec4 start, vec4 end, vec4 color)
{
	m_kLineCommands.endpoints.push_back(start); m_kLineCommands.endpoints.push_back(end);
	m_kLineCommands.colors.push_back(color);
	return ++m_uiLineCommandsNb;
}

void DebugRenderingCommands::RenderDebugCommands(Camera& kCamera)
{
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	
	m_kLineCommands.RenderLines(m_uiLineCommandsNb, kCamera);

	glUseProgram(last_program);
}

//Line drawing
void DebugRenderingCommands::LineDrawingCommands::InitRender()
{
	m_pkMaterial = MaterialManager::Instance()->InstantiateFromFile("media/Materials/LineShader.material");
	m_hkVao.Init();
	m_hkVao.Bind();

	m_kVertices.Init(GL_ARRAY_BUFFER, 4 * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	m_hkVao.SetAttribute(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
}

void DebugRenderingCommands::LineDrawingCommands::UpdateData()
{
	m_kVertices.UpdateData(GL_ARRAY_BUFFER, endpoints.size() * sizeof(vec4), endpoints.data(), GL_WRITE_ONLY, GL_DYNAMIC_DRAW);
}

void DebugRenderingCommands::LineDrawingCommands::Terminate()
{
	m_kVertices.Delete();
	m_hkVao.Delete();
	MaterialManager::Instance()->Destroy(m_pkMaterial);
}

void DebugRenderingCommands::LineDrawingCommands::RenderLines(UI32 uiLineCommandsNb, Camera& kCamera)
{
	m_pkMaterial->Use();
	kCamera.LoadProjectionOnGraphics(0);
	m_hkVao.Bind();
	UpdateData();
	glDrawArrays(GL_LINES, 0, endpoints.size());
}
