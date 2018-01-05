#pragma once
#include "GameObject.h"
#include "OGLRendering.h"
//using namespace vmath;

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
	void LoadProjectionOnGraphics(GLuint bufferIndex);

	const vmath::mat4& GetLookAt() const { return m_kLookAtMatrix; }
	const vmath::mat4& GetProjection() const { return m_kProjectionMatrice; }

	virtual void UpdateAllTransformsInHierarchy() override;
	virtual void Inspector() override;

	template<class Archive>
	void save(Archive & archive) const 
	{
		archive(m_kTransform);
		archive(CEREAL_NVP(m_eType), CEREAL_NVP(focal), CEREAL_NVP(fNearPlane), CEREAL_NVP(fFarPlane));
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(m_kTransform);
		archive(CEREAL_NVP(m_eType), CEREAL_NVP(focal), CEREAL_NVP(fNearPlane), CEREAL_NVP(fFarPlane));
	}

private:
	void ComputeLookAt();
	void ComputeProjection();
	void InitialiseProjectionBuffer();

private:
	CameraType m_eType;
	float focal;
	float aspectRatio;
	float fNearPlane;
	float fFarPlane;

	float m_top, m_bottom, m_left, m_right;


	OGLBuffer m_kProjectionBuffer;
	vmath::mat4 m_kProjectionMatrice;
	vmath::mat4 m_kLookAtMatrix;
};