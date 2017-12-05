#pragma once


class OGLUtilities
{
public:
	static const char* GLTypeEnumToCString(GLenum eType);
	static const char* GLDebugEnumToCString(GLenum eType);

	static GLenum GetErrors();

	static void APIENTRY DebugCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const GLvoid* userParam);
};