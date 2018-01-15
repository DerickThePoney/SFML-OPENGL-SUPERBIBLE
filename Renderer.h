#pragma once
#include "DebugRenderingCommands.h"
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
	bool m_bVSync;
	vec4 kClearColor;
	bool bDepthTesting;
	bool bCullFaces;
	bool bIsCCW;
	bool bCullBack;
	bool bCullFront;
	GLenum ePolygonMode;
	bool bBlit;

	RendererGlobalSettings()
		: m_bVSync(true), kClearColor(vec4(0,0,0,1)), bDepthTesting(true), bCullFaces(true), bIsCCW(true), bCullBack(true), bCullFront(false), bBlit(false)
	{}

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(
			CEREAL_NVP(m_bVSync),
			CEREAL_NVP(bCullFaces),
			CEREAL_NVP(bIsCCW),
			CEREAL_NVP(bCullBack),
			CEREAL_NVP(bCullFront),
			CEREAL_NVP(ePolygonMode),
			CEREAL_NVP(kClearColor),
			CEREAL_NVP(bDepthTesting),
			CEREAL_NVP(bBlit)
		);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		DEARCHIVE_WITH_DEFAULT(m_bVSync, true);
		DEARCHIVE_WITH_DEFAULT(bCullFaces, true);
		DEARCHIVE_WITH_DEFAULT(bIsCCW, true);
		DEARCHIVE_WITH_DEFAULT(bCullBack, true);
		DEARCHIVE_WITH_DEFAULT(bCullFront, false);
		DEARCHIVE_WITH_DEFAULT(ePolygonMode, GL_FILL);
		DEARCHIVE_WITH_DEFAULT(kClearColor, vec4(0, 0, 0, 1));
		DEARCHIVE_WITH_DEFAULT(bDepthTesting, true);
		DEARCHIVE_WITH_DEFAULT(bBlit, false);
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

	void OnResize(unsigned int width, unsigned int height);

	void ApplyGraphicsSettings();

	void GraphicsSettings();

	const Mesh& GetDefaultMesh() const { return *m_pkDefaultMesh; }

private:
	void SetVerticalSynchronisation();
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
	Mesh* m_pkScreenQuad;
	Material* m_pkDefaultMaterial;

	Material* m_pkLineShaderMaterial;
	Material* m_pkBlitShader;


	OGLFrameBuffer m_kFrameBuffer;
	OGLRenderBuffer m_kDepthBuffer;
	OGLTexture2D m_kScreenTexture;
	OGLTexture2D m_kDepthTexture;


	//Projection block index
	GLuint m_hiProjectionDataIndex;
};

