#include "stdafx.h"
#include "DebugRenderingCommands.h"

DebugRenderingCommands* Singleton<DebugRenderingCommands>::s_kInstance = nullptr;

DebugRenderingCommands::DebugRenderingCommands()
{
}

DebugRenderingCommands::~DebugRenderingCommands()
{
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

void DebugRenderingCommands::RenderDebugCommands()
{
	m_kLineCommands.RenderLines(m_uiLineCommandsNb);
}

void DebugRenderingCommands::LineDrawingCommands::RenderLines(UI32 uiLineCommandsNb)
{

}
