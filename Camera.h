#pragma once
#include "GameObject.h"
using namespace vmath;

enum CameraType
{
	ORTHOGRAPHIC,
	PERSPECTIVE
};

class Camera : public GameObject
{
public:
	Camera():GameObject(){}
	void Init(float fovy, float aspectRation, float n, float f);

	void OnResize(unsigned int width, unsigned int height);
	void LoadProjectionOnGraphics(GLuint m_hiProjectionLocation);

	const vmath::mat4& GetLookAt() const { return m_kLookAtMatrix; }

	virtual void UpdateAllTransformsInHierarchy() override;
	virtual void Inspector() override;

private:
	void ComputeLookAt();
	void ComputeProjection();

private:
	CameraType m_eType;
	float focal;
	float aspectRatio;
	float fNearPlane;
	float fFarPlane;

	float m_top, m_bottom, m_left, m_right;

	vmath::mat4 m_kProjectionMatrice;
	vmath::mat4 m_kLookAtMatrix;
};