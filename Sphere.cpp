#include "stdafx.h"
#include "Sphere.h"

Sphere::Sphere(vec4 kCenter, F32 radius)
	: m_kCenter(kCenter), m_fRadius(radius)
{
}

const vec4 & Sphere::GetCenter() const
{
	return m_kCenter;
}

const  F32 & Sphere::GetRadius() const
{
	return m_fRadius;
}
