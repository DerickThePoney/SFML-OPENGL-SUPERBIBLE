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

void OGLFrameBuffer::BindForDrawing(GLenum eTarget)
{
	Bind(eTarget);
	glViewport(0, 0, m_iWidth, m_iHeight);
}

void OGLFrameBuffer::UnBind(GLenum eTarget)
{
	glBindFramebuffer(eTarget, 0);
}

void OGLFrameBuffer::AddTextureAttachement(GLenum eTarget, const OGLTexture2D & kTexture, GLenum eAttachement, int iLevel)
{
	glFramebufferTexture(eTarget, eAttachement, kTexture, iLevel);
	m_iWidth = kTexture.GetTextureSize()[0];
	m_iHeight = kTexture.GetTextureSize()[1];
}

void OGLFrameBuffer::AddRenderBufferAttachement(GLenum eTarget, OGLRenderBuffer & kRenderBuffer, GLenum eAttachement)
{
	glFramebufferRenderbuffer(eTarget, eAttachement, GL_RENDERBUFFER, kRenderBuffer);
}
