#pragma once
#include "Mesh.h"

class PLYLoader
{
private:
	struct VertexData
	{
		VertexData() : position(vec3(0)), normal(vec3(0)) , color(vec4(1)) {}
		vec3 position;
		vec3 normal;
		vec4 color;
	};

	struct TriangleData
	{
		UI32 t1;
		UI32 t2;
		UI32 t3;
	};

public:
	static bool LoadFromPLYFile(Mesh* pkMeshObject, const std::string& kFilename);

private:
	/*static void ParseTriangle(TriangleIndex& idx, std::string t);
	static void FillDataForTriangle(const TriangleIndex& tri, const UI32 index, Mesh* pkMesh, std::vector<vec3>& akNormals, std::vector<vec2>& akUVs);

	static bool FirstCharIsNumber(const std::string& t);
	static bool FirstCharIsChar(const std::string& t, char cmp);
	static void Advance(std::string& t);*/
};