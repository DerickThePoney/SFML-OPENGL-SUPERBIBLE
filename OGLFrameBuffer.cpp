#include "stdafx.h"
#include "OGLFrameBuffer.h"


OGLFrameBuffer::OGLFrameBuffer()
{
}


OGLFrameBuffer::~OGLFrameBuffer()
{
}

void OGLFrameBuffer::Init()
{
	glGenFramebuffers(1, &m_hiFrameBuffer);
}

void OGLFrameBuffer::Delete()
{
	glDeleteFramebuffers(1, &m_hiFrameBuffer);
}

void OGLFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_hiFrameBuffer);
}

void OGLFrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
