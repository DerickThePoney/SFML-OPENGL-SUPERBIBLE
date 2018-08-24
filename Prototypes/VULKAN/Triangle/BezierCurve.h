#pragma once

//#include "vmath.h"
#include <vector>

#include <cassert>


template<typename T>
class CubicBezierCurve
{
public:
	CubicBezierCurve(const std::vector<T> akControlPoints)
		:m_akControlPoints(akControlPoints)
	{
		assert((m_akControlPoints.size() - 1) == 3);

		D = akControlPoints[0];
		C = 3 * akControlPoints[1] - akControlPoints[0];
		B = 3 * (akControlPoints[0] + akControlPoints[2] - 2 * akControlPoints[1]);
		A = akControlPoints[3] - 3 * akControlPoints[2] + 3 * akControlPoints[1] - akControlPoints[0];
	}

	const void EvaluatePosition(float t, T& rkPosition) const
	{
		float t1 = A * t + B;
		float t2 = t * t;
		float t3 = C * t + D;
		rkPosition = t1 * t2 + t3;
	}

protected:
	std::vector<T> m_akControlPoints;
	T A, B, C, D;
};

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
			for (size_t i = 0; i < kTemporaries.size() - 1; ++i)
			{
				kTemporaries[i] = glm::mix(kTemporaries[i], kTemporaries[i + 1], t);
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
class BezierCurve<glm::vec3>
{
public:
	BezierCurve(const std::vector<glm::vec3> akControlPoints)
		:m_akControlPoints(akControlPoints)
	{
		iDegree = m_akControlPoints.size() - 1;
		assert(iDegree > 0);
	}

	const void EvaluatePosition(float t, glm::vec3& rkPosition) const
	{
		std::vector<glm::vec3> kTemporaries(m_akControlPoints.begin(), m_akControlPoints.end()); // copy of control points;


		while (kTemporaries.size() > 1)
		{
			for (size_t i = 0; i < kTemporaries.size() - 1; ++i)
			{
				kTemporaries[i] = glm::mix(kTemporaries[i], kTemporaries[i + 1], t);
			}
			kTemporaries.pop_back();
		}

		//copy the result in the output
		rkPosition = kTemporaries[0];
	}


	const void EvaluatePositionAndSpace(float t, glm::vec3& rkPosition, glm::mat3& kPosition) const
	{
		this->EvaluatePosition(t, rkPosition);

		//make copy to get tengant;
		std::vector<glm::vec3> kTemporaries(m_akControlPoints.begin(), m_akControlPoints.end()); // copy of control points;

		while (kTemporaries.size() > 2)
		{
			for (size_t i = 0; i < kTemporaries.size() - 1; ++i)
			{
				kTemporaries[i] = glm::mix(kTemporaries[i], kTemporaries[i + 1], t);
			}
			kTemporaries.pop_back();
		}

		//copy the result in the output
		glm::vec3 tangant = glm::normalize(kTemporaries[1] - kTemporaries[0]);
		glm::vec3 normal = glm::vec3(0, 1, 0);
		glm::vec3 radial = glm::normalize(glm::cross(tangant, normal));
		normal = glm::normalize(glm::cross(radial, tangant));

		kPosition = glm::mat3(radial, normal, tangant);
	}

protected:
	std::vector<glm::vec3> m_akControlPoints;
	int iDegree;
};

typedef BezierCurve<glm::vec3> BezierCurve3D;