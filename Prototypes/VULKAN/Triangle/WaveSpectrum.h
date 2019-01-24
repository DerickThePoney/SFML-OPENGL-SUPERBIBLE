#pragma once

class WaveSpectrum
{
public:
	WaveSpectrum(double windSpeed);
	~WaveSpectrum();

	/*
	 * Maximal reasonable value of zeta to consider
	 */
	float maxZeta() const;

	/*
	 * Minamal resonable value of zeta to consider
	 */
	float minZeta() const;

	/*
	 * Returns density of wave for given zeta(=log2(wavelength))
	 */
	float operator()(double zeta) const;

public:
	double m_windSpeed = 1;
};

