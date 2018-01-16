#pragma once
#include "RTTI.h"
class IComponent;
class GameObject;
class TypeManager
{
	//singleton for test
public:
	typedef Delegate<std::shared_ptr<IComponent>(GameObject*)> FactoryFunction;///< Standard factory delegate

	/**
	\brief Singleton access method
	*/
	static TypeManager& Instance();
	
	//real type manager beef
public:
	//~TypeManager() {}

	/**
	\brief Register factory function with a type
	\param rkType Type to which the factory is associated
	\param kFunction Factory delegate that creates the object
	*/
	bool RegisterFactory(const RTTI& rkType, FactoryFunction kFunction);
	
	/**
	\brief Removes the factory function associated to RTTI
	\param rkType Type which we need to the remove the factory function
	*/
	bool RemoveFactory(const RTTI& rkType);

	/**
	\brief Access the factory
	\param rkTypeName The name of the type we are looking for
	\return The factory function associated to the request
	\sa{GetFactory, GetFactory}
	*/
	const FactoryFunction* GetFactory(const std::string& rkTypeName);
	/**
	\brief Access the factory
	\param rkType The type we are looking for
	\return The factory function associated to the request
	\sa{GetFactory, GetFactory}
	*/
	const FactoryFunction* GetFactory(const RTTI& rkType);

	/**
	\brief Access the factory
	\param riTypeID The ID of the type we are looking for
	\return The factory function associated to the request
	\sa{GetFactory, GetFactory}
	*/
	const FactoryFunction* GetFactory(const I32& riTypeID);

	/**
	\brief Registers a type without factory
	\param rkType The type to register
	*/
	bool RegisterType(const RTTI& rkType);

	/**
	\brief Access the type
	\param rkTypeName The name of the type we are looking for
	\return The type we are looking for
	\sa{GetType}
	*/
	const RTTI* GetType(const std::string& rkTypeName);

	/**
	\brief Access the type
	\param riTypeID The ID of the type we are looking for
	\return The type we are looking for
	\sa{GetType}
	*/
	const RTTI* GetType(const I32& riTypeID);



	/**
	\brief returns a list of constructible derived types
	\param rkHeadType Head inheritance type
	\param rkTypesList an std::list that contains pointers to the types instances that derived from type passed as parameter
	*/
	void GetAllContructibleDerivedTypes(const RTTI& rkHeadType, std::list<RTTI*>& rkTypesList);

private:
	std::unordered_map<RTTI, FactoryFunction> m_akFactoryMap;///< Links the IDs and types for easy look up.
	std::unordered_map<I32, RTTI> m_akTypes;///< Links the IDs and types for easy look up.
};

/**
\def  GET_FACTORY(x) (*TypeManager::Instance().GetFactory(x))
Macro to used to replace the horible call the get the delegate. X maybe of type RTTI, std::string, ou I32
*/
#define GET_TYPE(x) \
	TypeManager::Instance().GetType(x)

#define GET_FACTORY(x) \
		(*TypeManager::Instance().GetFactory(x))
#define CREATE_NEW_COMPONENT(x, parent) \
		(*TypeManager::Instance().GetFactory(x))(parent)
#define CREATE_NEW_OF_TYPE(type, x, parent) \
		std::dynamic_pointer_cast<type>(CREATE_NEW_COMPONENT(x, parent))



