#pragma once

#include <random>

class ValueNoise1D
{
public:
	ValueNoise1D(unsigned seed = 2016)
	{
		std::mt19937 gen(seed);
		std::uniform_real_distribution<float> distrFloat;
		auto randFloat = std::bind(distrFloat, gen);
		for (unsigned i = 0; i < kMaxVertices; ++i) {
			r[i] = randFloat();
		}
	}

	// Evaluate the noise function at position x
	float eval(const float &x)
	{
		// Floor
		int xi = (int)x - (x < 0 && x != (int)x);
		float t = x - xi;
		// Modulo using &
		int xMin = xi & kMaxVerticesMask;
		int xMax = (xMin + 1) & kMaxVerticesMask;
		t = glm::smoothstep(0.0f, 1.0f, t);
		return glm::mix(r[xMin], r[xMax], t);
	}
	static const unsigned kMaxVertices = 256;
	static const unsigned kMaxVerticesMask = kMaxVertices - 1;
	float r[kMaxVertices];
};

class ValueNoise2D
{
public:
	ValueNoise2D(unsigned seed = 2016)
	{
		std::mt19937 gen(seed);
		std::uniform_real_distribution<float> distrFloat;
		auto randFloat = std::bind(distrFloat, gen);

		// create an array of random values and initialize permutation table
		for (unsigned k = 0; k < kMaxTableSize; ++k) {
			r[k] = randFloat();
			permutationTable[k] = k;
		}

		// shuffle values of the permutation table
		std::uniform_int_distribution<int> distrUInt;
		auto randUInt = std::bind(distrUInt, gen);
		for (unsigned k = 0; k < kMaxTableSize; ++k) {
			unsigned i = randUInt() & kMaxTableSizeMask;
			std::swap(permutationTable[k], permutationTable[i]);
			permutationTable[k + kMaxTableSize] = permutationTable[k];
		}
	}

	float eval(glm::vec2 &p) const
	{
		int xi = (int)std::floor(p.x);
		int yi = (int)std::floor(p.y);

		float tx = p.x - xi;
		float ty = p.y - yi;

		int rx0 = xi & kMaxTableSizeMask;
		int rx1 = (rx0 + 1) & kMaxTableSizeMask;
		int ry0 = yi & kMaxTableSizeMask;
		int ry1 = (ry0 + 1) & kMaxTableSizeMask;

		// random values at the corners of the cell using permutation table
		const float & c00 = r[permutationTable[permutationTable[rx0] + ry0]];
		const float & c10 = r[permutationTable[permutationTable[rx1] + ry0]];
		const float & c01 = r[permutationTable[permutationTable[rx0] + ry1]];
		const float & c11 = r[permutationTable[permutationTable[rx1] + ry1]];

		// remapping of tx and ty using the Smoothstep function 
		float sx = glm::smoothstep(0.0f, 1.0f, tx);
		float sy = glm::smoothstep(0.0f, 1.0f, ty);

		// linearly interpolate values along the x axis
		float nx0 = glm::mix(c00, c10, sx);
		float nx1 = glm::mix(c01, c11, sx);

		// linearly interpolate the nx0/nx1 along they y axis
		return glm::mix(nx0, nx1, sy);
	}

	static const unsigned kMaxTableSize = 256;
	static const unsigned kMaxTableSizeMask = kMaxTableSize - 1;
	float r[kMaxTableSize];
	unsigned permutationTable[kMaxTableSize * 2];
};