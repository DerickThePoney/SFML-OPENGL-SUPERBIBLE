#include "stdafx.h"
#include "AmplitudeSimulationSpace.h"
#include "InterpolationWavelet.h"

constexpr int pos_modulo(int n, int d) { return (n % d + d) % d; }

constexpr float tau = 6.28318530718f; // https://tauday.com/tau-manifesto

AmplitudeSimulationSpace::AmplitudeSimulationSpace(Settings s)
	: m_spectrum(10)
{
	m_amplitude.resize(s.n_x, s.n_x, s.n_theta, s.n_zeta);
	m_newAmplitude.resize(s.n_x, s.n_x, s.n_theta, s.n_zeta);

	float zeta_min = m_spectrum.minZeta();
	float zeta_max = m_spectrum.maxZeta();

	m_xmin = { -s.size, -s.size, 0.0, zeta_min };
	m_xmax = { s.size, s.size, tau, zeta_max };

	for (int i = 0; i < 4; i++) {
		m_dx[i] = (m_xmax[i] - m_xmin[i]) / m_amplitude.dimension(i);
		m_idx[i] = 1.0f / m_dx[i];
	}

	m_time = s.initial_time;

	m_profileBuffers.resize(s.n_zeta);
	precomputeGroupSpeeds();
}


AmplitudeSimulationSpace::~AmplitudeSimulationSpace()
{
}


void AmplitudeSimulationSpace::timeStep(const float dt, bool fullUpdate) 
{
	if (fullUpdate) {
		advectionStep(dt);
		diffusionStep(dt);
	}
	precomputeProfileBuffers();
	m_time += dt;
}

float AmplitudeSimulationSpace::cflTimeStep() const {
	return std::min(m_dx[X], m_dx[Y]) / groupSpeed(gridDim(Zeta) - 1);
}

std::pair<glm::vec3, glm::vec3> AmplitudeSimulationSpace::waterSurface(glm::vec2 pos) const {

	glm::vec3 surface = { 0, 0, 0 };
	glm::vec3 tx = { 0, 0, 0 };
	glm::vec3 ty = { 0, 0, 0 };

	float ampl = 0;

	for (int izeta = 0; izeta < gridDim(Zeta); izeta++) {
		float  zeta = idxToPos(izeta, Zeta);
		auto &profile = m_profileBuffers[izeta];

		int  DIR_NUM = gridDim(Theta);
		int  N = 4 * DIR_NUM;
		float da = 1.0f / N;
		float dx = DIR_NUM * tau / N;
		for (float a = 0; a < 1; a += da) {

			float angle = a * tau;
			glm::vec2 kdir = glm::vec2{ cosf(angle), sinf(angle) };
			float kdir_x = glm::dot(kdir , pos);

			glm::vec4 prof = profile(kdir_x);
			ampl += amplitude({ pos[X], pos[Y], angle, zeta });
			glm::vec4 wave_data = dx * a * prof;

			surface +=
				glm::vec3{ kdir[0] * wave_data[0], kdir[1] * wave_data[0], wave_data[1] };

			tx += kdir[0] * glm::vec3{ wave_data[2], 0, wave_data[3] };
			ty += kdir[1] * glm::vec3{ 0, wave_data[2], wave_data[3] };
		}
	}

	glm::vec3 normal = normalize(cross(tx, ty));

	return { surface, normal };
}

auto AmplitudeSimulationSpace::extendedGrid() const {
	return [this](int ix, int iy, int itheta, int izeta) {
		// wrap arround for angle
		itheta = pos_modulo(itheta, gridDim(Theta));

		// return zero for wavenumber outside of a domain
		if (izeta < 0 || izeta >= gridDim(Zeta)) {
			return 0.0f;
		}

		// return a default value for points outside of the simulation box
		if (ix < 0 || ix >= gridDim(X) || iy < 0 || iy >= gridDim(Y)) {
			return defaultAmplitude(itheta, izeta);
		}

		// if the point is in the domain the return the actual value of the grid
		return m_amplitude(ix, iy, itheta, izeta);
	};
}

