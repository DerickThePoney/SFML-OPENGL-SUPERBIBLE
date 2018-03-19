#include "stdafx.h"
#include "Plane.h"

Plane::Plane(F32 distance, vec4 normal)
	: m_kDistance(distance), m_kNormal(normal)
{
}

const F32 & Plane::GetDistance() const
{
	return m_kDistance;
}

const vec4 & Plane::GetNormal() const
{
	return m_kNormal;
}

void Plane::GetUAndV(vec4 & U, vec4 & V)
{
	F32 invLength;

	if (fabs(m_kNormal[0]) > fabs(m_kNormal[1]))
	{
		invLength = 1 / sqrtf(m_kNormal[0] * m_kNormal[0] + m_kNormal[2] * m_kNormal[2]);
		U[0] = -m_kNormal[2] * invLength;
		U[1] = 0;
		U[2] = m_kNormal[0] * invLength;
		U[3] = 0;

		V[0] = m_kNormal[1] * U[2];
		V[1] = m_kNormal[2] * U[0] - m_kNormal[0] * U[2];
		V[2] = -m_kNormal[1] * U[0];
		V[3] = 0;
	}
	else
	{
		invLength = 1 / sqrtf(m_kNormal[1] * m_kNormal[1] + m_kNormal[2] * m_kNormal[2]);
		U[0] = 0;
		U[1] = m_kNormal[2] * invLength;
		U[2] = -m_kNormal[1] * invLength;
		U[3] = 0;

		V[0] = m_kNormal[1] * U[2] - m_kNormal[2] * U[1];
		V[1] = -m_kNormal[0] * U[2];
		V[2] = m_kNormal[0] * U[1];
		V[3] = 0;
	}
}
