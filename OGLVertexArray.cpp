#include "stdafx.h"
#include "OGLVertexArray.h"


OGLVertexArray::OGLVertexArray()
	: m_bIsBound(false)
{
}


OGLVertexArray::~OGLVertexArray()
{
}

void OGLVertexArray::Delete()
{
	m_bIsBound = false;
	glDeleteVertexArrays(1, &m_hiVertexBuffer);
}

void OGLVertexArray::Bind()
{
	glBindVertexArray(m_hiVertexBuffer);
	m_bIsBound = true;
}

void OGLVertexArray::UnBind()
{
	m_bIsBound = false;
}

void OGLVertexArray::SetAttribute(GLuint uiIndex, GLint iSize, GLenum eType, GLboolean bNormalized, 
									GLsizei iStride, const void * pPointer, GLuint uiDivisor)
{
	if (!m_bIsBound)
	{
		Bind();
	}

	glVertexAttribPointer(uiIndex, iSize, eType, bNormalized, iStride, pPointer);
	glEnableVertexAttribArray(uiIndex);
	glVertexAttribDivisor(uiIndex, uiDivisor);
}

void OGLVertexArray::Init()
{
	glGenVertexArrays(1, &m_hiVertexBuffer);
}
