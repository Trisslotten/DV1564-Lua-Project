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



irr::scene::SMesh* createMesh(std::vector<irr::core::vector3df> vertices)
{ 

	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();

	int size = vertices.size() - vertices.size() % 3;

	buffer->Vertices.set_used(size);
	for (int i = 0; i < size; i++)
	{
		auto& vert = buffer->Vertices[i];
		vert.Pos.set(vertices[i]);
		//vert.Normal.set(0, 1, 0);
		//vert.Color.set(255, 255, 0, 0);
		vert.TCoords.set(vertices[i].X, vertices[i].Y);
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



void addMesh(irr::IrrlichtDevice* device, std::vector<irr::core::vector3df> vertices, std::optional<std::string> name = std::nullopt)
{
	int id = generateGUID();
	if (!name)
	{
		name = "mesh_" + std::to_string(id);
	}
	//std::cout << name.value() << "\n";

	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::scene::ISceneManager* smgr = device->getSceneManager();

	auto meshNode = smgr->addMeshSceneNode(createMesh(vertices), 0, id);
	meshNode->setName(name.value().c_str());

	meshNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	meshNode->setMaterialTexture(0, driver->getTexture("../Meshes/sydney.bmp"));
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
	addMesh(device, vertices, name);
}


struct NodeInfo
{
	int id;
	std::string name;
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
	{
		return luaL_error(L, "expected single argument");
	}
	if (!lua_isstring(L, -1))
	{
		return luaL_argerror(L, 1, "Argument not string");
	}

	auto path = lua_tostring(L, -1);
	std::cout << path << "\n";

	snapshot(path);

	return 0;
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


	for (int i = 0; i < 5; i++)
	{
		std::vector<irr::core::vector3df> vertices;
		for (int j = 0; j < 3 * 1; j++)
		{
			float x = 2.f*float(rand()) / RAND_MAX + i*2;
			float y = 2.f*float(rand()) / RAND_MAX;
			float z = 2.f*float(rand()) / RAND_MAX;
			vertices.emplace_back(x,y,z);
		}
		addMesh(device, vertices);
	}

	addBox(device, irr::core::vector3df(1,9,3), 3.f);


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

	auto nodes = getNodes(smgr);
	for (auto info : nodes)
	{
		std::cout << "Name: " << info.name << ", ID: " << info.id << "\n";
	}
	

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lc_snapshot, 1);
	lua_setglobal(L, "snapshot"); 
	

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