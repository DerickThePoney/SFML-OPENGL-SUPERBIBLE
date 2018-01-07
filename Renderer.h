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

struct RendererGlobalSettings
{
	vec4 kClearColor;
	bool bCullFaces;
	bool bIsCCW;
	bool bCullBack;
	bool bCullFront;
	GLenum ePolygonMode;

	RendererGlobalSettings()
		: kClearColor(vec4(0,0,0,1)), bCullFaces(true), bIsCCW(true), bCullBack(true), bCullFront(false)
	{}

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(bCullFaces),
			CEREAL_NVP(bIsCCW),
			CEREAL_NVP(bCullBack),
			CEREAL_NVP(bCullFront),
			CEREAL_NVP(ePolygonMode),
			CEREAL_NVP(kClearColor)
		);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		DEARCHIVE_WITH_DEFAULT(bCullFaces, true);
		DEARCHIVE_WITH_DEFAULT(bIsCCW, true);
		DEARCHIVE_WITH_DEFAULT(bCullBack, true);
		DEARCHIVE_WITH_DEFAULT(bCullFront, false);
		DEARCHIVE_WITH_DEFAULT(ePolygonMode, GL_FILL);
		DEARCHIVE_WITH_DEFAULT(kClearColor, vec4(0, 0, 0, 1));
	}
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
	void Update();
	void Render(std::vector<GameObjectRenderData>& kVisibleObjectsList, Camera& kCamera);
	void ApplyDefaultState();
	void ApplyGameObjectRenderData(GameObjectRenderData& data);

	void ApplyGraphicsSettings();

	void GraphicsSettings();

	const Mesh& GetDefaultMesh() const { return *m_pkDefaultMesh; }

private:
	void SaveSettings();
	void LoadSettings();

private:
	//window stuff
	sf::RenderWindow* m_window;
	RendererStateData m_kRendererData;
	RendererGlobalSettings m_kGlobalRendererSettings;

	//current rendering state
	UI32 m_auiRenderingState[2];

	//Default rendering state 
	Mesh* m_pkDefaultMesh;
	Material* m_pkDefaultMaterial;

	Material* m_pkLineShaderMaterial;

	//Projection block index
	GLuint m_hiProjectionDataIndex;
};

