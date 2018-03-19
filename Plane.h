#pragma once

class Plane
{
public:
	Plane(F32 distance = 0, vec4 normal = vec4(0, 1, 0, 0));

	const F32& GetDistance()const;
	const vec4& GetNormal()const;
	void GetUAndV(vec4& U, vec4& V);
private:
	F32 m_kDistance;
	vec4 m_kNormal;
};
