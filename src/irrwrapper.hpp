#pragma once

#include <irrlicht.h>
#include <vector>
#include <cassert>
#include <optional>
#include <string>
#include <mutex>

int generateGUID();

irr::scene::SMesh* createMesh(std::vector<irr::core::vector3df> vertices, std::vector<irr::core::vector2df> texCoords);

void addMesh(irr::IrrlichtDevice* device, std::vector<irr::core::vector3df> vertices, std::vector<irr::core::vector2df> texCoords = {}, std::optional<std::string> name = std::nullopt);

void addBox(irr::IrrlichtDevice* device, irr::core::vector3df pos, float size, std::optional<std::string> name = std::nullopt);

struct NodeInfo
{
	int id;
	std::string name;
	std::string type;
};

std::vector<NodeInfo> getNodes(irr::scene::ISceneManager* smgr);

void snapshot(const std::string filename);

void handleSnapshots(irr::video::IVideoDriver* driver);