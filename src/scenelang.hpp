#pragma once
#include <irrlicht.h>
#include <string>
#include <iostream>
/*
Mesh("tri") {
	(-20,0,-20), 
	(20,0,-20), 
	(-20,0,20), 
}
Scene() 
{ 
	Mesh("tri") 
}

LANG: DEFINITION LANG | empty

DEFINITION: PROTOTYPE "{" BLOCK "}"
PROTOTYPE: IDENTIFIER "(" PARAMETERS ")"

PARAMETERS: PARAM RESTPARAMETERS | empty
RESTPARAMETERS: "," PARAMETERS

BLOCK: DATA | DECLERATIONS

DATA: VECTOR RESTDATA | empty
RESTDATA: "," DATA | empty

DECLERATIONS: PROTOTYPE DECLERATIONS | empty

// could cause problems if ">)" is in a string in lua code
LUA: "Lua(<" ANY ">)" 

VECTOR: "(" VECTOR2 ")"
VECTOR2: NUM RESTVECTOR
RESTVECTOR: "," VECTOR2 | empty

*/

bool loadScene(std::string path, irr::IrrlichtDevice* d)
{
	//Implement
	std::cout << "Hello from loadScene " << d->getTimer()->getRealTimeAndDate().Weekday << std::endl;
	return 1;
}
