#include "stdafx.h"
#include "LightingManager.h"

LightingManager* Singleton<LightingManager>::s_kInstance = nullptr;

LightingManager::LightingManager()
{
}

LightingManager::~LightingManager()
{
	m_kLightUniformsBuffer.Unmap(GL_UNIFORM_BUFFER);
	m_kLightUniformsBuffer.Delete();
}

void LightingManager::Initialise()
{
	m_kLightUniformsBuffer.Init(GL_UNIFORM_BUFFER, sizeof(Light) * NUM_LIGHT, NULL, GL_MAP_READ_BIT| GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
	m_kLightsData = reinterpret_cast<Light*>(m_kLightUniformsBuffer.Map(GL_UNIFORM_BUFFER, GL_READ_WRITE));

	for (int i = 0; i < NUM_LIGHT; ++i)
	{
		m_kLightsData[i].m_eLight = UNUSED;
	}
	
	m_kLightsData[0].m_eLight = POINT_LIGHT;
	m_kLightsData[0].m_kPosition = vec4(0, 0, 10, 1);
	m_kLightsData[0].m_kLightColor= vec4(0, 0, 1, 1);
}

void LightingManager::PrepareDataForRenderer()
{
	m_kLightUniformsBuffer.BindToUniformBindingPoint(1);
}

void LightingManager::LightSettings()
{
}


