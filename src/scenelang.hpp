#pragma once
#include <irrlicht.h>
#include <string>
#include <vector>
#include <iostream>
/*

DEFINITION : PROTOTYPE "{" BLOCK "}"
PROTOTYPE : IDENTIFIER "(" PARAMETERSSTART ")"
// for clean parse tree
PARAMETERSSTART : PARAM RESTPARAMETERS | empty
PARAMETERS : PARAM RESTPARAMETERS | empty
RESTPARAMETERS : "," PARAMETERS
PARAM : STRING | LUA | PROTOTYPE | empty
BLOCK : DATASTART | LUA | DECLARATIONSSTART
DATASTART: VECTOR RESTDATA
DATA : VECTOR RESTDATA | empty
RESTDATA : "," DATA | empty
DECLARATIONSSTART: PROTOTYPE DECLARATIONS
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




struct SceneVertex
{
	irr::core::vector3df position;
	irr::core::vector2df texCoords;
};

struct SceneMesh
{
	std::string name;
	bool hasTexCoords = false;
	std::vector<SceneVertex> vertices;
};


bool loadScene(const std::string& path, irr::IrrlichtDevice* d);

std::string loadFile(const std::string& path);

void testScene(const std::string& path);