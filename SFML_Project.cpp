// SFML_Project.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "OGLApplication.h"
#include "SinningCubeApplication.h"
#include "TestVesselApplication.h"
//#include "CEGUITutorial.h"
#include "GameObjectApplication.h"

// A pure virtual base class
struct BaseClass
{
	virtual void sayType() = 0;
};

// A class derived from BaseClass
struct DerivedClassOne : public BaseClass
{
	void sayType();

	int x;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(x);
	}
};

// Another class derived from BaseClass
struct EmbarrassingDerivedClass : public BaseClass
{
	void sayType();

	float y;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(y);
	}
};

// Include any archives you plan on using with your type before you register it
// Note that this could be done in any other location so long as it was prior
// to this file being included
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

// Register DerivedClassOne
CEREAL_REGISTER_TYPE(DerivedClassOne);

// Register EmbarassingDerivedClass with a less embarrasing name
CEREAL_REGISTER_TYPE_WITH_NAME(EmbarrassingDerivedClass, "DerivedClassTwo");

// Note that there is no need to register the base class, only derived classes
//  However, since we did not use cereal::base_class, we need to clarify
//  the relationship (more on this later)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, DerivedClassOne)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseClass, EmbarrassingDerivedClass)
void DerivedClassOne::sayType()
{
	std::cout << "DerivedClassOne" << std::endl;
}

void EmbarrassingDerivedClass::sayType()
{
	std::cout << "EmbarrassingDerivedClass. Wait.. I mean DerivedClassTwo!" << std::endl;
}

int main()
{
	//OGLApplication app;
	//SpinningCubeApplication app;
	//CEGUITutorial app;
	GameObjectApplication app;

	app.MainLoop();

	return 0;
}

