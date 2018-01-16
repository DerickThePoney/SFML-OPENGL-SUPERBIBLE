#include "stdafx.h"
#include "StringHash.h"

#define OFFSET_BASIS 2166136261
#define FNV_PRIME 16777619
I32 StringHashID(const std::string& rkNameToHash)
{
	I32 iID = OFFSET_BASIS;
	const char* cBuffer = rkNameToHash.c_str();
	const UI32 uiLength = (UI32)rkNameToHash.length();

	for (UI32 i = 0; i < uiLength; ++i)
	{
		iID = iID ^ cBuffer[i];
		iID *= FNV_PRIME;
	}

	return iID;
}