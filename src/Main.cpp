#pragma comment(lib, "Irrlicht.lib")
#ifdef _DEBUG
#pragma comment(lib, "LuaLibd.lib")
#else
#pragma comment(lib, "Lualib.lib")
#endif

#include <lua.hpp>
#include <Windows.h>
#include <iostream>
#include <thread>
#include "lua.hpp"
#include <irrlicht.h>

#include <vector>
#include <numeric>
#include <optional>
#include <string>
#include <mutex>
#include <cassert>


void ConsoleThread(lua_State* L)
{
	char command[1000];
	while (GetConsoleWindow())
	{
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		if (luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0))
			std::cout << lua_tostring(L, -1) << '\n';
	}
}


int generateGUID()
{
	static int incrementor = 0;
	return incrementor++;
}



irr::scene::SMesh* createMesh(std::vector<irr::core::vector3df> vertices, std::vector<irr::core::vector2df> texCoords)
{ 

	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();

	int size = vertices.size() - vertices.size() % 3;

	assert(vertices.size() == texCoords.size() || texCoords.empty());

	buffer->Vertices.set_used(size);
	for (int i = 0; i < size; i++)
	{
		auto& vert = buffer->Vertices[i];
		vert.Pos.set(vertices[i]);
		//vert.Normal.set(0, 1, 0);
		vert.Color.set(255, 255, 255, 255);
		if (!texCoords.empty())
		{
			vert.TCoords.set(texCoords[i]);
		}
		else
		{
			switch (i % 3)
			{
			case 0:
				vert.TCoords.set(0, 0);
				break;

			case 1:
				{
					auto first = buffer->Vertices[i - 1].Pos;
					auto diff = first - vert.Pos;
					float length = diff.getLength();
					vert.TCoords.set(length, 0);
					break;
				}
			case 2:
				{
					auto first = buffer->Vertices[i - 2].Pos;
					auto diff = first - vert.Pos;
					float length = diff.getLength();
					vert.TCoords.set(0, length);
					break;
				}
			}
		}

	}

	buffer->Indices.set_used(size);
	for (int i = 0; i < size; i++)
	{
		buffer->Indices[i] = i;
	}

	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();
	mesh->setDirty();
	return mesh;
}



void addMesh(irr::IrrlichtDevice* device, std::vector<irr::core::vector3df> vertices, std::vector<irr::core::vector2df> texCoords, std::optional<std::string> name = std::nullopt)
{
	int id = generateGUID();
	if (!name)
	{
		name = "mesh_" + std::to_string(id);
	}
	//std::cout << name.value() << "\n";

	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::scene::ISceneManager* smgr = device->getSceneManager();

	auto meshNode = smgr->addMeshSceneNode(createMesh(vertices, texCoords), 0, id);
	meshNode->setName(name.value().c_str());

	meshNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	//meshNode->setMaterialTexture(0, driver->getTexture("../Meshes/sydney.bmp"));
}


void addBox(irr::IrrlichtDevice* device, irr::core::vector3df pos, float size, std::optional<std::string> name = std::nullopt)
{
	std::vector<irr::core::vector3df> vertices;
	vertices.emplace_back(-1, -1, -1);
	vertices.emplace_back(1, 1, -1);
	vertices.emplace_back(-1, 1, -1);
	vertices.emplace_back(-1, -1, -1);
	vertices.emplace_back(1, -1, -1);
	vertices.emplace_back(1, 1, -1);

	irr::core::matrix4 mat;
	mat.setRotationDegrees(irr::core::vector3df(0, 90.f, 0));

	vertices.resize(6 * 6);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			auto vert = vertices[i * 6 + j];
			mat.rotateVect(vert);
			vertices[(i + 1) * 6 + j] = vert;
		}
	}
	mat.setRotationDegrees(irr::core::vector3df(0, 0, 90));
	for (int j = 0; j < 6; j++)
	{
		auto vert = vertices[3 * 6 + j];
		mat.rotateVect(vert);
		vertices[4 * 6 + j] = vert;
	}
	for (int j = 0; j < 6; j++)
	{
		auto vert = vertices[4 * 6 + j];
		mat.rotateVect(vert);
		mat.rotateVect(vert);
		vertices[5 * 6 + j] = vert;
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = size * vertices[i] / 2 + pos;
	}

	std::vector<irr::core::vector2df> texCoords;
	texCoords.resize(6 * 6);
	for (int i = 0; i < 6*6; i += 6)
	{
		texCoords[i].set(0, 0);
		texCoords[i+1].set(1, 1);
		texCoords[i+2].set(0, 1);
		texCoords[i + 3].set(0, 0);
		texCoords[i + 4].set(1, 0);
		texCoords[i + 5].set(1, 1);
	}

	addMesh(device, vertices, texCoords, name);
}


