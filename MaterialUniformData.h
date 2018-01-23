#pragma once

class IUniformDataContainer
{
public:
	virtual void LoadDataFromProgram(GLuint hProgram) = 0;
	virtual void UploadValues() = 0;
	virtual void Inspect() = 0;
	virtual bool Verify(GLuint hProgram) = 0;
	virtual std::string& GetName() = 0;
	virtual void SetName(const std::string& kName) = 0;
	virtual void UpdateUniformLocation(GLuint hiLocation) = 0;
};

typedef std::shared_ptr<IUniformDataContainer> IUniformDataContainerPtr;

template<class T>
inline void DataLoader(const GLuint& hProgram, const I32& iLocation, T& data)
{}

template<class T>
inline void DataUpload(const I32& iLocation, T& data)
{}

template<class T>
inline void DataInspection(const std::string& kName,T& data)
{}

template<class T>
class DataContainer : public IUniformDataContainer
{
public:
	virtual void LoadDataFromProgram(GLuint hProgram) override
	{
		DataLoader<T>(hProgram, m_iLocation, data);
	}

	// Hérité via UniformDataContainer
	virtual void UploadValues() override
	{
		DataUpload<T>(m_iLocation, data);
	}

	virtual void Inspect() override
	{
		DataInspection<T>(m_kName, data);
	}

	virtual bool Verify(GLuint hProgram) override
	{
		I32 iLocation = glGetUniformLocation(hProgram, m_kName.c_str());

		return iLocation == m_iLocation;
	}

	virtual std::string& GetName() override
	{
		return m_kName;
	}

	virtual void SetName(const std::string& kName) override
	{
		m_kName = kName;
	}

	virtual void UpdateUniformLocation(GLuint hiLocation) override
	{
		m_iLocation = hiLocation;
	}
	

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(m_kName, m_iLocation, data);
	}

	template<class Archive>
	void load(Archive& archive)
	{
		archive(m_kName, m_iLocation, data);
	}

	GLint m_iLocation;
	std::string m_kName;
private:
	T data;	
};

CEREAL_REGISTER_TYPE(DataContainer<float>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<float>);

CEREAL_REGISTER_TYPE(DataContainer<vec2>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<vec2>);

CEREAL_REGISTER_TYPE(DataContainer<vec3>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<vec3>);

CEREAL_REGISTER_TYPE(DataContainer<vec4>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<vec4>);

CEREAL_REGISTER_TYPE(DataContainer<I32>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<I32>);

CEREAL_REGISTER_TYPE(DataContainer<ivec2>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<ivec2>);

CEREAL_REGISTER_TYPE(DataContainer<ivec3>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<ivec3>);

CEREAL_REGISTER_TYPE(DataContainer<ivec4>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<ivec4>);

CEREAL_REGISTER_TYPE(DataContainer<UI32>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<UI32>);

CEREAL_REGISTER_TYPE(DataContainer<uvec2>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<uvec2>);

CEREAL_REGISTER_TYPE(DataContainer<uvec3>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<uvec3>);

CEREAL_REGISTER_TYPE(DataContainer<uvec4>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IUniformDataContainer, DataContainer<uvec4>);


/*********************************/
template<>
inline void DataInspection<float>(const std::string& kName, float& data)
{
	ImGui::InputFloat(kName.c_str(), &data);
}

template<>
inline void DataInspection<vec2>(const std::string& kName, vec2& data)
{
	ImGui::InputFloat2(kName.c_str(), data.GetData());
}
template<>
inline void DataInspection<vec3>(const std::string& kName, vec3& data)
{
	ImGui::InputFloat3(kName.c_str(), data.GetData());
}
template<>
inline void DataInspection<vec4>(const std::string& kName, vec4& data)
{
	ImGui::InputFloat4(kName.c_str(), data.GetData());
}

template<>
inline void DataInspection<I32>(const std::string& kName, I32& data)
{
	ImGui::InputInt(kName.c_str(), &data);
}

template<>
inline void DataInspection<ivec2>(const std::string& kName, ivec2& data)
{
	ImGui::InputInt2(kName.c_str(), data.GetData());
}
template<>
inline void DataInspection<ivec3>(const std::string& kName, ivec3& data)
{
	ImGui::InputInt3(kName.c_str(), data.GetData());
}
template<>
inline void DataInspection<ivec4>(const std::string& kName, ivec4& data)
{
	ImGui::InputInt3(kName.c_str(), data.GetData());
}

template<>
inline void DataInspection<UI32>(const std::string& kName, UI32& data)
{
	ImGui::InputInt(kName.c_str(), (I32*)&data);
}

