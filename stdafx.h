// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
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



// TODO: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
