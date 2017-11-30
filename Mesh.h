#pragma once
#include "vmath.h"
#include "Utility.h"

#include "OGLRendering.h"
#include "MeshManager.h"

using namespace vmath;

enum Attributes
{
	VERTEX,
	NORMALS,
	COLOR,
	UVS
};

class MeshManager;

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool operator==(const Mesh& other) { return m_uiMeshID == other.m_uiMeshID; }


	bool LoadFromFile(const std::string& kFilename);
	void BindForDrawing();
	void LoadBuffersOnGraphicsCard();
	void SetAttributes();

	void Delete();

	void RegisterMesh();

	std::vector<vec3> m_akVertices;
	std::vector<vec3> m_akNormals;
	std::vector<vec4> m_akColor;
	std::vector<UI32> m_aiIndices;
	std::vector<vec2> m_akUVs;

	struct OGLMeshBuffers
	{
		OGLVertexArray m_hkVao;
		OGLBuffer m_kVertices;
		OGLBuffer m_kNormals;
		OGLBuffer m_kColor;
		OGLBuffer m_kIndices;
		OGLBuffer m_kUVs;
	};

	OGLMeshBuffers m_kOGLBindings;

	//ID stuffs
	static UI32 s_uiMaxMeshID;
	UI32 m_uiMeshID;
};



