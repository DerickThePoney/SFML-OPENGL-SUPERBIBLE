#include "stdafx.h"
#include "Mesh.h"


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	m_kOGLBindings.m_kVertices.Delete();
	m_kOGLBindings.m_kNormals.Delete();
	m_kOGLBindings.m_kColor.Delete();
	m_kOGLBindings.m_kUVs.Delete();
	m_kOGLBindings.m_kIndices.Delete();
	glDeleteVertexArrays(1, &m_kOGLBindings.m_hiVao);
}

bool Mesh::LoadFromFile(const std::string & kFilename)
{
	return true;
}

void Mesh::BindForDrawing()
{
	glBindVertexArray(m_kOGLBindings.m_hiVao);
	m_kOGLBindings.m_kIndices.Bind(GL_ELEMENT_ARRAY_BUFFER);
	
}

void Mesh::LoadBuffersOnGraphicsCard()
{
	
	//generate vertex buffer
	m_kOGLBindings.m_kVertices.Init(GL_ARRAY_BUFFER, m_akVertices.size() * sizeof(vec3), m_akVertices.data(), 0);
	
	//generate normal buffer
	m_kOGLBindings.m_kNormals.Init(GL_ARRAY_BUFFER, m_akNormals.size() * sizeof(vec3), m_akNormals.data(), 0);

	//generate color buffer
	m_kOGLBindings.m_kColor.Init(GL_ARRAY_BUFFER, m_akColor.size() * sizeof(vec4), m_akColor.data(), 0);

	//generate UVS buffer
	m_kOGLBindings.m_kUVs.Init(GL_ARRAY_BUFFER, m_akUVs.size() * sizeof(vec2), m_akUVs.data(), 0);

	//generate indices buffer
	m_kOGLBindings.m_kIndices.Init(GL_ELEMENT_ARRAY_BUFFER, m_aiIndices.size() * sizeof(UI32), m_aiIndices.data(), 0);

	SetAttributes();
}

void Mesh::SetAttributes()
{

	//generate vertex array
	glGenVertexArrays(1, &m_kOGLBindings.m_hiVao);
	glBindVertexArray(m_kOGLBindings.m_hiVao);

	//set vertex array as attributes
	m_kOGLBindings.m_kVertices.Bind(GL_ARRAY_BUFFER);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//set normals array as attributes
	m_kOGLBindings.m_kNormals.Bind(GL_ARRAY_BUFFER);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//set color array as attributes
	m_kOGLBindings.m_kColor.Bind(GL_ARRAY_BUFFER);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	//set UVs array as attributes
	m_kOGLBindings.m_kUVs.Bind(GL_ARRAY_BUFFER);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);
}
