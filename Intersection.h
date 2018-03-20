#pragma once

//#include "vmath.h"
#include "Utility.h"
#include "GeometricPrimitives.h"

struct IntersectionContact
{
	vec4 m_kPosition;
	vec4 m_kContactNormal;
};

class Intersect
{
public:
	static bool Find(const Line& rkLine, const vmath::vec4 pkTriangle[3], I32& iQuantity, F32 fTValue[2]);
	static bool Find(const Line& rkLine, const vmath::vec4* apkVertices, const UI32& uiVertices, const UI32* apkTriangles, const UI32& uiTriangles, I32& iQuantity, F32 fTValue[2], std::vector<F32>& apkHit);

	static bool Find(const Sphere& rkSphere, const Plane& rkPlane, IntersectionContact& rkContact, const bool halfSpace = true);
};
