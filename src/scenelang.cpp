#include "scenelang.hpp"

#include <fstream>
#include <vector>
#include <list>
#include <map>
#include "irrwrapper.hpp"
#include "regex.hpp"
#include "misc.hpp"

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

namespace
{
	class Tree
	{
	public:
		std::string lexeme, tag;
		std::list < Tree * > children;
		Tree(std::string t, const char * l, int size) : tag(t), lexeme(l, size) {}

		~Tree()
		{
			for (auto c : children)
			{
				delete c;
			}
		}

		// probably wont work with \n existing in input
		void dump(std::vector<int> connections = {})
		{
			for (int i = 0; i < connections.size(); i++)
				if (i == connections.size() - 1)
					std::cout << char(179) << "  ";
				else if (connections[i] <= 0)
					std::cout << "   ";
				else
					std::cout << char(179) << "  ";
			if (connections.size() > 0)
				std::cout << "\n";

			for (int i = 0; i < connections.size(); i++)
				if (i == connections.size() - 1)
					if (connections[i] <= 0)
						std::cout << char(192) << char(196) << char(196);
					else
						std::cout << char(195) << char(196) << char(196);
				else if (connections[i] <= 0)
					std::cout << "   ";
				else
					std::cout << char(179) << "  ";
			std::cout << tag << ": " << lexeme << "\n";

			connections.push_back(children.size());

			int size = connections.size();
			for (auto child : children)
			{
				connections[size - 1] -= 1;
				child->dump(connections);
			}
		}
	};

	const char* input;
	const char* beginning;
}
bool TERM(const char *lit, Tree** result);
bool DEFINITION(Tree** result);
bool PROTOTYPE(Tree** result);
bool PARAMETERSSTART(Tree** result);
bool PARAMETERS(Tree** result);
bool RESTPARAMETERS(Tree** result);
bool PARAM(Tree** result);
bool BLOCK(Tree** result);
bool DATASTART(Tree** result);
bool DATA(Tree** result);
bool RESTDATA(Tree** result);
bool DECLARATIONSSTART(Tree** result);
bool DECLARATIONS(Tree** result);
bool VECTOR(Tree** result);
bool VECTOR2(Tree** result);
bool RESTVECTOR(Tree** result);

bool LUA(Tree** result);
bool NUM(Tree** result);
bool STRING(Tree** result);
bool IDENTIFIER(Tree** result);


void consumeWhitespace()
{
	int consumed = matchWhitespace(input);
	if (consumed > 0)
	{
		
		input += consumed;
	}
}


bool TERM(const char *lit, Tree** result)
{
	int i;
	for (i = 0; lit[i] != 0; i++)
		if (input[i] != lit[i])
			return false;
	//*result = new Tree("TERM", input, i);

	input += i;
	//std::cout << "TERM('" << lit << "')\n";

	consumeWhitespace();

	return true;
}

