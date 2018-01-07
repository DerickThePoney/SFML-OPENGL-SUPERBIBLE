#pragma once
#include "Singleton.h"

class DebugRenderingCommands : public Singleton<DebugRenderingCommands>
{
	friend class Singleton<DebugRenderingCommands>;

protected:
	DebugRenderingCommands();
	~DebugRenderingCommands();

public:
	void Reset();
	
	I32 DrawLine(vec4 start, vec4 end, vec4 color);


	void RenderDebugCommands();

private:
	struct LineDrawingCommands
	{
		std::vector<vec4> endpoints;
		std::vector<vec4> colors;

		void InitRender();
		void Terminate();

		void Clear()
		{
			endpoints.clear();
			colors.clear();
		}

		void RenderLines(UI32 uiLineCommandsNb);
	};

	LineDrawingCommands m_kLineCommands;
	UI32 m_uiLineCommandsNb;
};