#pragma once
#include "Application.h"

class OGLApplication : public Application
{
public:
	OGLApplication();
	virtual ~OGLApplication();
	void Render(double currentTime) override;
	void Initialise() override;
	void Terminate() override;

	GLuint CompileShaders();

protected:
	GLuint m_iProgram;
	GLuint m_iVertexArrayObject;
};

