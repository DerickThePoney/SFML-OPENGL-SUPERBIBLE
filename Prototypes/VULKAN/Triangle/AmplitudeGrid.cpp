#include "stdafx.h"
#include "AmplitudeGrid.h"


AmplitudeGrid::AmplitudeGrid() 
	: m_dimensions{ 0, 0, 0, 0 }, m_data{ 0 }
{
}


AmplitudeGrid::~AmplitudeGrid()
{
}


void AmplitudeGrid::resize(int n0, int n1, int n2, int n3) 
{
	m_dimensions = std::array<int, 4>{n0, n1, n2, n3};
	m_data.resize(n0 * n1 * n2 * n3);
}

float &AmplitudeGrid::operator()(int i0, int i1, int i2, int i3)
{
	assert(i0 >= 0 && i0 < dimension(0) && i1 >= 0 && i1 < dimension(1) &&
		i2 >= 0 && i2 < dimension(2) && i3 >= 0 && i3 < dimension(3));

	return m_data[i3 + dimension(3) * (i2 + dimension(2) * (i1 + dimension(1) * i0))];
}

float const &AmplitudeGrid::operator()(int i0, int i1, int i2, int i3) const 
{
	return m_data[i3 +
		dimension(3) * (i2 + dimension(2) * (i1 + dimension(1) * i0))];
}

int AmplitudeGrid::dimension(int dim) const 
{
	return m_dimensions[dim]; 
}