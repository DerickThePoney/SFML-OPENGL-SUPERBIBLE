#pragma once

/*
 * The class ProfileBuffer is representation of the integral (21) from the
 * paper.
 *
 * It provides two functionalities:
 * 1. Function `precompute` precomputes values of the integral for main input
 * valus of `p` (see paper for the meaning of p)
 * 2. Call operator evaluates returns the profile value at a given point.
 * This is done by interpolating from precomputed values.
 *
 * TODO: Add note about making (21) pariodic over precomputed interval.
 */
class WaveProfileBuffer
{
public:
	WaveProfileBuffer();
	~WaveProfileBuffer();

	template <typename Spectrum>
	void precompute
	(
		Spectrum &spectrum, 
		float time, 
		float zeta_min,
		float zeta_max, 
		int resolution = 4096,
		int periodicity = 2,
		int integration_nodes = 100
	)
	{
		m_data.resize(resolution);
		m_period = periodicity * powf(2, zeta_max);

		for (int i = 0; i < resolution; i++) {

			constexpr float tau = 6.28318530718f;
			float           p = (i * m_period) / resolution;

			m_data[i] =
				integrate(integration_nodes, zeta_min, zeta_max, [&](float zeta) {

				float waveLength = powf(2, zeta);
				float waveNumber = tau / waveLength;
				float phase1 =
					waveNumber * p - dispersionRelation(waveNumber) * time;
				float phase2 = waveNumber * (p - m_period) -
					dispersionRelation(waveNumber) * time;

				float weight1 = p / m_period;
				float weight2 = 1 - weight1;
				return waveLength * spectrum(zeta) *
					(cubic_bump(weight1) * gerstner_wave(phase1, waveNumber) +
						cubic_bump(weight2) * gerstner_wave(phase2, waveNumber));
			});
		}
	}



	/**
	 * Evaluate profile at point p by doing linear interpolation over precomputed
	 * data
	 * @param p evaluation position, it is usually p=dot(position, wavedirection)
	 */
	glm::vec4 operator()(float p) const;

private:
	/**
	 * Dispersion relation in infinite depth -
	 * https://en.wikipedia.org/wiki/Dispersion_(water_waves)
	 */
	float dispersionRelation(float k) const;

	/**
	 * Gerstner wave - https://en.wikipedia.org/wiki/Trochoidal_wave
	 *
	 * @return Array of the following values:
		1. horizontal position offset
		2. vertical position offset
		3. position derivative of horizontal offset
		4. position derivative of vertical offset
	 */
	glm::vec4 gerstner_wave(float phase /*=knum*x*/, float knum) const;

	/** bubic_bump is based on $p_0$ function from
	 * https://en.wikipedia.org/wiki/Cubic_Hermite_spline
	 */
	float cubic_bump(float x) const;

public:
	float m_period;

	std::vector<glm::vec4> m_data;

};

template <typename Fun>
auto integrate(int integration_nodes, float x_min, float x_max, Fun const &fun) {

	float dx = (x_max - x_min) / integration_nodes;
	float x = x_min + 0.5f * dx;

	auto result = dx * fun(x); // the first integration node
	for (int i = 1; i < integration_nodes; i++) { // proceed with other nodes, notice `int i= 1`
		x += dx;
		result += dx * fun(x);
	}

	return result;
}

