#pragma once
#include "Singleton.h"

class DebugRenderingCommands : public Singleton<DebugRenderingCommands>
{
	friend class Singleton<DebugRenderingCommands>;

protected:
	DebugRenderingCommands();
	~DebugRenderingCommands();

public:
	void Initialise();
	void Reset();
	
	I32 DrawLine(vec4 start, vec4 end, vec4 color);

	void RenderDebugCommands(Camera& kCamera);

private:
	struct LineDrawingCommands
	{
		std::vector<vec4> endpoints;
		std::vector<vec4> colors;
		Material* m_pkMaterial;
		OGLVertexArray m_hkVao;
		OGLBuffer m_kVertices;

		void InitRender();
		void UpdateData();
		void Terminate();

		void Clear()
		{
			endpoints.clear();
			colors.clear();
		}

		void RenderLines(UI32 uiLineCommandsNb, Camera& kCamera);
	};

	LineDrawingCommands m_kLineCommands;
	UI32 m_uiLineCommandsNb;
};