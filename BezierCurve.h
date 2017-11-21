#pragma once

#include "vmath.h"
#include "Utility.h"
#include <vector>

#include <cassert>

template<typename T>
class BezierCurve
{
public:
	BezierCurve(const std::vector<T> akControlPoints)
		:m_akControlPoints(akControlPoints)
	{
		iDegree = m_akControlPoints.size() - 1;
		assert(iDegree > 0);
	}

	const void EvaluatePosition(float t, T& rkPosition) const 
	{
		std::vector<T> kTemporaries(m_akControlPoints.begin(), m_akControlPoints.end()); // copy of control points;


		while (kTemporaries.size() > 1)
		{
			for (UI32 i = 0; i < kTemporaries.size() - 1; ++i)
			{
				kTemporaries[i] = Lerp(kTemporaries[i], kTemporaries[i + 1], t);
			}
			kTemporaries.pop_back();
		}

		//copy the result in the output
		rkPosition = kTemporaries[0];
	}

protected:
	std::vector<T> m_akControlPoints;
	int iDegree;
};


template<>
class BezierCurve<vmath::vec3>
{
public:
	BezierCurve(const std::vector<vmath::vec3> akControlPoints)
		:m_akControlPoints(akControlPoints)
	{
		iDegree = m_akControlPoints.size() - 1;
		assert(iDegree > 0);
	}

	const void EvaluatePosition(float t, vmath::vec3& rkPosition) const
	{
		std::vector<vmath::vec3> kTemporaries(m_akControlPoints.begin(), m_akControlPoints.end()); // copy of control points;


		while (kTemporaries.size() > 1)
		{
			for (UI32 i = 0; i < kTemporaries.size() - 1; ++i)
			{
				kTemporaries[i] = Lerp(kTemporaries[i], kTemporaries[i + 1], t);
			}
			kTemporaries.pop_back();
		}

		//copy the result in the output
		rkPosition = kTemporaries[0];
	}


	const void EvaluatePositionAndSpace(float t, vmath::vec3& rkPosition, vmath::mat3& kPosition) const
	{
		this->EvaluatePosition(t, rkPosition);

		//make copy to get tengant;
		std::vector<vmath::vec3> kTemporaries(m_akControlPoints.begin(), m_akControlPoints.end()); // copy of control points;

		while (kTemporaries.size() > 2)
		{
			for (UI32 i = 0; i < kTemporaries.size() - 1; ++i)
			{
				kTemporaries[i] = Lerp(kTemporaries[i], kTemporaries[i + 1], t);
			}
			kTemporaries.pop_back();
		}

		//copy the result in the output
		vmath::vec3 tangant = vmath::normalize(kTemporaries[1] - kTemporaries[0]);
		vmath::vec3 normal = vmath::vec3(0, 1, 0);
		vmath::vec3 radial = vmath::normalize(vmath::cross(tangant, normal));
		normal = vmath::normalize(vmath::cross(radial, tangant));

		kPosition = vmath::mat3(radial, normal, tangant);
	}

protected:
	std::vector<vmath::vec3> m_akControlPoints;
	int iDegree;
};

typedef BezierCurve<vmath::vec3> BezierCurve3D;