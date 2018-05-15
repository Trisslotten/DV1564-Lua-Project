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
#include <string>

#include "irrwrapper.hpp"
#include "bindings.hpp"



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


	lua_pushcclosure(L, lb_snapshot, 0);
	lua_setglobal(L, "snapshot"); 

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_addMesh, 1);
	lua_setglobal(L, "addMesh");

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_addBox, 1);
	lua_setglobal(L, "addBox");
	
	lua_pushlightuserdata(L, cam);
	lua_pushcclosure(L, lb_camera, 1);
	lua_setglobal(L, "camera");

	lua_pushlightuserdata(L, smgr);
	lua_pushcclosure(L, lb_getNodes, 1);
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

		
		handleSnapshots(driver);
	}

	device->drop();

	conThread.join();

	return 0;
}