auto AmplitudeSimulationSpace::interpolatedAmplitude() const {

	auto extended_grid = extendedGrid();

	// This function indicated which grid points are in domain and which are not
	auto domain = [this](int ix, int iy, int itheta, int izeta) -> bool {
		return true;// m_enviroment.inDomain(nodePosition(ix, iy));
	};

	/*auto interpolation = InterpolationDimWise(
		// CubicInterpolation, CubicInterpolation,
		LinearInterpolation, LinearInterpolation, LinearInterpolation,
		ConstantInterpolation);

	auto interpolated_grid =
		DomainInterpolation(interpolation, domain)(extended_grid);*/

	return [this](glm::vec4 pos4) mutable {
		return DomainInterpolation(pos4);
	};
}

float AmplitudeSimulationSpace::DomainInterpolation(glm::vec4 pos4) const
{
	auto extended_grid = extendedGrid();

	glm::vec4 ipos4 = posToGrid(pos4);

	glm::ivec4 idx = floor(ipos4);
	glm::vec4 coeffs = ipos4 - glm::vec4(idx[0], idx[1], idx[2], idx[3]);
	glm::vec4 coeffs_inv = 1.0f - coeffs;

	float amplitude0000 = extended_grid(idx[X], idx[Y], idx[Theta], idx[Zeta]);
	float amplitude0001 = extended_grid(idx[X], idx[Y], idx[Theta], idx[Zeta] + 1);
	float amplitude0010 = extended_grid(idx[X], idx[Y], idx[Theta] + 1, idx[Zeta]);
	float amplitude0011 = extended_grid(idx[X], idx[Y], idx[Theta] + 1, idx[Zeta] + 1);
	float amplitude0100 = extended_grid(idx[X], idx[Y] + 1, idx[Theta], idx[Zeta]);
	float amplitude0101 = extended_grid(idx[X], idx[Y] + 1, idx[Theta], idx[Zeta] + 1);
	float amplitude0110 = extended_grid(idx[X], idx[Y] + 1, idx[Theta] + 1, idx[Zeta]);
	float amplitude0111 = extended_grid(idx[X], idx[Y] + 1, idx[Theta] + 1, idx[Zeta] + 1);

	float amplitude1000 = extended_grid(idx[X] + 1, idx[Y], idx[Theta], idx[Zeta]);
	float amplitude1001 = extended_grid(idx[X] + 1, idx[Y], idx[Theta], idx[Zeta] + 1);
	float amplitude1010 = extended_grid(idx[X] + 1, idx[Y], idx[Theta] + 1, idx[Zeta]);
	float amplitude1011 = extended_grid(idx[X] + 1, idx[Y], idx[Theta] + 1, idx[Zeta] + 1);
	float amplitude1100 = extended_grid(idx[X] + 1, idx[Y] + 1, idx[Theta], idx[Zeta]);
	float amplitude1101 = extended_grid(idx[X] + 1, idx[Y] + 1, idx[Theta], idx[Zeta] + 1);
	float amplitude1110 = extended_grid(idx[X] + 1, idx[Y] + 1, idx[Theta] + 1, idx[Zeta]);
	float amplitude1111 = extended_grid(idx[X] + 1, idx[Y] + 1, idx[Theta] + 1, idx[Zeta] + 1);

	float c000 = amplitude0000 * coeffs_inv[0] + amplitude1000 * coeffs[0];
	float c001 = amplitude0001 * coeffs_inv[0] + amplitude1001 * coeffs[0];
	float c010 = amplitude0010 * coeffs_inv[0] + amplitude1010 * coeffs[0];
	float c011 = amplitude0011 * coeffs_inv[0] + amplitude1011 * coeffs[0];
	float c100 = amplitude0100 * coeffs_inv[0] + amplitude1100 * coeffs[0];
	float c101 = amplitude0101 * coeffs_inv[0] + amplitude1101 * coeffs[0];
	float c110 = amplitude0110 * coeffs_inv[0] + amplitude1110 * coeffs[0];
	float c111 = amplitude0111 * coeffs_inv[0] + amplitude1111 * coeffs[0];

	float c00 = c000 * coeffs_inv[1] + c100 * coeffs[1];
	float c01 = c001 * coeffs_inv[1] + c101 * coeffs[1];
	float c10 = c010 * coeffs_inv[1] + c110 * coeffs[1];
	float c11 = c011 * coeffs_inv[1] + c111 * coeffs[1];

	float c0 = c00 * coeffs_inv[2] + c10 * coeffs[2];
	float c1 = c01 * coeffs_inv[2] + c11 * coeffs[2];

	float c = c0 * coeffs_inv[3] + c1 * coeffs[3];

	return c;
}

