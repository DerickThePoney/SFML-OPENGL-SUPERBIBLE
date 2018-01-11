#include "stdafx.h"
#include "OGLFrameBuffer.h"


OGLFrameBuffer::OGLFrameBuffer()
{
}


OGLFrameBuffer::~OGLFrameBuffer()
{
	Delete();
}

void OGLFrameBuffer::Init()
{
	glGenFramebuffers(1, &m_hiFrameBuffer);
}

void OGLFrameBuffer::Delete()
{
	glDeleteFramebuffers(1, &m_hiFrameBuffer);
}

void OGLFrameBuffer::Bind(GLenum eTarget)
{
	glBindFramebuffer(eTarget, m_hiFrameBuffer);
}

void OGLFrameBuffer::UnBind(GLenum eTarget)
{
	glBindFramebuffer(eTarget, 0);
}

void OGLFrameBuffer::AddTextureAttachement(GLenum eTarget, const OGLTexture2D & kTexture, GLenum eBuffer, int iLevel)
{
	glFramebufferTexture(eTarget, eBuffer, kTexture, iLevel);
}

void OGLFrameBuffer::AddRenderBufferAttachement(GLenum eTarget, OGLRenderBuffer & kRenderBuffer, GLenum eBuffer)
{
	glFramebufferRenderbuffer(eTarget, eBuffer,	GL_RENDERBUFFER, kRenderBuffer);
}
