#include "stdafx.h"
#include "TypeManager.h"


TypeManager& TypeManager::Instance()
{
	static TypeManager s_rkInstance;

	return s_rkInstance;
}


bool TypeManager::RegisterFactory(const RTTI & rkType, FactoryFunction kFunction)
{
	//lookup the type
	auto itFind = m_akTypes.find(rkType.GetID());

	if (itFind == m_akTypes.end())
	{
		//if not found, add it and add the factory
		m_akTypes[rkType.GetID()] = rkType;
		m_akFactoryMap[rkType] = kFunction;

		return true;
	}

	//even if we found it we want to register a new factory. Just log it
	//std::cerr << "WARNING: Registering an already existing type with a possibly new factory." << std::endl;
	//LOG_MESSAGE_NO_SYSTEM_LOG_LEVEL("TYPE_SYSTEM: Registering an already existing type with a possibly new factory.", LOG_LEVEL::WARNING);
	m_akFactoryMap[rkType] = kFunction;

	return false;
}

bool TypeManager::RemoveFactory(const RTTI & rkType)
{
	//lookup the type
	auto itFind = m_akTypes.find(rkType.GetID());

	if (itFind == m_akTypes.end())
	{
		//case not existant --> directly return false
		return false;
	}

	//erase the factory and type
	m_akFactoryMap.erase(rkType);
	m_akTypes.erase(itFind);

	return true;
}

const TypeManager::FactoryFunction* TypeManager::GetFactory(const std::string & rkTypeName)
{
	I32 iID = StringHashID(rkTypeName);
	auto itFind = m_akTypes.find(iID);

	if (itFind == m_akTypes.end())
	{
		//the type is non existant
		return nullptr;
	}

	return &m_akFactoryMap[itFind->second];
}

const TypeManager::FactoryFunction* TypeManager::GetFactory(const RTTI & rkType)
{
	auto itFind = m_akFactoryMap.find(rkType);

	if (itFind == m_akFactoryMap.end())
	{
		//the type is non existant
		return nullptr;
	}

	return &itFind->second;
}

const TypeManager::FactoryFunction* TypeManager::GetFactory(const I32 & riTypeID)
{
	auto itFind = m_akTypes.find(riTypeID);

	if (itFind == m_akTypes.end())
	{
		//the type is non existant
		return nullptr;
	}

	return &m_akFactoryMap[itFind->second];
}

bool TypeManager::RegisterType(const RTTI & rkType)
{
	//lookup the type
	auto itFind = m_akTypes.find(rkType.GetID());

	if (itFind == m_akTypes.end())
	{
		//if not found, add it
		m_akTypes[rkType.GetID()] = rkType;

		return true;
	}

	return false;
}

const RTTI* TypeManager::GetType(const std::string & rkTypeName)
{
	I32 iID = StringHashID(rkTypeName);
	auto itFind = m_akTypes.find(iID);

	if (itFind == m_akTypes.end())
	{
		//the type is non existant
		return nullptr;
	}

	return &itFind->second;
}

const RTTI* TypeManager::GetType(const I32 & riTypeID)
{
	auto itFind = m_akTypes.find(riTypeID);

	if (itFind == m_akTypes.end())
	{
		//the type is non existant
		return nullptr;
	}

	return &itFind->second;
}



void TypeManager::GetAllContructibleDerivedTypes(const RTTI& rkHeadType, std::list<RTTI*>& rkTypesList)
{
	for (auto it = m_akTypes.begin(); it != m_akTypes.end(); ++it)
	{
		if (it->second.IsDerivedFrom(rkHeadType) && !(it->second == rkHeadType))
		{
			//check if factory exists for this type
			auto itFind = m_akFactoryMap.find(it->second);

			if (itFind != m_akFactoryMap.end())
			{
				//found factory so add type to list
				rkTypesList.push_back(&it->second);
			}
		}
	}
}

