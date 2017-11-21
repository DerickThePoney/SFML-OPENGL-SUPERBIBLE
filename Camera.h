#pragma once
#include "GameObject.h"
using namespace vmath;

class Camera : public GameObject
{
public:
	Camera(float fovy, float aspectRation, float n, float f);

	virtual void ImGUIHierarchy(GameObject*& node_clicked) override;
private:
	float focal;
	float aspectRatio;
	float n;
	float f;
	vmath::mat4 m_kProjectionMatrice;
	vmath::mat4 m_kLookAtMatrix;
};