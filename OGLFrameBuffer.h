#pragma once

class OGLFrameBuffer
{
public:
	OGLFrameBuffer();
	~OGLFrameBuffer();

	void Init();
	void Delete();
	void Bind(GLenum eTarget);
	void UnBind(GLenum eTarget);

	void AddTextureAttachement(GLenum eTarget, const OGLTexture2D& kTexture, GLenum eBuffer, int iLevel);
	void AddRenderBufferAttachement(GLenum eTarget, OGLRenderBuffer& kRenderBuffer, GLenum eBuffer);

	operator GLuint () { return m_hiFrameBuffer; }
private:
	GLuint m_hiFrameBuffer;
};

