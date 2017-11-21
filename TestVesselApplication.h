#pragma once
#include "Application.h"

struct Cube
{
	float bf;
	float tf;
	float hf;
	float lt;
	float lb;
	float bb;
	float tb;
	float hb;
};

class TestVesselApplication : public Application
{
public:
	TestVesselApplication();
	~TestVesselApplication();

	void Render(double currentTime) override;
	void Initialise() override;
	void Terminate() override;
	void OnResize(unsigned int width, unsigned int height) override;
	virtual void HandleMessages();

	void LoadAndCompileProgram();

private:
	void GenerateTexture();

	GLuint m_hiProgram;
	GLuint m_hiProgramLine;
	GLuint m_hiVao;
	GLuint m_hiBuffer;
	GLuint m_hiTextureUVS;
	GLuint m_hiTextureObject[2];
	vmath::mat4 m_kProjectionMatrice;
	vmath::mat4 m_kLookAtMatrix;
	GLuint m_hiMoveMatrixUniformLocation;
	GLuint m_hiProjMatrixUniformLocation;
	GLuint m_hiMoveMatrixUniformLocationLine;
	GLuint m_hiProjMatrixUniformLocationLine;
	GLuint m_hiDataLocation;
	GLuint m_hiTcLocation;

	Object m_kTest;

	bool m_bDragMode;
};

