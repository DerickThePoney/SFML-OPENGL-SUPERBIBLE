#pragma once
#include "vmath.h"
#include "Utility.h"

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
	void LoadOnGraphicsCard();
	void SetAttributes();

	std::vector<vec3> m_akVertices;
	std::vector<vec3> m_akNormals;
	std::vector<vec4> m_akColor;
	std::vector<UI32> m_aiIndices;
	std::vector<vec2> m_akUVs;

	struct OGLMeshBuffers
	{
		GLuint m_hiVao;
		GLuint m_hVertices;
		GLuint m_hNormals;
		GLuint m_hColor;
		GLuint m_hIndices;
		GLuint m_hUVs;
	};

	OGLMeshBuffers m_kOGLBindings;

	static void MakeSphere(Mesh& rkDest, I32 iPhiSubDiv, I32 iThetaSubdiv, F64 fRadius);
};



