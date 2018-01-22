#pragma once
#include "Singleton.h"
#include "Camera.h"

class CameraManager : public Singleton<CameraManager>
{
	friend class Singleton<CameraManager>;

protected:
	CameraManager();
	~CameraManager();

public:
	void Initialise();
	void LateInit(UI32 width, UI32 height, vmath::mat4& t);

	Camera* Instantiate();
	void Destroy(Camera* pkCamera);

	void OnResize(UI32 width, UI32 height);

	static Camera* main;
private:
	std::vector<Camera*> m_apkCameras;
};