#pragma once
#include "Application.h"
#include "vmath.h"
class SpinningCubeApplication :
	public Application
{
public:
	SpinningCubeApplication();
	~SpinningCubeApplication();

	void Render(double currentTime) override;
	void Initialise() override;
	void Terminate() override;
	void OnResize(unsigned int width, unsigned int height) override;

	void LoadAndCompileProgram();

private:
	GLuint m_hiProgram;
	GLuint m_hiVao;
	GLuint m_hiBuffer;
	vmath::mat4 m_kProjectionMatrice;
	GLuint m_hiMoveMatrixUniformLocation;
	GLuint m_hiProjMatrixUniformLocation;
};

