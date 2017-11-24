#pragma once
#include "vmath.h"
#include "Utility.h"

#include "OGLRendering.h"

using namespace vmath;

enum Attributes
{
	VERTEX,
	NORMALS,
	COLOR,
	UVS
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool LoadFromFile(const std::string& kFilename);
	void BindForDrawing();
	void LoadBuffersOnGraphicsCard();
	void SetAttributes();

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

	static void MakeSphere(Mesh& rkDest, I32 iPhiSubDiv, I32 iThetaSubdiv, F64 fRadius);
};



