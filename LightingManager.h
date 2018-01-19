#pragma once
#include "Singleton.h"

struct LightingSettings
{
	
	
	
	template<class Archive>
	void save(Archive & archive) const
	{
	}

	template<class Archive>
	void load(Archive & archive)
	{
	}
};

struct DirectionalLight
{
	vec4 m_kDirection;
	vec4 m_kLightColor;
};

struct PointLight
{
	vec4 m_kPosition;
	vec4 m_kLightColor;
	F32 m_fRange;
};

struct SpotLight
{
	vec4 m_kPosition;
	vec4 m_kDirection;
	vec4 m_kLightColor;
	F32 m_fRange;
	F32 m_fConeSize;
};

enum LIGHT_TYPE
{
	UNUSED,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

#pragma pack (push, 1)
struct Light
{
	vec4 m_kPosition;
	vec4 m_kDirection;
	vec4 m_kLightColor;
	LIGHT_TYPE m_eLight;
	F32 m_fRange;
	F32 m_fConeSize;
	UI32 m_uiLightStrength;
};
#pragma pack (pop)

const UI32 NUM_LIGHT = 10;
class LightingManager : public Singleton<LightingManager>
{
	friend class Singleton<LightingManager>;

protected:
	LightingManager();
	~LightingManager();

public:

	void Initialise();

	void PrepareDataForRenderer();

	

private:
	void LightSettings();

private:
	LightingSettings m_kSettings;


	Light* m_kLightsData;
	OGLBuffer m_kLightUniformsBuffer;
};