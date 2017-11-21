#include "stdafx.h"
#include "Camera.h"

Camera::Camera(float fovy, float aspectRation, float n, float f)
	: GameObject("Camera"), focal(fovy), aspectRatio(aspectRation), n(n), f(f)
{

}

void Camera::ImGUIHierarchy(GameObject *& node_clicked)
{
}