float AmplitudeSimulationSpace::amplitude(glm::vec4 pos4) const 
{
	auto extended_grid = extendedGrid();


	return interpolatedAmplitude()(pos4);
}

float AmplitudeSimulationSpace::gridValue(Idx idx4) const {
	return extendedGrid()(idx4[X], idx4[Y], idx4[Theta], idx4[Zeta]);
}

std::vector<glm::vec4> AmplitudeSimulationSpace::trajectory(glm::vec4 pos4, float length) const {

	std::vector<glm::vec4> trajectory;
	float              dist = 0;

	for (float dist = 0; dist <= length;) {

		trajectory.push_back(pos4);

		glm::vec2 vel = groupVelocity(pos4);
		float dt = dx(X) / glm::length(vel);

		pos4[X] += dt * vel[X];
		pos4[Y] += dt * vel[Y];

		pos4 = boundaryReflection(pos4);

		dist += dt * glm::length(vel);
	}
	trajectory.push_back(pos4);
	return trajectory;
}

void AmplitudeSimulationSpace::addPointDisturbance(const glm::vec2 pos, const float val) {
	// Find the closest point on the grid to the point `pos`
	int ix = posToIdx(pos[X], X);
	int iy = posToIdx(pos[Y], Y);
	if (ix >= 0 && ix < gridDim(X) && iy >= 0 && iy < gridDim(Y)) {

		for (int itheta = 0; itheta < gridDim(Theta); itheta++) {
			m_amplitude(ix, iy, itheta, 0) += val;
		}
	}
}

void AmplitudeSimulationSpace::advectionStep(const float dt) {

	auto amplitude = interpolatedAmplitude();

#pragma omp parallel for collapse(2)
	for (int ix = 0; ix < gridDim(X); ix++) {
		for (int iy = 0; iy < gridDim(Y); iy++) {

			glm::vec2 pos = nodePosition(ix, iy);

			// update only points in the domain
			if (true){//m_enviroment.inDomain(pos)) {

				for (int itheta = 0; itheta < gridDim(Theta); itheta++) {
					for (int izeta = 0; izeta < gridDim(Zeta); izeta++) {

						glm::vec4 pos4 = idxToPos({ ix, iy, itheta, izeta });
						glm::vec2 vel = groupVelocity(pos4);

						// Tracing back in Semi-Lagrangian
						glm::vec4 trace_back_pos4 = pos4;
						trace_back_pos4[X] -= dt * vel[X];
						trace_back_pos4[Y] -= dt * vel[Y];

						// Take care of boundaries
						trace_back_pos4 = boundaryReflection(trace_back_pos4);

						m_newAmplitude(ix, iy, itheta, izeta) = amplitude(trace_back_pos4);
					}
				}
			}
		}
	}

	std::swap(m_newAmplitude, m_amplitude);
}

