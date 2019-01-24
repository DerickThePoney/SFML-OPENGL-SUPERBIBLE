#pragma once
class AmplitudeGrid
{
public:
	AmplitudeGrid();
	~AmplitudeGrid();

	void resize(int n0, int n1, int n2, int n3);

	float &operator()(int i0, int i1, int i2, int i3);

	float const &operator()(int i0, int i1, int i2, int i3) const;

	int dimension(int dim) const;

private:
	std::vector<float>  m_data;
	std::array<int, 4> m_dimensions;
};

