#pragma once
#include <irrlicht.h>
#include <string>
#include <iostream>
/*
Texture("checkerboard") 
{ 
	(0,0,0), (1,1,1), (0,0,0), (1,1,1), (1,1,1), (0,0,0), (1,1,1), (0,0,0), (0,0,0), (1,1,1), (0,0,0), (1,1,1), (1,1,1), (0,0,0), (1,1,1), (0,0,0) 
} 

Texture("checkerboard2") 
{ 
	Lua(< ASDASDD BOGDAN >)
} 

Mesh("square") { (-20,0,-20,0,0), (20,0,-20,1,0), (-20,0,20,0,1), (20,0,-20,1,0), (20,0,20,1,1), (-20,0,20,0,1) } 

Scene()
{ 
	Bind( "checkerboard2", Mesh("bumpycyc") ) 
	Bind( "checkerboard", Transform( Lua(< local args = {...} return args[1]+10, args[2], args[3], args[4], args[5] >), Mesh("bumpycyc") ) ) 
	Bind( "checkerboard", Mesh("square") ) 
}

LANG: DEFINITION LANG | empty
DEFINITION : PROTOTYPE "{" BLOCK "}"
PROTOTYPE : IDENTIFIER "(" PARAMETERS ")"
PARAMETERS : PARAM RESTPARAMETERS | empty
RESTPARAMETERS : "," PARAMETERS
PARAM : STRING | LUA | PROTOTYPE
BLOCK : DATA | DECLARATIONS | LUA
DATA : VECTOR RESTDATA | empty
RESTDATA : "," DATA | empty
DECLARATIONS : PROTOTYPE DECLARATIONS | empty
VECTOR : "(" VECTOR2 ")"
VECTOR2 : NUM RESTVECTOR
RESTVECTOR : "," VECTOR2 | empty



regex:
IDENTIFIER
NUM
STRING
LUA
*/

bool loadScene(std::string path, irr::IrrlichtDevice* d)
{
	//Implement
	std::cout << "Hello from loadScene " << d->getTimer()->getRealTimeAndDate().Weekday << std::endl;
	return 1;
}
