#pragma once

//#include "vmath.h"
#include "Utility.h"
#include "Line.h"

class Intersect
{
public:
	static bool Find(const Line& rkLine, const vmath::vec3 pkTriangle[3], I32& iQuantity, F32 fTValue[2]);
	static bool Find(const Line& rkLine, const vmath::vec3* apkVertices, const UI32& uiVertices, const UI32* apkTriangles, const UI32& uiTriangles, I32& iQuantity, F32 fTValue[2], std::vector<F32>& apkHit);
};