glm::vec4 AmplitudeSimulationSpace::boundaryReflection(const glm::vec4 pos4) const {
	glm::vec2 pos = glm::vec2{ pos4[X], pos4[Y] };
	float ls = 1;//;m_enviroment.levelset(pos);
	if (ls >= 0) // no reflection is needed if point is in the domain
		return pos4;

	// Boundary normal is approximatex by the levelset gradient
	glm::vec2 n = glm::vec2(1, 0);// m_enviroment.levelsetGrad(pos);

	float theta = pos4[Theta];
	glm::vec2 kdir = glm::vec2{ cosf(theta), sinf(theta) };

	// Reflect point and wave-vector direction around boundary
	// Here we rely that `ls` is equal to the signed distance from the boundary
	pos = pos - 2.0f * ls * n;
	kdir = kdir - 2.0f * (kdir * n) * n;

	float reflected_theta = atan2(kdir[Y], kdir[X]);

	// We are assuming that after one reflection you are back in the domain. This
	// assumption is valid if you boundary is not so crazy.
	// This assert tests this assumption.
	//assert(m_enviroment.inDomain(pos));

	return glm::vec4{ pos[X], pos[Y], reflected_theta, pos4[Zeta] };
}

void AmplitudeSimulationSpace::diffusionStep(const float dt) {

	auto grid = extendedGrid();

#pragma omp parallel for collapse(2)
	for (int ix = 0; ix < gridDim(X); ix++) {
		for (int iy = 0; iy < gridDim(Y); iy++) {

			float ls = 1;// m_enviroment.levelset(nodePosition(ix, iy));

			for (int itheta = 0; itheta < gridDim(Theta); itheta++) {
				for (int izeta = 0; izeta < gridDim(Zeta); izeta++) {

					glm::vec4 pos4 = idxToPos({ ix, iy, itheta, izeta });
					float gamma = 2 * 0.025f * groupSpeed(izeta) * dt * m_idx[X];

					// do diffusion only if you are 2 grid nodes away from boudnary
					if (ls >= 4 * dx(X)) {
						m_newAmplitude(ix, iy, itheta, izeta) =
							(1 - gamma) * grid(ix, iy, itheta, izeta) +
							gamma * 0.5f *
							(grid(ix, iy, itheta + 1, izeta) +
								grid(ix, iy, itheta - 1, izeta));
					}
					else {
						m_newAmplitude(ix, iy, itheta, izeta) = grid(ix, iy, itheta, izeta);
					}
					// auto dispersion = [](int i) { return 1.0; };
					// float delta =
					//     1e-5 * dt * pow(m_dx[3], 2) * dispersion(waveNumber(izeta));
					// 0.5 * delta *
					//     (m_amplitude(ix, iy, itheta, izeta + 1) +
					//      m_amplitude(ix, iy, itheta, izeta + 1));
				}
			}
		}
	}
	std::swap(m_newAmplitude, m_amplitude);
}

void AmplitudeSimulationSpace::precomputeProfileBuffers() {

	for (int izeta = 0; izeta < gridDim(Zeta); izeta++) {

		float zeta_min = idxToPos(izeta, Zeta) - 0.5f * dx(Zeta);
		float zeta_max = idxToPos(izeta, Zeta) + 0.5f * dx(Zeta);

		// define spectrum

		m_profileBuffers[izeta].precompute(m_spectrum, m_time, zeta_min, zeta_max);
	}
}

void AmplitudeSimulationSpace::precomputeGroupSpeeds() {
	m_groupSpeeds.resize(gridDim(Zeta));
	for (int izeta = 0; izeta < gridDim(Zeta); izeta++) {

		float zeta_min = idxToPos(izeta, Zeta) - 0.5f * dx(Zeta);
		float zeta_max = idxToPos(izeta, Zeta) + 0.5f * dx(Zeta);

		int integration_nodes = 100;

		auto fun = [&](float zeta) -> glm::vec2 {
			float waveLength = powf(2, zeta);
			float waveNumber = tau / waveLength;
			float cg = 0.5f * sqrt(9.81f / waveNumber);
			float density = m_spectrum(zeta);
			return { cg * density, density };
		};

		float dx = (zeta_max - zeta_min) / integration_nodes;
		float x = zeta_min + 0.5f * dx;

		glm::vec2 result = dx * fun(x); // the first integration node
		for (int i = 1; i < integration_nodes; i++) { // proceed with other nodes, notice `int i= 1`
			x += dx;
			result += dx * fun(x);
		}

		m_groupSpeeds[izeta] =
			3 /*the 3 should not be here !!!*/ * result[0] / result[1];
	}
}

