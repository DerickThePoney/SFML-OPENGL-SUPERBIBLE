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

	void SetAttribute(GLuint m_uiIndex, GLint m_iSize, GLenum m_eType, 
						GLboolean m_bNormalized, GLsizei m_iStride, 
						const void* m_pPointer);

	operator GLuint () { return m_hiVertexBuffer; }

private:
	GLuint m_hiVertexBuffer;
	bool m_bIsBound;
};

