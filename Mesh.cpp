#include "stdafx.h"
#include "Mesh.h"

UI32 Mesh::s_uiMaxMeshID = 0;

Mesh::Mesh()
{
	m_uiMeshID = s_uiMaxMeshID++;
}


Mesh::~Mesh()
{
	Delete();
}

bool Mesh::LoadFromFile(const std::string & kFilename)
{
	return true;
}

void Mesh::BindForDrawing()
{
	m_kOGLBindings.m_hkVao.Bind();
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

	OGLBuffer justForTest((GLuint)m_kOGLBindings.m_kIndices);

	SetAttributes();
}

void Mesh::SetAttributes()
{

	//generate vertex array
	m_kOGLBindings.m_hkVao.Init();

	//set vertex array as attributes
	m_kOGLBindings.m_kVertices.Bind(GL_ARRAY_BUFFER);
	m_kOGLBindings.m_hkVao.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//set normals array as attributes
	m_kOGLBindings.m_kNormals.Bind(GL_ARRAY_BUFFER);
	m_kOGLBindings.m_hkVao.SetAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//set color array as attributes
	m_kOGLBindings.m_kColor.Bind(GL_ARRAY_BUFFER);
	m_kOGLBindings.m_hkVao.SetAttribute(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	//set UVs array as attributes
	m_kOGLBindings.m_kUVs.Bind(GL_ARRAY_BUFFER);
	m_kOGLBindings.m_hkVao.SetAttribute(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	m_kOGLBindings.m_hkVao.UnBind();
}

void Mesh::Delete()
{
	m_kOGLBindings.m_kVertices.Delete();
	m_kOGLBindings.m_kNormals.Delete();
	m_kOGLBindings.m_kColor.Delete();
	m_kOGLBindings.m_kUVs.Delete();
	m_kOGLBindings.m_kIndices.Delete();
	m_kOGLBindings.m_hkVao.Delete();
}
