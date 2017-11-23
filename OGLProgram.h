#pragma once
#include "OGLShader.h"

struct ActiveProgramVertexAttributes
{
	~Act
	GLsizei length;
	GLint size;
	GLenum type;
	GLchar* name;
};


class OGLProgram
{
public:
	OGLProgram();
	~OGLProgram();

	bool LinkProgram(OGLShader * uiShaders, int iShaderCount, bool bDeleteShaders);

	void UseProgram();
	void DeleteProgram();

	void ExtractInformation();

	operator GLuint() const { return m_hProgram; }
private:
	GLuint m_hProgram;
	std::vector<ActiveProgramVertexAttributes> m_akAttributesInfo;
};