float AmplitudeSimulationSpace::idxToPos(const int idx, const int dim) const {
	return m_xmin[dim] + (idx + 0.5f) * m_dx[dim];
}

glm::vec4 AmplitudeSimulationSpace::idxToPos(const Idx idx) const {
	return glm::vec4{ idxToPos(idx[X], X), idxToPos(idx[Y], Y),
				idxToPos(idx[Theta], Theta), idxToPos(idx[Zeta], Zeta) };
}

float AmplitudeSimulationSpace::posToGrid(const float pos, const int dim) const {
	return (pos - m_xmin[dim]) * m_idx[dim] - 0.5f;
}

glm::vec4 AmplitudeSimulationSpace::posToGrid(const glm::vec4 pos4) const {
	return glm::vec4{ posToGrid(pos4[X], X), posToGrid(pos4[Y], Y),
				posToGrid(pos4[Theta], Theta), posToGrid(pos4[Zeta], Zeta) };
}

int AmplitudeSimulationSpace::posToIdx(const float pos, const int dim) const {
	return (int)round(posToGrid(pos, dim));
}

AmplitudeSimulationSpace::Idx AmplitudeSimulationSpace::posToIdx(const glm::vec4 pos4) const {
	return Idx{ posToIdx(pos4[X], X), posToIdx(pos4[Y], Y),
			   posToIdx(pos4[Theta], Theta), posToIdx(pos4[Zeta], Zeta) };
}

glm::vec2 AmplitudeSimulationSpace::nodePosition(int ix, int iy) const {
	return glm::vec2{ idxToPos(ix, 0), idxToPos(iy, 1) };
}

float AmplitudeSimulationSpace::waveLength(int izeta) const {
	float zeta = idxToPos(izeta, Zeta);
	return powf(2, zeta);
}

float AmplitudeSimulationSpace::waveNumber(int izeta) const { return tau / waveLength(izeta); }

float AmplitudeSimulationSpace::dispersionRelation(float knum) const {
	const float g = 9.81f;
	return sqrt(knum * g);
}

float AmplitudeSimulationSpace::dispersionRelation(glm::vec4 pos4) const {
	float knum = waveNumber((int)pos4[Zeta]);
	return dispersionRelation(knum);
}

// float AmplitudeSimulationSpace::groupSpeed(glm::vec4 pos4) const {
//   float       knum = waveNumber(pos4[Zeta]);
//   const float g    = 9.81;
//   return 0.5 * sqrt(g / knum);
// }

float AmplitudeSimulationSpace::groupSpeed(int izeta) const { return m_groupSpeeds[izeta]; }

glm::vec2 AmplitudeSimulationSpace::groupVelocity(glm::vec4 pos4) const {
	int  izeta = posToIdx(pos4[Zeta], Zeta);
	float cg = groupSpeed(izeta);
	float theta = pos4[Theta];
	return cg * glm::vec2{ cosf(theta), sinf(theta) };
}

float AmplitudeSimulationSpace::defaultAmplitude(const int itheta, const int izeta) const {
	if (itheta == 5 * gridDim(Theta) / 16)
		return 0.1f;
	return 0.0f;
}

int AmplitudeSimulationSpace::gridDim(const int dim) const {
	return m_amplitude.dimension(dim);
}

float AmplitudeSimulationSpace::dx(int dim) const { return m_dx[dim]; }

