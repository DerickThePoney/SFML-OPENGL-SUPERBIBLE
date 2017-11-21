#pragma once

class OGLShader
{
public:
	OGLShader();
	~OGLShader();

	bool InitFromFile(const std::string& filename, GLenum eType);
	bool InitFromFile(const char* filename, GLenum eType);
	bool InitFromMemory(const char* shaderCode, GLenum eType, const char* filename = nullptr);

	operator GLuint() const { return m_hShader; }

private:
	GLuint m_hShader;
	GLenum m_eType;
};

class OGLProgram
{
public:
	OGLProgram();
	~OGLProgram();

	bool LinkProgram(OGLShader * uiShaders, int iShaderCount, bool bDeleteShaders);

	void UseProgram();

	operator GLuint() const { return m_hProgram; }
private:
	GLuint m_hProgram;
};

