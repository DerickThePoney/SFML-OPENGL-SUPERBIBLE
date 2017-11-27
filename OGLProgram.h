#pragma once
#include "OGLUtilities.h"
#include "OGLShader.h"

struct ActiveProgramInformations
{
	ActiveProgramInformations(): m_pcName(nullptr) {}
	GLsizei m_iLength;
	GLint m_iSize;
	GLenum m_eType;
	GLchar* m_pcName;
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
	std::vector<ActiveProgramInformations> m_akAttributesInfo;
	std::vector<ActiveProgramInformations> m_akUniformsInfo;
};


