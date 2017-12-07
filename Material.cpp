#include "stdafx.h"
#include "Material.h"

UI32 Material::s_uiMaxMaterialID = 0;

Material::Material()
{
}


Material::~Material()
{
}

bool Material::InitFromFile(const std::vector<std::string>& filenames, const std::vector<GLenum> eShaderTypes)
{
	//create shaders objects
	OGLShader* shaders = new OGLShader[filenames.size()];
	bool res = true;
	
	//compile shaders
	for (std::size_t i = 0; i < filenames.size(); ++i)
	{
		bool resShader = shaders[i].InitFromFile(filenames[i].c_str(), eShaderTypes[i]);
		if (!resShader)
		{
			return resShader;
		}
	}
	
	//link the program. This deletes the shader objects, cause we don't need it no more :-)
	res = m_kProgram.LinkProgram(shaders, filenames.size(), true);

	if (!res) // bail if link did not work
	{
		Delete();
		return res;
	}

	//get the id
	m_uiMaterialID = Material::s_uiMaxMaterialID++;

	return res;
}

void Material::Delete()
{
	m_kProgram.DeleteProgram();
}

void Material::Use()
{
	m_kProgram.UseProgram();
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
		const std::vector<ActiveProgramInformations>& uniforms = m_kProgram.GetUniformsInformation();
		for (size_t i = 0; i < uniforms.size(); ++i)
		{
			if (uniforms[i].m_bIsFromBlock) continue;
			ImGui::Bullet();
			ImGui::Text("%s", uniforms[i].m_pcName);			
			ImGui::LabelText("Type", "\t%s", OGLUtilities::GLTypeEnumToCString(uniforms[i].m_eType));

		}
		ImGui::TreePop();
	}
	
}
