#pragma once
//using namespace vmath;

class Transform
{
public:
	Transform(vec3 kPosition = vec3(0, 0, 0), float kScale = 1, quaternion kOrientation = quaternion(1, vec3(0,0,0)));
	~Transform();

	void UpdateWorldSpaceTransform(Transform* pkParent);

	inline const vec3& GetLocalPosition() const { return localPosition.m_kPosition; }
	inline void SetLocalPosition(const vec3& kLocalPosition) { localPosition.m_kPosition = kLocalPosition; m_bUpToDate = false; }

	inline const float& GetScale() const { return localPosition.m_fScale; }
	inline void SetScale(const float& kScale) { localPosition.m_fScale = kScale; m_bUpToDate = false; }

	inline const vec3 GetWorldspacePosition() const { return worldPosition.m_kPosition; }

	inline const quaternion& GetLocalOrientation() const { return localPosition.m_kOrientation; }
	inline void SetLocalOrientation(const quaternion& kLocalOrientation) { localPosition.m_kOrientation = kLocalOrientation; m_bUpToDate = false; }

	const mat4 GetWorldSpaceTransform() const;

	inline void TranslateInLocalSpace(const vec3& kTranslate) { localPosition.m_kPosition += kTranslate; m_bUpToDate = false; }
	inline void RotateInLocalSpace(const quaternion& kRotate) { localPosition.m_kOrientation = normalize(kRotate * localPosition.m_kOrientation); m_bUpToDate = false;}

	inline vec4 GetForwardVector() { return GetWorldSpaceTransform() * vec4(0, 0, 1, 0); }
	inline vec4 GetUpVector() { return GetWorldSpaceTransform() * vec4(0, 1, 0, 0); }
	inline vec4 GetRightVector() { return GetWorldSpaceTransform() * vec4(1, 0, 0, 0); }

	inline void SetValidate(bool bUpToDate) { m_bUpToDate &= bUpToDate; }
	inline bool IsValid() { return m_bUpToDate; }

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(localPosition.m_kPosition),
			CEREAL_NVP(localPosition.m_fScale),
			CEREAL_NVP(localPosition.m_kOrientation));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		DEARCHIVE_WITH_DEFAULT_FROM_NVP(localPosition.m_kPosition, "m_kLocalPosition", vec3(0, 0, 0));
		DEARCHIVE_WITH_DEFAULT_FROM_NVP(localPosition.m_fScale, "m_kScale", 1);
		DEARCHIVE_WITH_DEFAULT_FROM_NVP(localPosition.m_kOrientation, "m_kLocalOrientation", quaternion(1, vec3(0, 0, 0)));
		/*archive(CEREAL_NVP(localPosition.m_kPosition),
			CEREAL_NVP(localPosition.m_fScale),
			CEREAL_NVP(localPosition.m_kOrientation));*/
	}

private:
	struct TransformData
	{
		vec3 m_kPosition;
		float m_fScale;
		quaternion m_kOrientation;
	};
	
	TransformData localPosition;
	TransformData worldPosition;

	bool m_bUpToDate;
};
