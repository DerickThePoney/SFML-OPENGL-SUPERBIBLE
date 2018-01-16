#pragma once
#include "RTTI.h"
#include "TypeManager.h"

//macro definitions
#define TYPE_SYSTEM_DECLARE_REGISTRATION(classname) \
	static bool sg_bInitialisedFactory_##classname;
	//_Pragma("comment(linker, \"/INCLUDE:sg_bInitialisedFactory_"##classname"\")")

#define TYPE_SYSTEM_DECLARE_RTTI \
public:\
	static const RTTI TYPE;\
	virtual const RTTI& GetType() const { return TYPE; }\

#define TYPE_SYSTEM_DECLARE_NON_VIRTUAL_RTTI \
public:\
	static const RTTI TYPE;\
	const RTTI& GetType() const { return TYPE; }\

#define TYPE_SYSTEM_RTTI_OBJECT_NO_BASE(classname) \
	const RTTI classname::TYPE(#classname, nullptr);\
	bool sg_bRegisteredType_##classname = \
	TypeManager::Instance().RegisterType(classname::TYPE);

#define TYPE_SYSTEM_RTTI_OBJECT(classname, baseclassname) \
	const RTTI classname::TYPE(#classname, &baseclassname::TYPE);\
	static std::shared_ptr<IComponent> Factory_##classname(GameObject* pkParent){return std::shared_ptr<IComponent>(new classname(pkParent));}\
	bool sg_bRegisteredType_##classname = \
	TypeManager::Instance().RegisterFactory(classname::TYPE, DELEGATE_FREE(Factory_##classname));

#define TYPE_SYSTEM_RTTI_OBJECT_NOBASE(classname) \
	const RTTI classname::TYPE(#classname, nullptr);\
	bool sg_bRegisteredType_##classname = \
	TypeManager::Instance().RegisterType(classname::TYPE);



