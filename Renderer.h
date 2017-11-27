#pragma once
#include "BMesh.h"
#include "MeshRendererComponent.h"
#include "GameObject.h"
#include "Camera.h"

struct GameObjectRenderData
{
	Transform* m_pkTransform;
	MeshRendererComponent* m_pkMeshRenderer;
};


enum RENDER_STATE
{
	MATERIAL = 0,
	MESH = 1
};


class Renderer
{
public:
	Renderer();
	~Renderer();

	//init functions
	void Init();
	void InitDefaultState();

	//terminate function
	void Terminate();
	void TerminateDefaultState();


	//rendering functions
	void Render(std::vector<GameObjectRenderData>& kVisibleObjectsList, Camera& kCamera);
	void ApplyDefaultState();
	void ApplyGameObjectRenderData(GameObjectRenderData& data);

private:
	
	UI32 m_auiRenderingState[2];

	Mesh m_kDefaultMesh;
	Material m_kDefaultMaterial;

	GLuint m_hiProjectionDataIndex;
};

