#pragma once
#include "Application.h"
#include "GameObject.h"

class GameObjectApplication :
	public Application
{
public:
	GameObjectApplication();
	~GameObjectApplication();


	void Initialise() override;
	void Update(double deltaTime) override;
	void Render(double currentTime) override;
	void Terminate() override;
	void OnResize(unsigned int width, unsigned int height) override;

	//virtual void HandleMessages() override;

	//bool onDialog_Closed(const CEGUI::EventArgs& e);
	//bool onEditbox_TextChanged(const CEGUI::EventArgs& e);
	void LoadAndCompileProgram();

private:
	GameObject kGameObjects[3];
	Mesh m_kMesh;
	Mesh m_kDirectionsX;
	Mesh m_kDirectionsY;
	GLuint m_hiProgram;
	GLuint m_hiProgramArrows;
	GLuint m_hiMoveMatrixUniformLocation;
	GLuint m_hiColor;
	GLuint m_hiProjColor;
	GLuint m_hiMovColor;
	GLuint m_hiProjMatrixUniformLocation;
	vmath::mat4 m_kProjectionMatrice;
	vmath::mat4 m_kLookAtMatrix;

};

