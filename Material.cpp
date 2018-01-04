#include "stdafx.h"
#include "Material.h"

UI32 Material::s_uiMaxMaterialID = 0;

Material::Material()
{
	m_uiMaterialID = Material::s_uiMaxMaterialID++;
}

Material::Material(const Material & other)
	:m_kFilename(other.m_kFilename), m_kMaterialData(other.m_kMaterialData), m_pkProgram(other.m_pkProgram)
{
	m_uiMaterialID = Material::s_uiMaxMaterialID++;
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
	m_pkProgram = ProgramManager::Instance()->InstantiateFromRessource(m_kMaterialData.m_kShaderFilename);

	if (m_pkProgram == nullptr) // bail if link did not work
	{
		Delete();
		return false;
	}

	//get the id
	//m_uiMaterialID = Material::s_uiMaxMaterialID++;
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
}

void Material::Inspect()
{
	std::stringstream sstr;
	sstr << m_uiMaterialID;

	ImGui::LabelText("ID" , sstr.str().c_str());
	
	const std::vector<ActiveProgramInformations>& uniforms = m_pkProgram->GetUniformsInformation();
	for (size_t i = 0; i < uniforms.size(); ++i)
	{
		if (uniforms[i].m_bIsFromBlock) continue;
		m_pkProgram->InspectUniformProgramInformation(uniforms[i]);
	}	
	
}
