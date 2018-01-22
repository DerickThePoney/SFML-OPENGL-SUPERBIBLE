#pragma once
#include "OGLRendering.h"

enum CameraType
{
	ORTHOGRAPHIC,
	PERSPECTIVE
};

class CameraManager;
class Camera
{
	friend class CameraManager;
protected:
	Camera();
	~Camera();

public:
	void Init(UI32 width, UI32 height, vmath::mat4& t);

	void OnResize(UI32 width, UI32 height);
	void LoadProjectionOnGraphics(GLuint bufferIndex);
	const vmath::mat4& GetLookAt() const { return m_kLookAtMatrix; }
	const vmath::mat4& GetProjection() const { return m_kProjectionMatrice; }
	void ComputeLookAt(vmath::mat4& t);
	void ComputeProjection();

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		//GameObject::save(archive);
		archive(CEREAL_NVP(m_eType), CEREAL_NVP(focal), CEREAL_NVP(fNearPlane), CEREAL_NVP(fFarPlane));
		archive(CEREAL_NVP(m_top), CEREAL_NVP(m_bottom));
	}

	template<class Archive>
	void load(Archive & archive)
	{
		//GameObject::load(archive);
		DEARCHIVE_WITH_DEFAULT(m_eType, PERSPECTIVE);
		DEARCHIVE_WITH_DEFAULT(focal, 50.0f);
		DEARCHIVE_WITH_DEFAULT(fNearPlane, 0.1f);
		DEARCHIVE_WITH_DEFAULT(fFarPlane, 1000.0f);
		DEARCHIVE_WITH_DEFAULT(m_top, 2.5);
		DEARCHIVE_WITH_DEFAULT(m_bottom, -2.5);
	}

private:
	void InitialiseProjectionBuffer();

private:
	vmath::mat4 m_kProjectionMatrice;
	vmath::mat4 m_kLookAtMatrix;

public:
	CameraType m_eType;
	float focal;
	float aspectRatio;
	float fNearPlane;
	float fFarPlane;
	float m_top;
	float m_bottom;
	float m_left;
	float m_right;

	OGLBuffer m_kProjectionBuffer;
};
