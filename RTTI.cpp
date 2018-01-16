#include "stdafx.h"
#include "RTTI.h"


RTTI::RTTI()
	:m_kName(""), m_pkParentClass(nullptr)
{
	m_iID = StringHashID(m_kName);
}

RTTI::RTTI(const std::string & rkName, const RTTI* pkParentClass)
	: m_kName(rkName)
{
	m_pkParentClass = pkParentClass;
	m_iID = StringHashID(rkName);
}


const bool RTTI::IsDerivedFrom(const RTTI & rkOther) const
{
	const RTTI* pkSearch = this;
	while (pkSearch != nullptr)
	{
		if (*pkSearch == rkOther)
		{
			return true;
		}
		pkSearch = pkSearch->m_pkParentClass;
	}

	return false;
}

const I32 RTTI::GetID() const
{
	return m_iID;
}

const std::string & RTTI::GetName() const
{
	return m_kName;
}

