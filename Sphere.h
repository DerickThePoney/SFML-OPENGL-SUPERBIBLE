#pragma once


class Sphere
{
public:
	Sphere(vec4 kCenter = vec4(0, 0, 0, 1), F32 radius = 1);

	const vec4& GetCenter() const;
	const F32& GetRadius() const;
private:
	vec4 m_kCenter;
	F32 m_fRadius;
};