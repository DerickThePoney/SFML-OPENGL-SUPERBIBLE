#pragma once
//using namespace vmath;

class Transform
{
public:
	Transform(vec3 kPosition = vec3(0, 0, 0), vec3 kScale = vec3(1, 1, 1), quaternion kOrientation = quaternion(1, vec3(0,0,0)));
	~Transform();

	void UpdateWorldSpaceTransform(Transform* pkParent);

	inline const vec3& GetLocalPosition() const { return m_kLocalPosition; }
	inline void SetLocalPosition(const vec3& kLocalPosition) { m_kLocalPosition = kLocalPosition; m_bUpToDate = false; }

	inline const vec3& GetScale() const { return m_kScale; }
	inline void SetScale(const vec3& kScale) { m_kScale = kScale; m_bUpToDate = false; }

	inline const vec4 GetWorldspacePosition() const { return m_kWorlspaceTransform[3]; }

	inline const quaternion& GetLocalOrientation() const { return m_kLocalOrientation; }
	inline void SetLocalOrientation(const quaternion& kLocalOrientation) { m_kLocalOrientation = kLocalOrientation; m_bUpToDate = false; }

	inline const mat4 GetWorldSpaceTransform() const { return m_kWorlspaceTransform; }

	inline void TranslateInLocalSpace(const vec3& kTranslate) { m_kLocalPosition += kTranslate; m_bUpToDate = false; }
	inline void RotateInLocalSpace(const quaternion& kTranslate) { m_kLocalOrientation = normalize(kTranslate * m_kLocalOrientation); m_bUpToDate = false;}

	void WriteIntoStream(std::ostream& out)
	{
		out << "LP:(" << m_kLocalPosition[0] << "," << m_kLocalPosition[1] << "," << m_kLocalPosition[2] << ")\n";
		vec4 kWPosition = m_kWorlspaceTransform[3];
		out << "WP:(" << kWPosition[0] << "," << kWPosition[1] << "," << kWPosition[2] << ")\n";
	}

	inline void SetValidate(bool bUpToDate) { m_bUpToDate &= bUpToDate; }
	inline bool IsValid() { return m_bUpToDate; }

	void Inspect();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_kLocalPosition),
			CEREAL_NVP(m_kScale),
			CEREAL_NVP(m_kLocalOrientation));
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(CEREAL_NVP(m_kLocalPosition),
			CEREAL_NVP(m_kScale),
			CEREAL_NVP(m_kLocalOrientation));
	}

private:
	vec3 m_kLocalPosition;
	vec3 m_kScale;
	quaternion m_kLocalOrientation;
	mat4 m_kWorlspaceTransform;

	bool m_bUpToDate;
};
