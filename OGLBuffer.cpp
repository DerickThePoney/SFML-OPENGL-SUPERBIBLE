#include "stdafx.h"
#include "OGLBuffer.h"

OGLBuffer::OGLBuffer(GLuint hiBuffer)
{
	m_hiBuffer = hiBuffer;
	InitFromHandle();
}

void OGLBuffer::Init(GLenum eTarget, I32 iSize, void * pData, GLbitfield eFlags)
{
	//generate the buffer
	glGenBuffers(1, &m_hiBuffer);


#if OGL_4_5
	//Init the storage no binding
	glNamedBufferStorage(m_hiBuffer, iSize, pData, eFlags);
#else
	//bind the buffer
	glBindBuffer(eTarget, m_hiBuffer);
	//Init the storage
	glBufferStorage(eTarget, iSize, pData, eFlags);
#endif
	//save these data for later;
	m_iSize = iSize;
	m_eFlags = eFlags;
}

void OGLBuffer::UpdateData(GLenum eTarget, I32 iSize, void * pData, GLbitfield eFlags)
{
	if (iSize != m_iSize)
	{
		//if size is different, just clear the buffer, and reinit it
		Delete();
		Init(eTarget, iSize, pData, eFlags);
		return;
	}
	
#if OGL_4_5
	// use the named version for these buffers
	void* ptr = glMapNamedBuffer(m_hiBuffer, eFlags);
	memcpy(ptr, pData, m_iSize);
	glUnmapNamedBuffer(m_hiBuffer);
#else
	// use the standard binded version for these buffers
	glBindBuffer(eTarget, m_hiBuffer);
	void* ptr = glMapBuffer(eTarget, eFlags);
	memcpy(ptr, pData, m_iSize);
	glUnmapBuffer(eTarget);
#endif
}

void OGLBuffer::Delete()
{
	glDeleteBuffers(1, &m_hiBuffer);
}

void OGLBuffer::Bind(GLenum eTarget)
{
	glBindBuffer(eTarget, m_hiBuffer);
}

void OGLBuffer::InitFromHandle()
{
#if OGL_4_5
	glGetBufferParameteriv(m_hiBuffer, GL_BUFFER_SIZE, (GLint*)&m_iSize);
	glGetBufferParameteriv(m_hiBuffer, GL_BUFFER_STORAGE_FLAGS, (GLint*)&m_eFlags);
#else
	glBindBuffer(GL_ARRAY_BUFFER, m_hiBuffer);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, (GLint*)&m_iSize);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_STORAGE_FLAGS, (GLint*)&m_eFlags);
#endif
}
