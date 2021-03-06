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
#include <queue>

#include "irrwrapper.hpp"
#include "bindings.hpp"

#include "regex.hpp"


std::mutex commandMutex;
std::queue<std::string> commands;

// maybe change to run lua_pcall in main thread
void ConsoleThread(lua_State* L)
{
	char command[1000];
	while (GetConsoleWindow())
	{
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		commandMutex.lock();
		commands.push(command);
		commandMutex.unlock();
	}
}



int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	std::thread conThread(ConsoleThread, L);

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);
	if (!device)
		return 1;

	device->setWindowCaption(L"Lua Irrlicht");
	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::scene::ISceneManager* smgr = device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();


	//testScene("testscene.txt");
	//loadScene("testscene.txt", device);

	addCamera(device);


	lua_pushcclosure(L, lb_snapshot, 0);
	lua_setglobal(L, "snapshot"); 

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_addMesh, 1);
	lua_setglobal(L, "addMesh");

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_addBox, 1);
	lua_setglobal(L, "addBox");

	lua_pushlightuserdata(L, smgr);
	lua_pushcclosure(L, lb_camera, 1);
	lua_setglobal(L, "camera");

	lua_pushlightuserdata(L, smgr);
	lua_pushcclosure(L, lb_getNodes, 1);
	lua_setglobal(L, "getNodes");

	lua_pushlightuserdata(L, driver);
	lua_pushcclosure(L, lb_addTexture, 1);
	lua_setglobal(L, "addTexture");

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_bind, 1);
	lua_setglobal(L, "bind");

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_loadScene, 1);
	lua_setglobal(L, "loadScene");

	lua_pushlightuserdata(L, device);
	lua_pushcclosure(L, lb_clearScene, 1);
	lua_setglobal(L, "clearScene");

	while (device->run())
	{
		bool windowActive = device->isWindowActive();
		auto cam = smgr->getActiveCamera();
		if (cam)
		{
			cam->setInputReceiverEnabled(windowActive);
		}
		device->getCursorControl()->setVisible(!windowActive);


		if (commandMutex.try_lock())
		{
			while (!commands.empty())
			{
				auto command = commands.front();
				commands.pop();
				if (luaL_loadstring(L, command.c_str()) || lua_pcall(L, 0, 0, 0))
				{
					std::cout << lua_tostring(L, -1) << '\n';
					lua_pop(L, 1);
				}
			}
			commandMutex.unlock();
		}


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