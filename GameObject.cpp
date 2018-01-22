#include "stdafx.h"
#include "GameObject.h"

UI32 GameObject::s_uiIDCounter = 0;


GameObject::GameObject()
	: m_pkParent(nullptr), m_kName(""), m_uiID(s_uiIDCounter++)//, m_kMeshRenderer(this)
{
}

GameObject::GameObject(GameObject * pkParent)
	: m_pkParent(pkParent), m_kName(""), m_uiID(s_uiIDCounter++)//, m_kMeshRenderer(this)
{
}

GameObject::GameObject(GameObject * pkParent, const std::string & rkName)
	: m_pkParent(pkParent), m_kName(rkName), m_uiID(s_uiIDCounter++)//, m_kMeshRenderer(this)
{
}

GameObject::GameObject(const std::string & rkName)
	: m_pkParent(nullptr), m_kName(rkName), m_uiID(s_uiIDCounter++)//, m_kMeshRenderer(this)
{
}


GameObject::~GameObject()
{
	m_apkChildren.clear();
}

bool GameObject::AddChild(GameObject * pkObject)
{
	if (!CheckPresenceInChildren(pkObject))
	{
		pkObject->m_pkParent = this;
		m_apkChildren.push_back(pkObject);
		return true;
	}
	return false;
}

bool GameObject::RemoveChild(const UI32 iIndex)
{
	if (iIndex < 0 || iIndex >= m_apkChildren.size())
	{
		return false;
	}

	m_apkChildren.erase(m_apkChildren.begin() + iIndex);
	return true;
}

bool GameObject::RemoveChild(const std::string & rkName)
{
	for (auto it = m_apkChildren.begin(); it != m_apkChildren.end(); ++it)
	{
		if ((*it)->m_kName == rkName)
		{
			m_apkChildren.erase(it);
			return true;
		}
	}

	return false;
}

std::size_t GameObject::GetNumberOfChildren()
{
	return m_apkChildren.size();
}

void GameObject::FindChildrenInHierarchy(const std::string & rkName, std::vector<GameObject*> apkReturnVector)
{
	for (std::size_t it = 0; it < m_apkChildren.size(); ++it)
	{
		if (m_apkChildren[it]->m_kName == rkName)
		{
			apkReturnVector.push_back(m_apkChildren[it]);
		}
		m_apkChildren[it]->FindChildrenInHierarchy(rkName, apkReturnVector);
	}
}

GameObject * GameObject::GetChild(const UI32 iIndex)
{
	if (iIndex < 0 || iIndex >= m_apkChildren.size())
	{
		return nullptr;
	}

	return m_apkChildren[iIndex];
}

GameObject * GameObject::GetChild(const std::string & rkName)
{
	for (std::size_t it = 0; it < m_apkChildren.size(); ++it)
	{
		if (m_apkChildren[it]->m_kName == rkName)
		{
			return m_apkChildren[it];
		}
	}

	return nullptr;
}

void GameObject::Update(double fElapsedTime)
{
	for (UI32 i = 0; i < m_kComponent.size(); ++i)
	{
		m_kComponent[i]->Update(fElapsedTime);
	}
}

bool GameObject::CheckPresenceInChildren(GameObject * pkGameObject)
{
	for (std::size_t i = 0; i < m_apkChildren.size(); ++i)
	{
		if (m_apkChildren[i] == pkGameObject)
		{
			return true;
		}
	}
	return false;
}

void GameObject::UpdateAllTransformsInHierarchy()
{
	//update the childrens flags
	for (std::size_t i = 0; i < m_apkChildren.size(); ++i)
	{
		m_apkChildren[i]->m_kTransform.SetValidate(m_kTransform.IsValid());
	}

	if (m_pkParent == nullptr)
	{
		m_kTransform.UpdateWorldSpaceTransform(nullptr);
	}
	else
	{
		m_kTransform.UpdateWorldSpaceTransform(&m_pkParent->m_kTransform);
	}

	for (std::size_t i = 0; i < m_apkChildren.size(); ++i)
	{
		m_apkChildren[i]->UpdateAllTransformsInHierarchy();
	}
}

void GameObject::OnPreRender()
{
	for (UI32 i = 0; i < m_kComponent.size(); ++i)
	{
		m_kComponent[i]->OnPreRender();
	}
}

void GameObject::ImGUIHierarchy(GameObject*& node_clicked)
{
	// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((this == node_clicked) ? ImGuiTreeNodeFlags_Selected : 0);
	if (m_apkChildren.size() > 0)
	{
		// Node
		bool node_open = ImGui::TreeNodeEx(m_kName.c_str(), node_flags);
		if (ImGui::IsItemClicked())
			node_clicked = this;
		if (node_open)
		{
			for (std::size_t i = 0; i < m_apkChildren.size(); ++i)
			{
				m_apkChildren[i]->ImGUIHierarchy(node_clicked);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		// Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
		ImGui::TreeNodeEx(m_kName.c_str(), node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
		if (ImGui::IsItemClicked())
			node_clicked = this;
	}
}

void GameObject::Inspector()
{
	char buff[100];
	strcpy(buff, m_kName.c_str());
	ImGui::InputText("Name", buff, 100);
	m_kName = buff;
	m_kTransform.Inspect();

	for (UI32 i = 0; i < m_kComponent.size(); ++i)
	{
		m_kComponent[i]->Inspect();
	}

	ImGui::Separator();
	
	static int selectedComponent = -1;
	ImGui::Combo("Name", &selectedComponent, Components, OGL_ARRAYSIZE(Components));

	ImGui::SameLine();

	if (ImGui::Button("Add component"))
	{
		AddComponent(GET_TYPE(Components[selectedComponent]));
	}

}

void GameObject::AddComponent(const RTTI* pkComponentType)
{
	IComponentPtr pkComponent = CREATE_NEW_COMPONENT(*pkComponentType, this);

	if (pkComponent != nullptr)
	{
		m_kComponent.push_back(pkComponent);
		pkComponent->Init();
	}
}