template<>
inline void DataInspection<uvec2>(const std::string& kName, uvec2& data)
{
	ImGui::InputInt2(kName.c_str(), (I32*)data.GetData());
}
template<>
inline void DataInspection<uvec3>(const std::string& kName, uvec3& data)
{
	ImGui::InputInt3(kName.c_str(), (I32*)data.GetData());
}
template<>
inline void DataInspection<uvec4>(const std::string& kName, uvec4& data)
{
	ImGui::InputInt3(kName.c_str(), (I32*)data.GetData());
}


/*********************************/
template<>
inline void DataUpload<float>(const I32& iLocation, float& data)
{
	glUniform1f(iLocation, data);
}
template<>
inline void DataUpload<vec2>(const I32& iLocation, vec2& data)
{
	glUniform2fv(iLocation, 1, data.GetData());
}
template<>
inline void DataUpload<vec3>(const I32& iLocation, vec3& data)
{
	glUniform3fv(iLocation, 1, data.GetData());
}
template<>
inline void DataUpload<vec4>(const I32& iLocation, vec4& data)
{
	glUniform4fv(iLocation, 1, data.GetData());
}

template<>
inline void DataUpload<I32>(const I32& iLocation, I32& data)
{
	glUniform1i(iLocation, data);
}

template<>
inline void DataUpload<ivec2>(const I32& iLocation, ivec2& data)
{
	glUniform2iv(iLocation, 1, data.GetData());
}
template<>
inline void DataUpload<ivec3>(const I32& iLocation, ivec3& data)
{
	glUniform3iv(iLocation, 1, data.GetData());
}
template<>
inline void DataUpload<ivec4>(const I32& iLocation, ivec4& data)
{
	glUniform4iv(iLocation, 1, data.GetData());
}

template<>
inline void DataUpload<UI32>(const I32& iLocation, UI32& data)
{
	glUniform1ui(iLocation, data);
}

template<>
inline void DataUpload<uvec2>(const I32& iLocation, uvec2& data)
{
	glUniform2uiv(iLocation, 1, data.GetData());
}
template<>
inline void DataUpload<uvec3>(const I32& iLocation, uvec3& data)
{
	glUniform3uiv(iLocation, 1, data.GetData());
}
template<>
inline void DataUpload<uvec4>(const I32& iLocation, uvec4& data)
{
	glUniform4uiv(iLocation, 1, data.GetData());
}



/*********************************/
template<>
inline void DataLoader<float>(const GLuint& hProgram, const I32& iLocation, float& data)
{
	//glGetUniformuiv(m_hProgram, info.m_iLocation, value.GetData());
	glGetUniformfv(hProgram, iLocation, &data);
}
template<>
inline void DataLoader<vec2>(const GLuint& hProgram, const I32& iLocation, vec2& data)
{
	glGetUniformfv(hProgram, iLocation, data.GetData());
}
template<>
inline void DataLoader<vec3>(const GLuint& hProgram, const I32& iLocation, vec3& data)
{
	glGetUniformfv(hProgram, iLocation, data.GetData());
}
template<>
inline void DataLoader<vec4>(const GLuint& hProgram, const I32& iLocation, vec4& data)
{
	glGetUniformfv(hProgram, iLocation, data.GetData());
}

template<>
inline void DataLoader<I32>(const GLuint& hProgram, const I32& iLocation, I32& data)
{
	glGetUniformiv(hProgram, iLocation, &data);
}

template<>
inline void DataLoader<ivec2>(const GLuint& hProgram, const I32& iLocation, ivec2& data)
{
	glGetUniformiv(hProgram, iLocation, data.GetData());
}
template<>
inline void DataLoader<ivec3>(const GLuint& hProgram, const I32& iLocation, ivec3& data)
{
	glGetUniformiv(hProgram, iLocation, data.GetData());
}
template<>
inline void DataLoader<ivec4>(const GLuint& hProgram, const I32& iLocation, ivec4& data)
{
	glGetUniformiv(hProgram, iLocation, data.GetData());
}

template<>
inline void DataLoader<UI32>(const GLuint& hProgram, const I32& iLocation, UI32& data)
{
	glGetUniformuiv(hProgram, iLocation, &data);
}

template<>
inline void DataLoader<uvec2>(const GLuint& hProgram, const I32& iLocation, uvec2& data)
{
	glGetUniformuiv(hProgram, iLocation, data.GetData());
}
template<>
inline void DataLoader<uvec3>(const GLuint& hProgram, const I32& iLocation, uvec3& data)
{
	glGetUniformuiv(hProgram, iLocation, data.GetData());
}
template<>
inline void DataLoader<uvec4>(const GLuint& hProgram, const I32& iLocation, uvec4& data)
{
	glGetUniformuiv(hProgram, iLocation, data.GetData());
}
