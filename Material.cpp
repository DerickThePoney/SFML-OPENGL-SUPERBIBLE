#include "stdafx.h"
#include "Material.h"

UI32 Material::s_uiMaxMaterialID = 0;

Material::Material()
	: m_pkProgram(nullptr)
{
	m_uiMaterialID = Material::s_uiMaxMaterialID++;
}

Material::Material(const Material & other)
	: m_kFilename(other.m_kFilename), m_kMaterialData(other.m_kMaterialData), m_pkProgram(other.m_pkProgram)
{
	m_uiMaterialID = Material::s_uiMaxMaterialID++;
	m_kMaterialData.m_akDataForShader.clear();
	PrepareDataForContainers();
}


Material::~Material()
{
	Delete();
}

bool Material::InitMaterialFromRessource(const std::string & kFilename)
{
	//first read asset
	try
	{
		std::ifstream ifstr(kFilename);
		//Scene testLoading;
		cereal::XMLInputArchive output(ifstr);
		output(m_kMaterialData);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	//"Compile" Material
	if (!LoadProgram())
	{
		return false;
	}

	m_kFilename = kFilename;

	return true;
}

void Material::Delete()
{
	ProgramManager::Instance()->Destroy(m_pkProgram);
}

void Material::Use()
{
	m_pkProgram->UseProgram();
	if (m_kMaterialData.m_bBlendColors)
	{
		glEnable(GL_BLEND);

		if(m_kMaterialData.m_bBlendSeparate)
		{
			glBlendFuncSeparate(
				m_kMaterialData.m_eSRCBlend,
				m_kMaterialData.m_eSRCBlendAlpha,
				m_kMaterialData.m_eDSTBlend,
				m_kMaterialData.m_eDSTBlendAlpha
			);
		}
		else
		{
			glBlendFunc(
				m_kMaterialData.m_eSRCBlend,
				m_kMaterialData.m_eDSTBlend
			);
		}
	}
	else
	{
		glDisable(GL_BLEND);
	}

	LoadDataInGPU();
}



bool Material::LoadProgram()
{
	if (m_pkProgram != nullptr)
	{
		Delete();
	}

	m_pkProgram = ProgramManager::Instance()->InstantiateFromRessource(m_kMaterialData.m_kShaderFilename);

	if (m_pkProgram == nullptr) // bail if link did not work
	{
		Delete();
		return false;
	}

	PrepareDataForContainers();

	return true;
}

void Material::Inspect()
{
	std::stringstream sstr;
	sstr << m_uiMaterialID;

	ImGui::LabelText("ID" , sstr.str().c_str());

	if (ImGui::Button("Save values"))
	{
		std::ofstream ofstr(m_kFilename);
		//Scene testLoading;
		cereal::XMLOutputArchive output(ofstr);
		output(m_kMaterialData);
	}

	ImGui::SameLine();
	if (ImGui::Button("Reload shaders"))
	{
		LoadProgram();
	}

	ImGui::Checkbox("Per Instance data", &m_kMaterialData.m_bPerInstanceData);

	//color blending
	ImGui::Checkbox("Blend colors", &m_kMaterialData.m_bBlendColors);
	if (m_kMaterialData.m_bBlendColors)
	{
		ImGui::Checkbox("Separate alpha blend", &m_kMaterialData.m_bBlendSeparate);

		static char* blendFuncs[19] = 
		{ 
			"GL_ZERO" , "GL_ONE" , "GL_SRC_COLOR", "GL_ONE_MINUS_SRC_COLOR", 
			"GL_DST_COLOR", "GL_ONE_MINUS_DST_COLOR", "GL_SRC_ALPHA", 
			"GL_ONE_MINUS_SRC_ALPHA", "GL_DST_ALPHA", "GL_ONE_MINUS_DST_ALPHA", 
			"GL_CONSTANT_COLOR", "GL_ONE_MINUS_CONSTANT_COLOR", 
			"GL_CONSTANT_ALPHA", "GL_ONE_MINUS_CONSTANT_ALPHA", 
			"GL_SRC_ALPHA_SATURATE", "GL_SRC1_COLOR", "GL_ONE_MINUS_SRC1_COLOR", 
			"GL_SRC1_ALPHA", "GL_ONE_MINUS_SRC1_ALPHA" 
		};

		//source blending
		int index = BlendFuncToIndex(m_kMaterialData.m_eSRCBlend);
		if (ImGui::Combo("Source blending", &index, blendFuncs, 19))
		{
			m_kMaterialData.m_eSRCBlend = IndexToBlendFunc(index);
		}
		if (m_kMaterialData.m_bBlendSeparate)
		{
			index = BlendFuncToIndex(m_kMaterialData.m_eSRCBlendAlpha);
			if (ImGui::Combo("Source alpha blending", &index, blendFuncs, 19))
			{
				m_kMaterialData.m_eSRCBlendAlpha = IndexToBlendFunc(index);
			}
		}

		//Destination blending
		index = BlendFuncToIndex(m_kMaterialData.m_eDSTBlend);
		if (ImGui::Combo("Destination blending", &index, blendFuncs, 19))
		{
			m_kMaterialData.m_eDSTBlend = IndexToBlendFunc(index);
		}
		if (m_kMaterialData.m_bBlendSeparate)
		{
			index = BlendFuncToIndex(m_kMaterialData.m_eDSTBlendAlpha);
			if (ImGui::Combo("Destination alpha blending", &index, blendFuncs, 19))
			{
				m_kMaterialData.m_eDSTBlendAlpha = IndexToBlendFunc(index);
			}
		}
		

	}
	
	InspectData();
	
}

I32 Material::BlendFuncToIndex(GLenum blendFunc)
{
	switch (blendFunc)
	{
	case GL_ZERO: return 0;
	case GL_ONE: return 1;
	case GL_SRC_COLOR: return 2;
	case GL_ONE_MINUS_SRC_COLOR: return 3;
	case GL_DST_COLOR: return 4;
	case GL_ONE_MINUS_DST_COLOR: return 5;
	case GL_SRC_ALPHA: return 6;
	case GL_ONE_MINUS_SRC_ALPHA: return 7;
	case GL_DST_ALPHA: return 8;
	case GL_ONE_MINUS_DST_ALPHA: return 9;
	case GL_CONSTANT_COLOR: return 10;
	case GL_ONE_MINUS_CONSTANT_COLOR: return 11;
	case GL_CONSTANT_ALPHA: return 12;
	case GL_ONE_MINUS_CONSTANT_ALPHA:return 13;
	case GL_SRC_ALPHA_SATURATE:return 14;
	case GL_SRC1_COLOR:return 15;
	case GL_ONE_MINUS_SRC1_COLOR:return 16;
	case GL_SRC1_ALPHA:return 17;
	case GL_ONE_MINUS_SRC1_ALPHA:return 18;
	default:
		return -1;
	}
}

GLenum Material::IndexToBlendFunc(I32 blendFunc)
{
	switch (blendFunc)
	{
	case 0: return GL_ZERO;
	case 1: return GL_ONE;
	case 2: return GL_SRC_COLOR;
	case 3: return GL_ONE_MINUS_SRC_COLOR;
	case 4: return GL_DST_COLOR;
	case 5: return GL_ONE_MINUS_DST_COLOR;
	case 6: return GL_SRC_ALPHA;
	case 7: return GL_ONE_MINUS_SRC_ALPHA;
	case 8: return GL_DST_ALPHA;
	case 9: return GL_ONE_MINUS_DST_ALPHA;
	case 10: return GL_CONSTANT_COLOR;
	case 11: return GL_ONE_MINUS_CONSTANT_COLOR;
	case 12: return GL_CONSTANT_ALPHA;
	case 13: return GL_ONE_MINUS_CONSTANT_ALPHA;
	case 14: return GL_SRC_ALPHA_SATURATE;
	case 15: return GL_SRC1_COLOR;
	case 16: return GL_ONE_MINUS_SRC1_COLOR;
	case 17: return GL_SRC1_ALPHA;
	case 18: return GL_ONE_MINUS_SRC1_ALPHA;
	default:
		return GL_ZERO;
	}
}

void Material::PrepareDataForContainers()
{
	//check unknown uniforms
	const std::vector<ActiveProgramInformations>& uniforms = m_pkProgram->GetUniformsInformation();
	for (size_t i = 0; i < uniforms.size(); ++i)
	{
		if (uniforms[i].m_bIsFromBlock) continue;

		if (!IsAlreadyMapped(uniforms[i]))
		{
			Map(uniforms[i]);
		}
	}


	//delete containers that are not existent
	if (m_kMaterialData.m_akDataForShader.size() != uniforms.size())
	{
		for (auto it = m_kMaterialData.m_akDataForShader.begin(); it != m_kMaterialData.m_akDataForShader.end(); ++it)
		{
			bool bFound = false;
			std::string thisName = (*it)->GetName();
			for (size_t j = 0; j < uniforms.size(); ++j)
			{
				if (std::string(uniforms[j].m_pcName) == thisName)
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				it = m_kMaterialData.m_akDataForShader.erase(it);
			}
		}
	}
}

bool Material::IsAlreadyMapped(const ActiveProgramInformations & info)
{
	//Create loop through data to check the data is already mapped
	for (size_t i = 0; i < m_kMaterialData.m_akDataForShader.size(); ++i)
	{
		if (std::string(info.m_pcName) == m_kMaterialData.m_akDataForShader[i]->GetName())
		{
			if (!m_kMaterialData.m_akDataForShader[i]->Verify(*m_pkProgram))
			{
				m_kMaterialData.m_akDataForShader[i]->UpdateUniformLocation(info.m_iLocation);
			}

			return true;
		}
	}

	return false;
}

void Material::Map(const ActiveProgramInformations & info)
{
	IUniformDataContainerPtr container;
	switch (info.m_eType)
	{
	case GL_FLOAT:
	{
		container = IUniformDataContainerPtr(new DataContainer<float>());
		break;
	}
	case GL_FLOAT_VEC2:
	{
		container = IUniformDataContainerPtr(new DataContainer<vec2>());
		break;
	}
	case GL_FLOAT_VEC3:
	{
		container = IUniformDataContainerPtr(new DataContainer<vec3>());
		break;
	}
	case GL_FLOAT_VEC4:
	{
		container = IUniformDataContainerPtr(new DataContainer<vec4>());
		break;
	}
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3: {return; }
	case GL_INT:
	{
		container = IUniformDataContainerPtr(new DataContainer<I32>());
		break;
	}
	case GL_INT_VEC2:
	{
		container = IUniformDataContainerPtr(new DataContainer<ivec2>());
		break;
	}
	case GL_INT_VEC3:
	{
		container = IUniformDataContainerPtr(new DataContainer<ivec3>());
		break;
	}
	case GL_INT_VEC4:
	{
		container = IUniformDataContainerPtr(new DataContainer<ivec4>());
		break;
	}
	case GL_UNSIGNED_INT:
	{
		container = IUniformDataContainerPtr(new DataContainer<UI32>());
		break;
	}
	case GL_UNSIGNED_INT_VEC2:
	{
		container = IUniformDataContainerPtr(new DataContainer<uvec2>());
		break;
	}
	case GL_UNSIGNED_INT_VEC3:
	{
		container = IUniformDataContainerPtr(new DataContainer<uvec3>());
		break;
	}
	case GL_UNSIGNED_INT_VEC4:
	{
		container = IUniformDataContainerPtr(new DataContainer<uvec4>());
		break;
	}
	case GL_DOUBLE_MAT2:
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4:
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
	default:
	{return; }
	}

	container->SetName(info.m_pcName);
	container->UpdateUniformLocation(info.m_iLocation);
	container->LoadDataFromProgram(*m_pkProgram);

	m_kMaterialData.m_akDataForShader.push_back(container);
}

void Material::LoadDataInGPU()
{
	for (size_t i = 0; i < m_kMaterialData.m_akDataForShader.size(); ++i)
	{
		m_kMaterialData.m_akDataForShader[i]->UploadValues();
	}
}

void Material::InspectData()
{
	for (size_t i = 0; i < m_kMaterialData.m_akDataForShader.size(); ++i)
	{
		m_kMaterialData.m_akDataForShader[i]->Inspect();
	}
}
