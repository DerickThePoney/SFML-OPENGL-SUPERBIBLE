#pragma once
#include "TypeSystem.h"

#define ICOMPONENTS_RTTI(classname) \
	static std::string classname;

class GameObject;
class IComponent
{
	TYPE_SYSTEM_DECLARE_RTTI;
protected:
	IComponent(GameObject* pkParent) : m_pkParent(pkParent){}

public:
	virtual void Init() {}
	virtual void LateInit() {}
	virtual void Update(double deltaTime) {}
	virtual void OnPreRender() {}
	virtual void Render() {}

	virtual void Inspect() = 0;
	virtual void Clone(std::shared_ptr<IComponent> pkComponent) = 0;

public:
	GameObject* m_pkParent;
};

typedef std::shared_ptr<IComponent> IComponentPtr;

#define ADDCOMPONENT(x) #x

static const C8* Components[] =
{
	ADDCOMPONENT(MeshRendererComponent),
	ADDCOMPONENT(LightComponent),
	ADDCOMPONENT(TestPBRMaterialComponent),
	ADDCOMPONENT(FreeCameraComponent),
	ADDCOMPONENT(TerrainComponent)
};
