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
	friend class BMesh;
	friend class MeshManager;

protected:
	Mesh();
	~Mesh();

public:
	bool operator==(const Mesh& other) { return m_uiMeshID == other.m_uiMeshID; }


	bool LoadFromFile(const std::string& kFilename);
	void BindForDrawing();
	void LoadBuffersOnGraphicsCard();
	void SetAttributes();

	void Delete();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(cereal::make_nvp("IsProcedural", false),
			CEREAL_NVP(m_akVertices),
			CEREAL_NVP(m_akNormals),
			CEREAL_NVP(m_akColor),
			CEREAL_NVP(m_aiIndices),
			CEREAL_NVP(m_akUVs));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		//bool bIsProcedural = false;
		archive(cereal::make_nvp("IsProcedural", m_bIsProcedural));

		if (!m_bIsProcedural)
		{
			archive(CEREAL_NVP(m_akVertices),
				CEREAL_NVP(m_akNormals),
				CEREAL_NVP(m_akColor),
				CEREAL_NVP(m_aiIndices),
				CEREAL_NVP(m_akUVs));
		}
	}

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
	bool m_bIsProcedural;
};



