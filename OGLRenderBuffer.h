#pragma once
class OGLRenderBuffer
{
public:
	OGLRenderBuffer();
	~OGLRenderBuffer();

	void Init(GLenum eType, I32 width, I32 height);
	void Delete();
	void Bind();
	void UnBind();

	void SetStorage(GLenum eType, I32 width, I32 height);

	operator GLuint () { return m_hiRenderBuffer; }

private:
	GLuint m_hiRenderBuffer;
};