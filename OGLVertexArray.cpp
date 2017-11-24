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

void OGLVertexArray::SetAttribute(GLuint m_uiIndex, GLint m_iSize, GLenum m_eType, GLboolean m_bNormalized, GLsizei m_iStride, const void * m_pPointer)
{
	if (!m_bIsBound)
	{
		Bind();
	}

	glVertexAttribPointer(m_uiIndex, m_iSize, m_eType, m_bNormalized, m_iStride, m_pPointer);
	glEnableVertexAttribArray(m_uiIndex);
}

void OGLVertexArray::Init()
{
	glGenVertexArrays(1, &m_hiVertexBuffer);
}
