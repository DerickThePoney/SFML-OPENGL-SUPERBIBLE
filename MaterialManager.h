#pragma once
#include "Singleton.h"
#include "Material.h"

class MaterialManager: public Singleton<MaterialManager>
{
	friend class Singleton<MaterialManager>;
protected:
	MaterialManager();
	~MaterialManager();
	
public:
	Material* Instantiate();
	Material* InstantiateFromFile(const std::string& kFilename);
	bool Destroy(Material* pkObj);

	Material* FindFromID(UI32 uiID);

private:
	void ReleaseRessourceFromID(const UI32& id);

private:
	std::unordered_map<UI32, Material*> m_akMaterials;
	std::map<std::string, UI32> m_akRessourceToID;
};

