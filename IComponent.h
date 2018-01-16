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
	virtual void Update(double deltaTime) {}
	virtual void Render() {}

	virtual void Inspect() = 0;

public:
	GameObject* m_pkParent;
};
