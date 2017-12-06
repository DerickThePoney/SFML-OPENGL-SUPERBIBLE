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


struct RendererStateData
{
	I32 m_iGLMajorVersion;
	I32 m_iGLMinorVersion;
	const GLubyte* m_kHardwareVendor;
	const GLubyte*  m_kRendererName;
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
	void Init(sf::RenderWindow* m_window);
	void ExtractGLContextInformation();
	void InitDefaultState();

	//terminate function
	void Terminate();
	void TerminateDefaultState();


	//rendering functions
	void Render(std::vector<GameObjectRenderData>& kVisibleObjectsList, Camera& kCamera);
	void ApplyDefaultState();
	void ApplyGameObjectRenderData(GameObjectRenderData& data);

private:
	//window stuff
	sf::RenderWindow* m_window;
	RendererStateData m_kRendererData;

	//current rendering state
	UI32 m_auiRenderingState[2];

	//Default rendering state 
	Mesh* m_pkDefaultMesh;
	Material m_kDefaultMaterial;

	//Projection block index
	GLuint m_hiProjectionDataIndex;
};

