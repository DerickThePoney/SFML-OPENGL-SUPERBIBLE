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
	//1 - get the necessary size
	I32 iSize = m_akVertices.size() * sizeof(vec4) +
		m_akNormals.size() * sizeof(vec3) +
		m_akColor.size() * sizeof(vec4) +
		m_akUVs.size() * sizeof(vec2);

	m_kOGLBindings.m_kBuffer.Init(GL_ARRAY_BUFFER, iSize, NULL, GL_DYNAMIC_STORAGE_BIT);
	I32 iOffset = 0;
	if (m_akVertices.size() > 0)
	{
		//generate vertex buffer
		m_kOGLBindings.m_kBuffer.UpdateSubData(GL_ARRAY_BUFFER, iOffset, m_akVertices.size() * sizeof(vec4), m_akVertices.data());
		iOffset += m_akVertices.size() * sizeof(vec4);
	}

	if (m_akNormals.size() > 0)
	{
		//generate normal buffer
		m_kOGLBindings.m_kBuffer.UpdateSubData(GL_ARRAY_BUFFER, iOffset, m_akNormals.size() * sizeof(vec3), m_akNormals.data());
		iOffset += m_akNormals.size() * sizeof(vec3);
	}

	if (m_akColor.size() > 0)
	{
		//generate color buffer
		m_kOGLBindings.m_kBuffer.UpdateSubData(GL_ARRAY_BUFFER, iOffset, m_akColor.size() * sizeof(vec4), m_akColor.data());
		iOffset += m_akColor.size() * sizeof(vec4);
	}

	if (m_akUVs.size() > 0)
	{
		//generate UVS buffer
		m_kOGLBindings.m_kBuffer.UpdateSubData(GL_ARRAY_BUFFER, iOffset, m_akUVs.size() * sizeof(vec2), m_akUVs.data());
		iOffset += m_akUVs.size() * sizeof(vec2);
	}

	if (m_aiIndices.size() > 0)
	{
		//generate indices buffer
		m_kOGLBindings.m_kIndices.Init(GL_ELEMENT_ARRAY_BUFFER, m_aiIndices.size() * sizeof(UI32), m_aiIndices.data(), 0);
	}

	SetAttributes();

	//ClearLocalData();
}

void Mesh::SetAttributes()
{

	//generate vertex array
	m_kOGLBindings.m_hkVao.Init();

	m_kOGLBindings.m_kBuffer.Bind(GL_ARRAY_BUFFER);
	I32 iOffset = 0;

	//set vertex array as attributes	
	m_kOGLBindings.m_hkVao.SetAttribute(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)iOffset);
	//glVertexAttribBinding(0, 0);
	iOffset += m_akVertices.size() * sizeof(vec4);

	//set normals array as attributes
	m_kOGLBindings.m_hkVao.SetAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)iOffset);
	//glVertexAttribBinding(1, 0);
	iOffset += m_akNormals.size() * sizeof(vec3);

	//set color array as attributes
	m_kOGLBindings.m_hkVao.SetAttribute(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)iOffset);
	//glVertexAttribBinding(2, 0);
	iOffset += m_akColor.size() * sizeof(vec4);

	//set UVs array as attributes
	m_kOGLBindings.m_hkVao.SetAttribute(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)iOffset);
	//glVertexAttribBinding(3, 0);

	m_kOGLBindings.m_hkVao.UnBind();
}

void Mesh::ClearLocalData()
{
	m_akVertices.clear();
	m_akNormals.clear();
	m_akColor.clear();
	m_akUVs.clear();
	m_aiIndices.clear();
}

Sphere Mesh::GetBoundingSphere()
{
	vec4 center(0);
	F32 radius = 0;

	for (std::size_t i = 0; i < m_akVertices.size(); ++i)
	{
		center += m_akVertices[i];
	}
	center /= center[3];

	for (std::size_t i = 0; i < m_akVertices.size(); ++i)
	{
		F32 dist = vmath::length(m_akVertices[i] - center);
		if (dist > radius)
		{
			radius = dist;
		}
	}

	return Sphere(center, radius);
}

void Mesh::Delete()
{
	m_kOGLBindings.m_kBuffer.Delete();
	m_kOGLBindings.m_kIndices.Delete();
	m_kOGLBindings.m_hkVao.Delete();
}
