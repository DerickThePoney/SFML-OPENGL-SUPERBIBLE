#pragma once
#include "BezierCurve.h"

template<typename T>
class BezierSpline
{
public:
	BezierSpline(const std::vector<float>& rafStarts, const std::vector<float>& rafEnds, const std::vector<BezierCurve<T>> &rakCurves)
		: m_afStarts(rafStarts), m_afEnds(rafEnds), m_akCurves(rakCurves)
	{
		assert(m_akCurves.size() == m_afEnds.size());
		assert(m_akCurves.size() == m_afStarts.size());
	}

	const void EvaluatePosition(float t, T& rkPosition) const
	{
		UI32 i = 0;
		while (t > m_afEnds[i])
		{
			++i;
		}

		assert(i < m_akCurves.size());

		m_akCurves[i].EvaluatePosition((t - m_afStarts[i]) / (m_afEnds[i] - m_afStarts[i]), rkPosition);
	}

private:
	std::vector<BezierCurve<T>> m_akCurves;
	std::vector<float> m_afStarts;
	std::vector<float> m_afEnds;

};
