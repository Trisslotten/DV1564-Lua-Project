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
	mesh->setDirty();
	mesh->recalculateBoundingBox();
	return mesh;
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

	irr::video::SMaterial bogdan;

	std::vector<irr::core::vector3df> vertices;
	for (int i = 0; i < 3 * 1; i++)
	{
		float x = 2.f*float(rand()) / RAND_MAX;
		float y = 2.f*float(rand()) / RAND_MAX;
		float z = 2.f*float(rand()) / RAND_MAX;
		vertices.emplace_back(x,y,z);
	}

	auto meshNode = smgr->addMeshSceneNode(createMesh(vertices));

	meshNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	meshNode->setMaterialTexture(0, driver->getTexture("../Meshes/sydney.bmp"));


	irr::SKeyMap keys[4];
	keys[0].Action = irr::EKA_MOVE_FORWARD;
	keys[0].KeyCode = irr::KEY_KEY_W;

	keys[1].Action = irr::EKA_MOVE_BACKWARD;
	keys[1].KeyCode = irr::KEY_KEY_S;

	keys[2].Action = irr::EKA_STRAFE_LEFT;
	keys[2].KeyCode = irr::KEY_KEY_A;

	keys[3].Action = irr::EKA_STRAFE_RIGHT;
	keys[3].KeyCode = irr::KEY_KEY_D;

	auto cam = smgr->addCameraSceneNodeFPS(0, 100.f, 0.02f, -1, keys, 4);
	

	while (device->run())
	{
		bool windowActive = device->isWindowActive();
		cam->setInputReceiverEnabled(windowActive);
		device->getCursorControl()->setVisible(!windowActive);


		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();

	}

	device->drop();

	conThread.join();
	return 0;
}