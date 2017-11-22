#include "stdafx.h"
#include "Mesh.h"


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_kOGLBindings.m_hVertices);
	glDeleteBuffers(1, &m_kOGLBindings.m_hNormals);
	glDeleteBuffers(1, &m_kOGLBindings.m_hColor);
	glDeleteBuffers(1, &m_kOGLBindings.m_hUVs);
	glDeleteBuffers(1, &m_kOGLBindings.m_hIndices);
	glDeleteVertexArrays(1, &m_kOGLBindings.m_hiVao);
}

bool Mesh::LoadFromFile(const std::string & kFilename)
{
	return true;
}

void Mesh::BindForDrawing()
{
	glBindVertexArray(m_kOGLBindings.m_hiVao);
	SetAttributes();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_kOGLBindings.m_hIndices);
	
}

void Mesh::LoadOnGraphicsCard()
{
	//generate vertex array
	glGenVertexArrays(1, &m_kOGLBindings.m_hiVao);
	glBindVertexArray(m_kOGLBindings.m_hiVao);

	//generate vertex buffer
	glGenBuffers(1, &m_kOGLBindings.m_hVertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hVertices);
	glBufferData(GL_ARRAY_BUFFER, m_akVertices.size() * sizeof(vec3), m_akVertices.data(), GL_STATIC_DRAW);

	//generate normal buffer
	glGenBuffers(1, &m_kOGLBindings.m_hNormals);
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hNormals);
	glBufferData(GL_ARRAY_BUFFER, m_akNormals.size() * sizeof(vec3), m_akNormals.data(), GL_STATIC_DRAW);

	//generate color buffer
	glGenBuffers(1, &m_kOGLBindings.m_hColor);
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hColor);
	glBufferData(GL_ARRAY_BUFFER, m_akColor.size() * sizeof(vec4), m_akColor.data(), GL_STATIC_DRAW);

	//generate UVS buffer
	glGenBuffers(1, &m_kOGLBindings.m_hUVs);
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hUVs);
	glBufferData(GL_ARRAY_BUFFER, m_akUVs.size() * sizeof(vec2), m_akUVs.data(), GL_STATIC_DRAW);

	//generate indices buffer
	glGenBuffers(1, &m_kOGLBindings.m_hIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_kOGLBindings.m_hIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_aiIndices.size() * sizeof(UI32), m_aiIndices.data(), GL_STATIC_DRAW);
}

void Mesh::SetAttributes()
{
	//set vertex array as attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//set normals array as attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hNormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//set color array as attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hColor);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	//set UVs array as attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_kOGLBindings.m_hUVs);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);
}
