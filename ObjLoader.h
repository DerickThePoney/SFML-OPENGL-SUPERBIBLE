#pragma once
#include "Mesh.h"

class ObjLoader
{
private:
	struct TriangleIndex
	{
		TriangleIndex() : v(0), vt(0), vn(0){}
		UI32 v;
		UI32 vt;
		UI32 vn;
	};

	struct TriangleData
	{
		TriangleIndex t1;
		TriangleIndex t2;
		TriangleIndex t3;
	};

public:
	static bool LoadFromObjFile(Mesh* pkMeshObject, const std::string& kFilename);

private:
	static void ParseTriangle(TriangleIndex& idx, std::string t);
	static void FillDataForTriangle(const TriangleIndex& tri, const UI32 index, Mesh* pkMesh, std::vector<vec3>& akNormals, std::vector<vec2>& akUVs);

	static bool FirstCharIsNumber(const std::string& t);
	static bool FirstCharIsChar(const std::string& t, char cmp);
	static void Advance(std::string& t);
};