struct NodeInfo
{
	int id;
	std::string name;
	std::string type;
};

std::vector<NodeInfo> getNodes(irr::scene::ISceneManager* smgr)
{
	std::vector<NodeInfo> result;

	auto root = smgr->getRootSceneNode();
	auto& children = root->getChildren();

	for (auto& child : children)
	{

		NodeInfo info;
		info.id = child->getID();
		info.name = child->getName();
		result.push_back(info);
	}
	return result;
}

std::mutex snapshotsLock;
std::vector<std::string> snapshots;
void snapshot(const std::string filename)
{
	snapshotsLock.lock();

	snapshots.push_back(filename);

	snapshotsLock.unlock();
}



static int lc_snapshot(lua_State* L)
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

static int lc_camera(lua_State* L)
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
			return luaL_argerror(L, 2, "target contains non number");

		setVec(target, i, lua_tonumber(L, -1));
		lua_pop(L, 1);


		lua_rawgeti(L, -2, i + 1);
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 1, "eye contains non number");

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


static int lc_addBox(lua_State* L)
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
		return luaL_argerror(L, 2, "argument not number");

	float boxSize = lua_tonumber(L, 2);
	irr::core::vector3df pos;

	for (int i = 0; i < 3; i++)
	{
		lua_rawgeti(L, 1, i + 1);
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 1, "position contains non number");

		setVec(pos, i, lua_tonumber(L, -1));
		lua_pop(L, 1);
	}

	irr::IrrlichtDevice* device = static_cast<irr::IrrlichtDevice*>(
		lua_touserdata(L, lua_upvalueindex(1))
	);

	if (numArgs == 3)
	{
		if(!lua_isstring(L, 3))
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

static int lc_getNodes(lua_State* L)
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

int main()
{

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	std::thread conThread(ConsoleThread, L);

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(1280, 720), 16, false, false, true, 0);
	if (!device)
		return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::scene::ISceneManager* smgr = device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);


	irr::SKeyMap keys[4];
	keys[0].Action = irr::EKA_MOVE_FORWARD;
	keys[0].KeyCode = irr::KEY_KEY_W;

	keys[1].Action = irr::EKA_MOVE_BACKWARD;
	keys[1].KeyCode = irr::KEY_KEY_S;

	keys[2].Action = irr::EKA_STRAFE_LEFT;
	keys[2].KeyCode = irr::KEY_KEY_A;

	keys[3].Action = irr::EKA_STRAFE_RIGHT;
	keys[3].KeyCode = irr::KEY_KEY_D;

	auto cam = smgr->addCameraSceneNodeFPS(0, 100.f, 0.02f, generateGUID(), keys, 4);
	cam->setName("camera");


	lua_pushcclosure(L, lc_snapshot, 0);
	lua_setglobal(L, "snapshot"); 

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lc_addBox, 1);
	lua_setglobal(L, "addBox");
	
	lua_pushlightuserdata(L, cam);
	lua_pushcclosure(L, lc_camera, 1);
	lua_setglobal(L, "camera");

	lua_pushlightuserdata(L, smgr);
	lua_pushcclosure(L, lc_getNodes, 1);
	lua_setglobal(L, "getNodes");


	while (device->run())
	{
		bool windowActive = device->isWindowActive();
		cam->setInputReceiverEnabled(windowActive);
		device->getCursorControl()->setVisible(!windowActive);


		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();

		
		if (snapshotsLock.try_lock())
		{
			for (int i = 0; i < snapshots.size(); i++)
			{
				//std::cout << "Creating snapshot '" << snapshots[i] << "'\n";
				auto img = driver->createScreenShot();
				if (!(img && driver->writeImageToFile(img, snapshots[i].c_str())))
				{
					std::cerr << "ERROR: Cannot create snapshot '"  <<  snapshots[i] << "'\n";
					
				}
			}
			snapshots.clear();

			snapshotsLock.unlock();
		}
	}

	device->drop();

	conThread.join();

	return 0;
}