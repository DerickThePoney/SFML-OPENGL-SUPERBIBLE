#pragma once
#include "Application.h"
#include "LightingManager.h"
#include "GameObjectManager.h"
#include "ShaderCompiler.h"
#include "MeshManager.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Renderer.h"
#include "Scene.h"

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
	Renderer m_kRenderer;
	//Camera m_kCamera;
	GameObject* m_pkGameObjects;
	Mesh* m_pkMesh;
	//Material* m_pkMaterial;
	Scene m_kScene;
};