bool DEFINITION(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	Tree* c3 = nullptr;
	Tree* c4 = nullptr;
	const char* start = input;
	if (PROTOTYPE(&c1) && TERM("{", &c2) && BLOCK(&c3) && TERM("}", &c4))
	{
		*result = new Tree("DEFINITION", start, input - start);
		(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		(*result)->children.push_back(c3);
		//(*result)->children.push_back(c4);
		//std::cout << "DEFINITION " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	delete c3;
	delete c4;

	return false;
}

bool PROTOTYPE(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	Tree* c3 = nullptr;
	Tree* c4 = nullptr;
	const char* start = input;
	if (IDENTIFIER(&c1) && TERM("(", &c2) && PARAMETERSSTART(&c3) && TERM(")", &c4))
	{
		*result = new Tree("PROTOTYPE", start, input - start);
		(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		(*result)->children.push_back(c3);
		//(*result)->children.push_back(c4);
		//std::cout << "PROTOTYPE" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	delete c3;
	delete c4;

	return false;
}


bool PARAMETERSSTART(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	const char* start = input;
	*result = new Tree("PARAMETERS", start, input - start);
	if (PARAM(result) && RESTPARAMETERS(result))
	{
		//(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "PARAMETERS" << " " << input << "\n";

		//(*result)->children.reverse();
		return true;
	}
	
	delete c1;
	delete c2;
	return false;
}

bool PARAMETERS(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	const char* start = input;
	if (PARAM(result) && RESTPARAMETERS(result))
	{
		//*result = new Tree("PARAMETERS", start, input - start);
		//(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "PARAMETERS" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	return false;
}

bool RESTPARAMETERS(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (TERM(",", &c1) && PARAMETERS(result))
	{
		//*result = new Tree("RESTPARAMETERS", start, input - start);
		//(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "RESTPARAMETERS" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	input = start;
	//*result = new Tree("RESTPARAMETERS", start, input - start);
	return true;
}

bool PARAM(Tree** result)
{
	Tree* c = nullptr;
	auto start = input;
	if (STRING(&c) || LUA(&c) || PROTOTYPE(&c))
	{
		//*result = new Tree("PARAM", start, input - start);
		(*result)->children.push_back(c);
		//std::cout << "PARAM" << " " << input << "\n";
		return true;
	}
	delete c;
	input = start;
	*result = new Tree("PARAM", start, input - start);
	return true;
}

bool BLOCK(Tree** result)
{
	Tree* c = nullptr;
	auto start = input;
	// check lua before decl "Lua(" matches start of decl
	if (DATASTART(&c) || LUA(&c) || DECLARATIONSSTART(&c))
	{
		*result = new Tree("BLOCK", start, input - start);
		(*result)->children.push_back(c);
		//std::cout << "BLOCK" << " " << input << "\n";
		return true;
	}
	delete c;
	return false;
}


bool DATASTART(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	*result = new Tree("DATA", start, input - start);
	if (VECTOR(&c1) && RESTDATA(result))
	{
		// *result = new Tree("DATASTART", start, input - start);
		(*result)->children.push_back(c1);

		(*result)->children.reverse();
		//(*result)->children.push_back(c2);
		//std::cout << "DATA" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	return false;
}

bool DATA(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (VECTOR(&c1) && RESTDATA(result))
	{
		(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "DATA" << " " << input << "\n";
		return true;
	}
	input = start;
	//*result = new Tree("DATA", start, input - start);
	delete c1;
	delete c2;
	return true;
}

bool RESTDATA(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (TERM(",", &c1) && DATA(result))
	{
		//*result = new Tree("RESTDATA", start, input - start);
		//(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "RESTDATA" << " " << input << "\n";
		return true;
	}
	input = start;
	//*result = new Tree("RESTDATA", start, input - start);
	delete c1;
	delete c2;

	return true;
}

bool DECLARATIONSSTART(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (PROTOTYPE(&c1) && DECLARATIONS(&c2))
	{
		//*result = new Tree("DECLARATIONSSTART", start, input - start);
		*result = new Tree("DECLARATIONS", start, input - start);
		(*result)->children.push_back(c1);
		(*result)->children.push_back(c2);
		//std::cout << "DECLARATIONS" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;

	return false;
}

bool DECLARATIONS(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (PROTOTYPE(&c1) && DECLARATIONS(&c2))
	{
		*result = new Tree("DECLARATIONS", start, input - start);
		(*result)->children.push_back(c1);
		(*result)->children.push_back(c2);
		//std::cout << "DECLARATIONS" << " " << input << "\n";
		return true;
	}
	input = start;
	*result = new Tree("DECLARATIONS", start, input - start);
	delete c1;
	delete c2;
	return true;
}

bool VECTOR(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	Tree* c3 = nullptr;
	auto start = input;
	*result = new Tree("VECTOR", start, input - start);
	if (TERM("(", &c1) && VECTOR2(result) && TERM(")", &c3))
	{
		(*result)->children.reverse();
		//(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//(*result)->children.push_back(c3);
		//std::cout << "VECTOR" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	delete c3;
	return false;
}

bool VECTOR2(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (NUM(&c1) && RESTVECTOR(result))
	{
		//*result = new Tree("VECTOR", start, input - start);
		(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "VECTOR2" << " " << input << "\n";
		return true;
	}
	delete c1;
	delete c2;
	return false;
}

bool RESTVECTOR(Tree** result)
{
	Tree* c1 = nullptr;
	Tree* c2 = nullptr;
	auto start = input;
	if (TERM(",", &c1) && VECTOR2(result))
	{
		//*result = new Tree("RESTVECTOR", start, input - start);
		//(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "RESTVECTOR" << " " << input << "\n";
		return true;
	}
	input = start;
	//*result = new Tree("VECTOREND", start, input - start);
	delete c1;
	delete c2;
	return true;
}

bool LUA(Tree** result)
{
	int consumed = matchLua(input);
	if (consumed > 0)
	{
		*result = new Tree("LUA", input, consumed);

		input += consumed;

		consumeWhitespace();

		//std::cout << "LUA" << " " << input << "\n";

		return true;
	}
	return false;
}

bool NUM(Tree** result)
{
	int consumed = matchDecimal(input);
	if (consumed > 0)
	{
		*result = new Tree("NUM", input, consumed);

		input += consumed;

		consumeWhitespace();

		//std::cout << "NUM" << " " << input << "\n";
		return true;
	}
	return false;
}

bool STRING(Tree** result)
{
	int consumed = matchString(input);
	if (consumed > 0)
	{
		// remove quotation marks
		*result = new Tree("STRING", input+1, consumed-2);

		input += consumed;

		consumeWhitespace();

		//std::cout << "STRING "<< input << "\n";
		return true;
	}
	return false;
}

bool IDENTIFIER(Tree** result)
{
	int consumed = matchIdentifier(input);
	if (consumed > 0)
	{
		*result = new Tree("IDENTIFIER", input, consumed);

		input += consumed;

		consumeWhitespace();

		//std::cout << "IDENTIFIER" << " " << input << "\n";
		return true;
	}
	return false;
}

std::string removeQuot(const std::string& str)
{
	return str.substr(1, str.size() - 2);
}

std::vector<Tree*> generateTree(const std::string& source)
{
	input = source.c_str();
	beginning = input;
	std::vector<Tree*> roots;
	bool run = true;
	while (run)
	{
		Tree* child;
		if (DEFINITION(&child))
		{
			auto def = child->children.front()->children.front()->lexeme;
			std::cout << "Parsed: '" << def << "'\n";
			roots.push_back(child);
		}
		else
		{
			int lines = 0;
			for (const char* i = beginning; i != input; i++)
				if (*i == '\n')
					lines++;
			std::cout << "DEFINITION syntax error near line " << lines << "!\n";
			run = false;
		}
		std::string inputleft = input;
		if (inputleft.empty())
		{
			run = false;
		}
	}
	return roots;
}


SceneMesh* extractMeshLua(Tree* lua)
{
	return nullptr;
}

SceneMesh* extractMeshData(Tree* data)
{
	SceneMesh* result = new SceneMesh();


	int vectorSize = data->children.front()->children.size();

	if (vectorSize == 5)
	{
		result->hasTexCoords = true;
	}

	for (auto vector : data->children)
	{
		int currVectorSize = vector->children.size();
		
		if (currVectorSize != 3 && currVectorSize != 5)
		{
			delete result;
			std::cerr << "SCENE ERROR: Vector wrong size\n";
			return nullptr;
		}
		if (currVectorSize != vectorSize)
		{
			delete result;
			std::cerr << "SCENE ERROR: Vector size mismatch\n";
			return nullptr;
		}

		irr::core::vector3df pos;
		irr::core::vector2df tex;
		int i = 0;
		for (auto x : vector->children)
		{
			float num = std::stof(x->lexeme);
			switch (i)
			{
			case 0:
				pos.X = num;
				break;
			case 1:
				pos.Y = num;
				break;
			case 2:
				pos.Z = num;
				break;
			case 3:
				tex.X = num;
				break;
			case 4:
				tex.Y = num;
				break;
			}
			i++;
		}
		result->positions.push_back(pos);
		result->texCoords.push_back(tex);
	}

	return result;
}





SceneMesh* extractMesh(Tree* def)
{
	Tree* prototype = def->children.front();
	Tree* identifier = prototype->children.front();
	Tree* parameters = prototype->children.back();
	if (parameters->children.empty())
	{
		std::cerr << "SCENE ERROR: Expected parameter in 'Mesh'\n";
		return nullptr;
	}
	Tree* firstparam = parameters->children.front();
	if (firstparam->tag != "STRING")
	{
		std::cerr << "SCENE ERROR: Expected name (string) as parameter in 'Mesh'\n";
		return nullptr;
	}

	Tree* block = def->children.back();
	std::string blockContentTag = block->children.front()->tag;
	if (blockContentTag == "DECLARATIONS")
	{
		std::cerr << "SCENE ERROR: Declarations in 'Mesh', expected Lua or raw vertex data\n";
		return nullptr;
	}

	SceneMesh* result = nullptr;
	if (blockContentTag == "DATA")
	{
		result = extractMeshData(block->children.front());
	}
	if (blockContentTag == "LUA")
	{
		result = extractMeshLua(block->children.front());
	}
	if (result)
	{
		result->name = parameters->children.front()->lexeme;
	}
	return result;
}

SceneTexture* extractTextureData(Tree* data)
{
	SceneTexture* result = new SceneTexture();

	int numColors = data->children.size();
	int root = sqrt(numColors);
	if (root * root != numColors)
	{
		delete result;
		std::cerr << "SCENE ERROR: Texture was not a square\n";
		return nullptr;
	}
	result->size = root;

	for (auto color : data->children)
	{
		if (color->children.size() != 3)
		{
			delete result;
			std::cerr << "SCENE ERROR: Color in Texture must have exactly 3 components\n";
		}

		for (auto comp : color->children)
		{
			float val = std::stof(comp->lexeme);
			result->colors.push_back(val * 255);
		}
	}
	return result;
}

SceneTexture* extractTexture(Tree* def)
{
	Tree* prototype = def->children.front();
	Tree* identifier = prototype->children.front();
	Tree* parameters = prototype->children.back();
	if (parameters->children.empty())
	{
		std::cerr << "SCENE ERROR: Expected parameter in 'Texture'\n";
		return nullptr;
	}
	Tree* firstparam = parameters->children.front();
	if (firstparam->tag != "STRING")
	{
		std::cerr << "SCENE ERROR: Expected name (string) as parameter in 'Texture'\n";
		return nullptr;
	}
	Tree* block = def->children.back();
	std::string blockContentTag = block->children.front()->tag;
	if (blockContentTag == "DECLARATIONS")
	{
		std::cerr << "SCENE ERROR: Declarations in 'Texture', expected Lua or raw color data\n";
		return nullptr;
	}

	SceneTexture* result = nullptr;
	if (blockContentTag == "DATA")
	{
		result = extractTextureData(block->children.front());
	}
	if (blockContentTag == "LUA")
	{
		//result = extractTextureLua(block->children.front());
	}
	if (result)
	{
		result->name = parameters->children.front()->lexeme;
	}
	return result;
}


bool loadScene(const std::string & path, irr::IrrlichtDevice * d)
{
	std::string scene = loadFile(path);
	auto defs = generateTree(scene);


	std::map<std::string, SceneMesh*> meshes;
	std::map<std::string, SceneTexture*> textures;

	Tree* sceneDef = nullptr;

	std::cout << "Extracting data from syntax tree...\n";
	for (auto def : defs)
	{
		auto prototype = def->children.front();
		std::string identifier = prototype->children.front()->lexeme;
		std::cout << identifier << "\n";

		if (identifier == "Mesh")
		{
			auto mesh = extractMesh(def);
			if (mesh)
			{
				meshes[mesh->name] = mesh;
			}
			continue;
		}
		if (identifier == "Texture")
		{
			auto texture = extractTexture(def);
			if (texture)
			{
				textures[texture->name] = texture;
			}
			continue;
		}
		if (identifier == "Scene")
		{
			if (sceneDef)
			{
				std::cerr << "SCENE ERROR: Cannot have multiple scene definitions, skipping\n";
			}
			else
			{
				sceneDef = def;
			}
			continue;
		}
		std::cout << "SCENE ERROR: Unknown definition: '" << identifier << "', skipping\n";
	}



	for (auto[name, texture] : textures)
	{
		addTexture(d->getVideoDriver(), texture->name, texture->colors, texture->size, texture->size);
	}


	for (auto [name, mesh] : meshes)
	{
		if (mesh->hasTexCoords)
		{
			addMesh(d, mesh->positions, mesh->texCoords, name);
		}
		else
		{
			addMesh(d, mesh->positions, {}, name);
		}
		for (auto[txname, texture] : textures)
		{
			mybind(d, name, txname);
			break;
		}
	}




	return 1;
}


std::string loadFile(const std::string & path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Could not open file '" << path << "'\n";
		return std::string();
	}
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}



void testScene(const std::string & path)
{
	std::string scene = loadFile(path);

	auto roots = generateTree(scene);

	system("color a");
	for (auto& root : roots)
	{
		root->dump();
	}
}
