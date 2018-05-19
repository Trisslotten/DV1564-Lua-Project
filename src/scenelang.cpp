#include "scenelang.hpp"

#include <fstream>
#include <vector>
#include <list>
#include <map>
#include "regex.hpp"

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
	if (PARAM(&c1) && RESTPARAMETERS(result))
	{
		(*result)->children.push_back(c1);
		//(*result)->children.push_back(c2);
		//std::cout << "PARAMETERS" << " " << input << "\n";

		(*result)->children.reverse();
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
	if (PARAM(&c1) && RESTPARAMETERS(result))
	{
		//*result = new Tree("PARAMETERS", start, input - start);
		(*result)->children.push_back(c1);
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
		*result = new Tree("PARAM", start, input - start);
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
		*result = new Tree("STRING", input, consumed);

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

}

SceneMesh* extractMeshData(Tree* data)
{
	SceneMesh* result = new SceneMesh();


}

SceneMesh* extractMesh(Tree* block)
{
	std::string tag = block->children.front()->tag;
	if (tag == "DECLARATIONS")
	{
		std::cerr << "SCENE ERROR: Declarations in 'Mesh', expected Lua or raw vertex data\n";
		return nullptr;
	}

	SceneMesh* result;
	if (tag == "DATA")
	{
		result = extractMeshData(block->children.front());
	}
	if (tag == "LUA")
	{
		result = extractMeshLua(block->children.front());
	}
	if (result)
	{
		
	}
	return result;
}


bool loadScene(const std::string & path, irr::IrrlichtDevice * d)
{
	std::string scene = loadFile(path);
	auto roots = generateTree(scene);


	std::map<std::string, SceneMesh*> meshes;

	std::cout << "Extracting data from syntax tree...\n";
	for (auto root : roots)
	{
		auto prototype = root->children.front();
		std::string identifier = prototype->children.front()->lexeme;
		std::cout << identifier << "\n";

		if (identifier == "Mesh")
		{
			auto mesh = extractMesh(root->children.back());
			if (mesh)
			{
				meshes[mesh->name] = mesh;
			}
			continue;
		}

		if (identifier == "Texture")
		{

			continue;
		}

		if (identifier == "Scene")
		{

			continue;
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
