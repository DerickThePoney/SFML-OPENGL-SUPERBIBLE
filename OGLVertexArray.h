#pragma once
class OGLVertexArray
{
public:
	OGLVertexArray();
	~OGLVertexArray();

	void Init();
	void Delete();
	void Bind();
	void UnBind();

	void SetAttribute(GLuint uiIndex, GLint iSize, GLenum eType,
		GLboolean bNormalized, GLsizei iStride,
		const void* pPointer, GLuint uiDivisor = 0);

	operator GLuint () { return m_hiVertexBuffer; }

private:
	GLuint m_hiVertexBuffer;
	bool m_bIsBound;
};

