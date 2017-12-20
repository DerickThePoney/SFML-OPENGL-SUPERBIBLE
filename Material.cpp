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

bool Material::InitFromFiles(const std::vector<std::string>& filenames, const std::vector<GLenum> eShaderTypes)
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

	return res;
}

bool Material::InitMaterialFromRessource(const std::string & kFilename)
{
	//"Compile" Material
	std::vector<MaterialInformation> infos;
	MaterialCompiler::instance.RetrieveShaders(infos, kFilename);

	//create shaders objects
	OGLShader* shaders = new OGLShader[infos.size()];
	bool res = true;

	//compile shaders
	for (std::size_t i = 0; i < infos.size(); ++i)
	{
		bool resShader = shaders[i].InitFromMemory(infos[i].m_kShaderCode.c_str(), infos[i].m_eShaderType, kFilename.c_str());
		if (!resShader)
		{
			return resShader;
		}
	}

	//link the program. This deletes the shader objects, cause we don't need it no more :-)
	res = m_kProgram.LinkProgram(shaders, infos.size(), true);

	if (!res) // bail if link did not work
	{
		Delete();
		return res;
	}

	//get the id
	//m_uiMaterialID = Material::s_uiMaxMaterialID++;
	m_kFilename = kFilename;

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
			m_kProgram.InspectUniformProgramInformation(uniforms[i]);
		}
		ImGui::TreePop();
	}
	
}
