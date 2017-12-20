#pragma once
//#include "vmath.h"
#include "Utility.h"
#include "Object.h"
#include "OGLRendering.h"
//#include "GUIManager.h"

#define TIME_MSEC_DOUBLE(x) ((double)(x).count() / 1000000.0)
#define TIME_SEC_DOUBLE(x) ((double)(x).count() / 1000000000.0)

#define TIME_MSEC_FLOAT(x) ((float)TIME_MSEC_DOUBLE(x))
#define TIME_SEC_FLOAT(x) ((float)TIME_SEC_DOUBLE(x))

class Application
{
public:
	Application();
	virtual ~Application();

	virtual void Initialise() {}
	virtual void Terminate() {}
	virtual void Render(double currentTime) { ImGui::Render(); m_window.display(); }
	virtual void Update(double deltaTime) {}
	virtual void OnResize(unsigned int width, unsigned int height);

	virtual void HandleMessages();
	void InterpretMessage(sf::Event event);
	void MainLoop();

	GLuint LoadShader(const char * pcFilename, GLenum eShaderType, bool bCheckErrors);
	GLuint LinkProgram(const GLuint * uiShaders, int iShaderCount, bool bDeleteShaders, bool bCheckErrors);

protected:
	sf::RenderWindow m_window;
	bool m_bOnEscapeQuit;
	bool m_bRunning;
};

