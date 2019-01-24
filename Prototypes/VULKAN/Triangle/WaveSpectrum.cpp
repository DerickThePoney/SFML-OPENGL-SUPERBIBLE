#include "stdafx.h"
#include "WaveSpectrum.h"

WaveSpectrum::WaveSpectrum(double windSpeed)
: m_windSpeed(windSpeed)
{
}

WaveSpectrum::~WaveSpectrum()
{
}

float WaveSpectrum::maxZeta() const
{
	return log2(10);
}

float WaveSpectrum::minZeta() const
{
	return log2(0.03);
}

float WaveSpectrum::operator()(double zeta) const
{
	double A = pow(1.1, 1.5 * zeta); // original pow(2, 1.5*zeta)
	double B = exp(-1.8038897788076411f * pow(4, zeta) / pow(m_windSpeed, 4));
	return 0.139098f * sqrt(A * B);;
}
