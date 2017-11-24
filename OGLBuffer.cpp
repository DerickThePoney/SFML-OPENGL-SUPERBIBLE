#include "stdafx.h"
#include "OGLBuffer.h"

void OGLBuffer::Init(GLenum eTarget, I32 iSize, void * pData, GLbitfield eFlags)
{
	//generate the buffer
	glGenBuffers(1, &m_hiBuffer);

	//bind the buffer
	glBindBuffer(eTarget, m_hiBuffer);

	//Init the storage
	glBufferStorage(eTarget, iSize, pData, eFlags);

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

	glBindBuffer(eTarget, m_hiBuffer);
	void* ptr = glMapBuffer(eTarget, eFlags);
	memcpy(ptr, pData, m_iSize);
	glUnmapBuffer(eTarget);
}

void OGLBuffer::Delete()
{
	glDeleteBuffers(1, &m_hiBuffer);
}

void OGLBuffer::Bind(GLenum eTarget)
{
	glBindBuffer(eTarget, m_hiBuffer);
}
