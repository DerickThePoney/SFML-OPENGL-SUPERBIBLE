#include "stdafx.h"
#include "OGLRenderBuffer.h"

OGLRenderBuffer::OGLRenderBuffer()
{
}

OGLRenderBuffer::~OGLRenderBuffer()
{
	Delete();
}

void OGLRenderBuffer::Init(GLenum eType, I32 width, I32 height)
{
	glGenRenderbuffers(1, &m_hiRenderBuffer);
	Bind();
	SetStorage(eType, width, height);
}

void OGLRenderBuffer::Delete()
{
	glDeleteRenderbuffers(1, &m_hiRenderBuffer);
}

void OGLRenderBuffer::Bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_hiRenderBuffer);
}

void OGLRenderBuffer::UnBind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void OGLRenderBuffer::SetStorage(GLenum eType, I32 width, I32 height)
{
	glRenderbufferStorage(GL_RENDERBUFFER, eType, width, height);
}
