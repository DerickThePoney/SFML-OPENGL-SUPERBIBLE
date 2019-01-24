#include "stdafx.h"
#include "WaveProfileBuffer.h"

constexpr int pos_modulo(int n, int d) { return (n % d + d) % d; }

WaveProfileBuffer::WaveProfileBuffer()
{
}


WaveProfileBuffer::~WaveProfileBuffer()
{
}

glm::vec4 WaveProfileBuffer::operator()(float p) const
{
	const int N = m_data.size();
	float x = N * p / m_period;

	int ix = (int)floor(x);
	auto wx = x - ix;
	glm::vec4 res;
	glm::vec4 v1 = m_data[pos_modulo(ix+1, N)];
	glm::vec4 v0 = m_data[pos_modulo(ix, N)];

	return glm::mix(v0, v1, wx);
}

float WaveProfileBuffer::dispersionRelation(float k) const
{
	constexpr float g = 9.81f;
	return sqrt(k * g);
}

glm::vec4 WaveProfileBuffer::gerstner_wave(float phase, float knum) const
{
	float s = sin(phase);
	float c = cos(phase);
	return glm::vec4{-s, c, -knum * c, -knum * s};
}

float WaveProfileBuffer::cubic_bump(float x) const
{
	if (abs(x) >= 1)
		return 0.0f;
	else
		return x * x * (2 * abs(x) - 3) + 1;
}
