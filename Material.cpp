#include "stdafx.h"
#include "Material.h"

UI32 Material::s_uiMaxMaterialID = 0;

Material::Material()
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
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	// Node
	std::stringstream sstr;
	sstr << "Material ID " << m_uiMaterialID;
	bool node_open = ImGui::TreeNodeEx(sstr.str().c_str(), node_flags);


	if (node_open)
	{
		const std::vector<ActiveProgramInformations>& uniforms = m_pkProgram->GetUniformsInformation();
		for (size_t i = 0; i < uniforms.size(); ++i)
		{
			if (uniforms[i].m_bIsFromBlock) continue;
			m_pkProgram->InspectUniformProgramInformation(uniforms[i]);
		}
		ImGui::TreePop();
	}
	
}
