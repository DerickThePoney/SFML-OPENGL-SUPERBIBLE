#pragma once
#include "Application.h"
#include "GameObject.h"
#include "Camera.h"

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
	Camera m_kCamera;
	GameObject kGameObjects[3];
	Mesh m_kMesh;
	OGLProgram m_kProgram;
	GLuint m_hiMoveMatrixUniformLocation;
	GLuint m_hiProjMatrixUniformLocation;

};

