// SFML_Project.cpp�: d�finit le point d'entr�e pour l'application console.
//

#include "stdafx.h"
#include "OGLApplication.h"
#include "SinningCubeApplication.h"
#include "TestVesselApplication.h"
//#include "CEGUITutorial.h"
#include "GameObjectApplication.h"



int main()
{
	//OGLApplication app;
	//SpinningCubeApplication app;
	//CEGUITutorial app;
	GameObjectApplication app;

	app.MainLoop();

	return 0;
}

