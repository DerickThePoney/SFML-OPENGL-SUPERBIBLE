/**
\brief Run Time Type Identification class

This class is used for run time type identification. It contains the name of the type and its ID, as well as a link to its parent class.
This class is mainly used for factory search.
*/
class RTTI
{
public:
	RTTI();
	RTTI(const std::string &rkName, const RTTI* pkParentClass);

	/**
	\brief Equality comparison
	\param rkOther the RTTI object to compare
	\return true if the IDs are identical

	Uses directly the computed ID.
	*/
	const bool operator==(const RTTI& rkOther) const { return m_iID == rkOther.m_iID; }
	const bool operator<(const RTTI& rkOther) const { return m_iID < rkOther.m_iID; }

	/**
	\brief Checks that this object derives from the one in parameter
	\param rkOther the RTTI object to verify
	\return true if this derives from rkOther
	*/
	const bool IsDerivedFrom(const RTTI& rkOther) const;

	const I32 GetID() const;
	const std::string& GetName() const;

private:
	std::string m_kName;///< the name of this type in plain representation
	I32 m_iID; ///< the ID corresponding to m_kName
	const RTTI* m_pkParentClass; ///< a pointer to the parent class
};



namespace std
{
	template<>
	class hash<RTTI>
	{
	public:
		size_t operator()(const RTTI& rkOther) const
		{
			return rkOther.GetID();
		}
	};
}
#pragma once
