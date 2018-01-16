// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#ifdef _WIN32
#include "targetver.h"
#endif // WIN32

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

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

#include "imgui.h"
#include "imgui-events-SFML.h"
#include "imgui-rendering-SFML.h"



// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme
