// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#ifdef _WIN32
#include "targetver.h"
#endif // WIN32

//#include "Macros.h"
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
#include "vmath.h"

#include "imgui.h"
#include "imgui-events-SFML.h"
#include "imgui-rendering-SFML.h"



// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme
