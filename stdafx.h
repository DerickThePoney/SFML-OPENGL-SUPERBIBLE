// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once

#ifdef _WIN32
#include "targetver.h"
#endif // WIN32

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stb/stb_image.h"

//#include "Macros.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>

#include "Delegate.h"

#define DEARCHIVE_WITH_DEFAULT(x, def) \
	try{archive(CEREAL_NVP(x));}catch(std::exception e){x = def;}
#define DEARCHIVE_NO_DEFAULT(x, str, def) \
	try{archive(CEREAL_NVP(x));}catch(std::exception e){}
#define DEARCHIVE_WITH_DEFAULT_FROM_NVP(x, str, def) \
	try{archive(cereal::make_nvp(str, x));}catch(std::exception e){x = def;}
#define DEARCHIVE_NO_DEFAULT_FROM_NVP(x, str, def) \
	try{archive(cereal::make_nvp(str, x));}catch(std::exception e){}
#define OGL_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

#include <iostream>
#include <sstream>
#include <SFML\Graphics.hpp>
#define GLEW_STATIC
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <map>
#include "Utility.h"
#include "Math.h"
#include "Line.h"
#include "StringHash.h"
#include "InputManager.h"

#include "imgui.h"
#include "imgui-events-SFML.h"
#include "imgui-rendering-SFML.h"

#include "MaterialUniformData.h"



// TODO: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
