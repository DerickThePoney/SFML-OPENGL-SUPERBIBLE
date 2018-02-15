#include "stdafx.h"
#include "TestPBRMaterialComponent.h"
#include "GameObjectManager.h"

TYPE_SYSTEM_RTTI_OBJECT(TestPBRMaterialComponent, IComponent);

TestPBRMaterialComponent::TestPBRMaterialComponent(GameObject * pkParent)
	: m_kPrefabFile("media/prefabs/uvsphere.prefab"), m_kTranslation(vec3(2)), m_fRoughnessIncrement(.1f), m_fMetallicIncrement(.1f), IComponent(pkParent)
{
}

TestPBRMaterialComponent::~TestPBRMaterialComponent()
{
}

void TestPBRMaterialComponent::Init()
{
	CreateChildren();
	ChildrenParameters();
}

void TestPBRMaterialComponent::Inspect()
{
	
	ImGui::LabelText("Prefab", m_kPrefabFile.c_str());

	if (ImGui::SliderFloat2("Translation", m_kTranslation.GetData(), 1, 3))
	{
		ChildrenParameters();
	}

	if (ImGui::ColorEdit3("albedo", m_kAlbedo.GetData()))
	{
		ChildrenParameters();
	}

	bool fullmodif = false;
	if (ImGui::SliderFloat("roughness increment", &m_fRoughnessIncrement, 0.01f, 0.2f)) fullmodif = true;
	if (ImGui::SliderFloat("metallic increment", &m_fMetallicIncrement, 0.01f, 0.2f)) fullmodif = true;
	//if (fullmodif) Init();
	
}

void TestPBRMaterialComponent::Clone(std::shared_ptr<IComponent> pkComponent)
{
	std::shared_ptr<TestPBRMaterialComponent> pkData = std::dynamic_pointer_cast<TestPBRMaterialComponent>(pkComponent);
	if (pkData == nullptr) return;

	
	m_kPrefabFile = pkData->m_kPrefabFile;
	m_kTranslation = pkData->m_kTranslation;
	m_kAlbedo = pkData->m_kAlbedo;
	m_fRoughnessIncrement = pkData->m_fRoughnessIncrement;
	m_fMetallicIncrement = pkData->m_fMetallicIncrement;
}

void TestPBRMaterialComponent::CreateChildren()
{
	m_pkParent->ClearChildren();
	GameObject* prefab = GameObjectManager::Instance()->Instantiate();
	
	//first read asset
	try
	{
		std::ifstream ifstr(m_kPrefabFile);
		//Scene testLoading;
		cereal::XMLInputArchive output(ifstr);
		output(*prefab);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	for (F32 roughness = 0; roughness < 1; roughness += m_fRoughnessIncrement)
	{
		for (F32 metallicness = 0; metallicness < 1; metallicness += m_fMetallicIncrement)
		{
			GameObject* obj = GameObjectManager::Instance()->Instantiate(prefab);
			
			m_pkParent->AddChild(obj);
		}
	}

	GameObjectManager::Instance()->Destroy(prefab);
}

void TestPBRMaterialComponent::ChildrenParameters()
{
	UI32 i = 0, j = 0;
	UI32 nbMetallic = (UI32) (1 / m_fMetallicIncrement);
	for (F32 roughness = 0; roughness < 1; roughness += m_fRoughnessIncrement, ++i)
	{
		for (F32 metallicness = 0; metallicness < 1; metallicness += m_fMetallicIncrement, ++j)
		{
			GameObject* obj = m_pkParent->GetChild(i * nbMetallic + j);
			obj->m_kTransform.SetLocalPosition(vec3(i*m_kTranslation[0], j*m_kTranslation[1], 0));
			std::shared_ptr<MeshRendererComponent> meshRenderer = obj->FindComponent<MeshRendererComponent>();
			meshRenderer->m_pkMaterial->SetData("roughness", roughness);
			meshRenderer->m_pkMaterial->SetData("metallic", metallicness);
			meshRenderer->m_pkMaterial->SetData("albedo", m_kAlbedo);
		}
		j = 0;
	}
}


