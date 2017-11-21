#include "stdafx.h"
#include "Intersection.h"

bool Intersect::Find(const Line & rkLine, const vmath::vec3 pkTriangle[3], I32 & iQuantity, F32 fTValue[2])
{
	const F32 fEpsilon = 1e-6f;
	vmath::vec3 kE0 = pkTriangle[1] - pkTriangle[0];
	vmath::vec3 kE1 = pkTriangle[2] - pkTriangle[0];
	vmath::vec3 kNormal = vmath::cross(vmath::normalize(kE0), vmath::normalize(kE1));
	vmath::vec3 kDiff = rkLine.m_kOrigin - pkTriangle[0];

	//compute the dot product of line and normal
	F32 fDND = vmath::dot(kNormal, vmath::normalize(rkLine.m_kDirection));
	F32 fSign;
	if (fDND < -fEpsilon)
	{
		fSign = -1.0f;
		fDND = -fDND;
	}
	else if (fDND > fEpsilon)
	{
		fSign = 1.0f;
	}
	else
	{
		return false;
	}

	
	//we are in buisiness -> line and triangle are not coplanar
	//find the point of intersection and determine wether it is in the triangle
	F32 fDdQxE2 = fSign * vmath::dot(rkLine.m_kDirection, vmath::cross(kDiff, kE1));
	if (fDdQxE2 >= 0.0f)
	{
		F32 fDdE1xQ = fSign*vmath::dot(rkLine.m_kDirection, vmath::cross(kDiff, kE0));
		if (fDdE1xQ >= 0.0f)
		{
			if (fDdQxE2 + fDdE1xQ <= fDND)
			{
				// line intersects triangle
				F32 fQdN = -fSign*vmath::dot(kDiff, kNormal);
				F32 fInv = (1.0f) / fDND;
					
				F32 fTriB1 = fDdQxE2*fInv;
				F32 fTriB2 = fDdE1xQ*fInv;
				F32 fTriB0 = 1.0f - fTriB1 - fTriB2;

				if (fTriB1 >= 0 && fTriB2 >= 0 && fTriB0 >= 0)
				{
					fTValue[0] = fQdN*fInv;
					iQuantity = 1;
					return true;
				}
			}
			// else: b1+b2 > 1, no intersection
		}
		// else: b2 < 0, no intersection
	}
	

	return false;
}

bool Intersect::Find(const Line & rkLine, const vmath::vec3 * apkVertices, const UI32 & uiVertices, 
					 const UI32 * apkTriangles, const UI32 & uiTriangles, I32 & iQuantity, F32 fTValue[2], std::vector<F32>& apkHit)
{
	vmath::vec3 pkTriangle[3];
	F32 fLocalTValue[2];

	for(UI32 i = 0; i < uiTriangles; ++i)
	{ 
		pkTriangle[0] = apkVertices[apkTriangles[i * 3]];
		pkTriangle[1] = apkVertices[apkTriangles[i * 3 + 1]];
		pkTriangle[2] = apkVertices[apkTriangles[i * 3 + 2]];

		if (Find(rkLine, pkTriangle, iQuantity, fLocalTValue))
		{
			std::cout << "Intersect" << std::endl;
			vmath::vec3 pkIntersectPoint = rkLine.m_kOrigin + fLocalTValue[0] * rkLine.m_kDirection;
			std::cout << pkIntersectPoint[0] << "\t" << pkIntersectPoint[1] << "\t" << pkIntersectPoint[2] << "\t" << std::endl;
			apkHit[apkTriangles[i * 3]] = 1.0f;
			apkHit[apkTriangles[i * 3 + 1]] = 1.0f;
			apkHit[apkTriangles[i * 3 + 2]] = 1.0f;
		}
	}

	return false;
}
