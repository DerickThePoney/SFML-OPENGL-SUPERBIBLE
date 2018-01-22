#include "stdafx.h"
#include "CameraManager.h"

CameraManager* Singleton<CameraManager>::s_kInstance = nullptr;
Camera* CameraManager::main = nullptr;

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::Initialise()
{
}

void CameraManager::LateInit(UI32 width, UI32 height, vmath::mat4 & t)
{
	if (main != nullptr)
	{
		main->Init(width, height, t);
	}
}

Camera * CameraManager::Instantiate()
{
	Camera* pkCam = new Camera();

	if (CameraManager::main == nullptr) CameraManager::main = pkCam;

	m_apkCameras.push_back(pkCam);
	return pkCam;
}

void CameraManager::Destroy(Camera * pkCamera)
{
	if (pkCamera == nullptr)return;
	for (auto it = m_apkCameras.begin(); it < m_apkCameras.end(); ++it)
	{
		if (pkCamera == *it)
		{
			m_apkCameras.erase(it);
			return;
		}
	}
}

void CameraManager::OnResize(UI32 width, UI32 height)
{
	if (main != nullptr)
	{
		main->OnResize(width, height);
	}
}
