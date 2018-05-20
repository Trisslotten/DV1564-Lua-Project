#include "irrwrapper.hpp"

#include <irrlicht.h>
#include <vector>
#include <iostream>
#include <cassert>
#include <optional>
#include <string>
#include <mutex>


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



void addMesh(irr::IrrlichtDevice* device, std::vector<irr::core::vector3df> vertices, std::vector<irr::core::vector2df> texCoords, std::optional<std::string> name)
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


void addBox(irr::IrrlichtDevice* device, irr::core::vector3df pos, float size, std::optional<std::string> name)
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
	for (int i = 0; i < 6 * 6; i += 6)
	{
		texCoords[i].set(0, 0);
		texCoords[i + 1].set(1, 1);
		texCoords[i + 2].set(0, 1);
		texCoords[i + 3].set(0, 0);
		texCoords[i + 4].set(1, 0);
		texCoords[i + 5].set(1, 1);
	}

	addMesh(device, vertices, texCoords, name);
}




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


std::vector<std::string> snapshots;
void snapshot(const std::string filename)
{
	snapshots.push_back(filename);
}

void handleSnapshots(irr::video::IVideoDriver* driver)
{
	for (int i = 0; i < snapshots.size(); i++)
	{
		//std::cout << "Creating snapshot '" << snapshots[i] << "'\n";
		auto img = driver->createScreenShot();
		if (!(img && driver->writeImageToFile(img, snapshots[i].c_str())))
		{
			std::cerr << "ERROR: Cannot create snapshot '" << snapshots[i] << "'\n";
		}
	}
	snapshots.clear();
}

void addTexture(irr::video::IVideoDriver* driver, const std::string& name, std::vector<uint8_t> data, int width, int height)
{
	auto img = driver->createImageFromData(irr::video::ECF_R8G8B8, irr::core::dimension2du(width, height), &data[0]);
	if (!img)
	{
		std::cout << "Error: Could not create image from data\n";
		return;
	}
	auto tex = driver->addTexture(name.c_str(), img);
}

void mybind(irr::IrrlichtDevice * device, const std::string & nodeName, const std::string & texture)
{
	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::scene::ISceneManager* smgr = device->getSceneManager();
	auto root = smgr->getRootSceneNode();
	auto& children = root->getChildren();

	irr::scene::ISceneNode* node = nullptr;
	for (auto child : children)
	{
		if (child->getName() == nodeName)
		{
			node = child;
			break;
		}
	}
	if (!node)
	{
		std::cerr << "Could not find node '" << nodeName << "'\n";
		return;
	}

	auto tex = driver->getTexture(texture.c_str());
	if (!tex)
	{
		std::cerr << "Could not find texture '" << texture << "'\n";
		return;
	}

	node->setMaterialTexture(0, tex);
	auto& mat = node->getMaterial(0);
	mat.setFlag(irr::video::EMF_BILINEAR_FILTER, false);
}

void clearScene(irr::IrrlichtDevice * device)
{
	auto driver = device->getVideoDriver();
	auto smgr = device->getSceneManager();

	driver->removeAllTextures();

	auto root = smgr->getRootSceneNode();
	root->removeAll();
}



