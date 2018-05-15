#pragma once

#include "lua.hpp"
#include <iostream>
#include <irrlicht.h>
#include <vector>
#include "irrwrapper.hpp"



void setVec(irr::core::vector3df& vec, int i, float value)
{
	switch (i)
	{
	case 0:
		vec.X = value;
		break;
	case 1:
		vec.Y = value;
		break;
	case 2:
		vec.Z = value;
		break;
	}
}

static int lb_snapshot(lua_State* L)
{
	if (lua_gettop(L) != 1)
		return luaL_error(L, "expected single argument");

	if (!lua_isstring(L, -1))
		return luaL_argerror(L, 1, "argument not string");

	auto path = lua_tostring(L, -1);
	std::cout << path << "\n";

	snapshot(path);

	return 0;
}


static int lb_camera(lua_State* L)
{
	if (lua_gettop(L) != 2)
		return luaL_error(L, "Error: expected two argument");

	for (int i = 0; i < 2; i++)
		if (!lua_istable(L, i + 1))
			return luaL_argerror(L, i + 1, "argument not table");


	lua_len(L, 1);
	int tableSize = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if (tableSize != 3)
		return luaL_argerror(L, 1, "eye not 3d coordinate");


	lua_len(L, 2);
	tableSize = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if (tableSize != 3)
		return luaL_argerror(L, 2, "target not 3d coordinate");


	irr::core::vector3df eye;
	irr::core::vector3df target;


	for (int i = 0; i < 3; i++)
	{
		lua_rawgeti(L, -1, i + 1);
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 2, "target contains non-numeric");

		setVec(target, i, lua_tonumber(L, -1));
		lua_pop(L, 1);


		lua_rawgeti(L, -2, i + 1);
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 1, "eye contains non-numeric");

		setVec(eye, i, lua_tonumber(L, -1));
		lua_pop(L, 1);
	}

	irr::scene::ICameraSceneNode* cam = static_cast<irr::scene::ICameraSceneNode*>(
		lua_touserdata(L, lua_upvalueindex(1))
		);

	cam->setPosition(eye);
	cam->setTarget(target);

	return 0;
}

static int lb_addMesh(lua_State* L)
{
	if (lua_gettop(L) != 1)
		return luaL_error(L, "Error: single argument");

	if (!lua_istable(L, 1))
		return luaL_argerror(L, 1, "argument not table");


	lua_len(L, 1);
	int numVertices = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if (numVertices == 0)
		return luaL_argerror(L, 1, "no vertices");
	if (numVertices % 3 != 0)
		return luaL_argerror(L, 1, "number of vertices not multiple of 3");


	std::vector<irr::core::vector3df> vertices;

	for (int i = 0; i < numVertices; i++)
	{
		lua_rawgeti(L, 1, i + 1);
		if (!lua_istable(L, -1))
			return luaL_argerror(L, 1, "vertex not a table");

		lua_len(L, -1);
		int numCoords = lua_tonumber(L, -1);
		lua_pop(L, 1);

		if (numCoords != 3)
			return luaL_argerror(L, 1, "vertex not 3d coord");

		irr::core::vector3df vec;
		for (int j = 0; j < 3; j++)
		{
			lua_rawgeti(L, -1, j + 1);
			if (!lua_isnumber(L, -1))
				return luaL_argerror(L, 1, "coord non-numeric");

			setVec(vec, j, lua_tonumber(L, -1));
			lua_pop(L, 1);
		}
		vertices.push_back(vec);
	}

	auto* device = static_cast<irr::IrrlichtDevice*>(
		lua_touserdata(L, lua_upvalueindex(1))
		);

	addMesh(device, vertices);

	return 0;
}

static int lb_addBox(lua_State* L)
{
	int numArgs = lua_gettop(L);
	if (numArgs < 2 || numArgs > 3)
		return luaL_error(L, "Error: expected two or three argument");

	if (!lua_istable(L, 1))
		return luaL_argerror(L, 1, "argument not table");

	lua_len(L, 1);
	int tableSize = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if (tableSize != 3)
		return luaL_argerror(L, 1, "position not 3d coordinate");

	if (!lua_isnumber(L, 2))
		return luaL_argerror(L, 2, "argument non-numeric");

	float boxSize = lua_tonumber(L, 2);
	irr::core::vector3df pos;

	for (int i = 0; i < 3; i++)
	{
		lua_rawgeti(L, 1, i + 1);
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 1, "position contains non-numeric");

		setVec(pos, i, lua_tonumber(L, -1));
		lua_pop(L, 1);
	}

	irr::IrrlichtDevice* device = static_cast<irr::IrrlichtDevice*>(
		lua_touserdata(L, lua_upvalueindex(1))
		);

	if (numArgs == 3)
	{
		if (!lua_isstring(L, 3))
			return luaL_argerror(L, 3, "argument not string");

		std::string name = lua_tostring(L, 3);
		addBox(device, pos, boxSize, name);
	}
	else
	{
		addBox(device, pos, boxSize);
	}

	return 0;
}

static int lb_getNodes(lua_State* L)
{
	// remove arguments from stack if any
	lua_pop(L, lua_gettop(L));

	auto smgr = static_cast<irr::scene::ISceneManager*>(
		lua_touserdata(L, lua_upvalueindex(1))
		);

	auto nodes = getNodes(smgr);

	// nodes table
	lua_newtable(L);

	for (int i = 0; i < nodes.size(); i++)
	{
		lua_newtable(L);
		lua_pushstring(L, "id");
		lua_pushnumber(L, nodes[i].id);
		lua_settable(L, 2);

		lua_pushstring(L, "name");
		lua_pushstring(L, nodes[i].name.c_str());
		lua_settable(L, 2);

		lua_pushstring(L, "type");
		lua_pushstring(L, nodes[i].type.c_str());
		lua_settable(L, 2);

		// add created table to the nodes table
		lua_rawseti(L, 1, i + 1);
	}

	return 1;
}


static int lb_addTexture(lua_State* L)
{
	int numArgs = lua_gettop(L);
	if (numArgs != 1 && numArgs != 2)
		return luaL_error(L, "Error: expected 1 or 2 arguments");

	if (!lua_istable(L, 1))
		return luaL_argerror(L, 1, "not a table");

	int width = 0;
	int height = 0;
	std::vector<uint8_t> img;


}