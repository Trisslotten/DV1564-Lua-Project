#include "scenelang.hpp"

#include <fstream>
#include "regex.hpp"

/*

DEFINITION : PROTOTYPE "{" BLOCK "}"
PROTOTYPE : IDENTIFIER "(" PARAMETERS ")"
PARAMETERS : PARAM RESTPARAMETERS | empty
RESTPARAMETERS : "," PARAMETERS
PARAM : STRING | LUA | PROTOTYPE | empty
BLOCK : DATA | DECLARATIONS | LUA
DATA : VECTOR RESTDATA | empty
RESTDATA : "," DATA | empty
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
	const char* input;
}
bool TERM(const char *lit);
bool DEFINITION();
bool PROTOTYPE();
bool PARAMETERS();
bool RESTPARAMETERS();
bool PARAM();
bool BLOCK();
bool DATA();
bool RESTDATA();
bool DECLARATIONS();
bool VECTOR();
bool VECTOR2();
bool RESTVECTOR();

bool LUA();
bool NUM();
bool STRING();
bool IDENTIFIER();


void consumeWhitespace()
{
	int consumed = matchWhitespace(input);
	if (consumed > 0)
		input += consumed;
}


bool TERM(const char *lit)
{
	int i;
	for (i = 0; lit[i] != 0; i++)
		if (input[i] != lit[i])
			return false;
	input += i;

	//std::cout << "TERM('" << lit << "')\n";

	consumeWhitespace();

	return true;
}

bool DEFINITION()
{
	if (PROTOTYPE() && TERM("{") && BLOCK() && TERM("}"))
	{
		//std::cout << "DEFINITION " << input << "\n";
		return true;
	}

	return false;
}

bool PROTOTYPE()
{
	if (IDENTIFIER() && TERM("(") && PARAMETERS() && TERM(")"))
	{
		//std::cout << "PROTOTYPE" << " " << input << "\n";
		return true;
	}

	return false;
}

bool PARAMETERS()
{
	if (PARAM() && RESTPARAMETERS())
	{
		//std::cout << "PARAMETERS" << " " << input << "\n";
		return true;
	}
	return false;
}

bool RESTPARAMETERS()
{
	auto start = input;
	if (TERM(",") && PARAMETERS())
	{
		//std::cout << "RESTPARAMETERS" << " " << input << "\n";
		return true;
	}
	input = start;
	return true;
}

bool PARAM()
{
	auto start = input;
	if (STRING() || LUA() || PROTOTYPE())
	{
		//std::cout << "PARAM" << " " << input << "\n";
		return true;
	}
	input = start;
	return true;
}

bool BLOCK()
{
	if (DATA() || DECLARATIONS() || LUA())
	{
		//std::cout << "BLOCK" << " " << input << "\n";
		return true;
	}

	return false;
}

bool DATA()
{
	auto start = input;
	if (VECTOR() && RESTDATA())
	{
		//std::cout << "DATA" << " " << input << "\n";
		return true;
	}
	input = start;

	return true;
}

bool RESTDATA()
{
	auto start = input;
	if (TERM(",") && DATA())
	{
		//std::cout << "RESTDATA" << " " << input << "\n";
		return true;
	}
	input = start;

	return true;
}

bool DECLARATIONS()
{
	auto start = input;
	if (PROTOTYPE() && DECLARATIONS())
	{
		//std::cout << "DECLARATIONS" << " " << input << "\n";
		return true;
	}
	input = start;

	return true;
}

bool VECTOR()
{
	if (TERM("(") && VECTOR2() && TERM(")"))
	{
		//std::cout << "VECTOR" << " " << input << "\n";
		return true;
	}
	return false;
}

bool VECTOR2()
{
	if (NUM() && RESTVECTOR())
	{
		//std::cout << "VECTOR2" << " " << input << "\n";
		return true;
	}
	return false;
}

bool RESTVECTOR()
{
	auto start = input;
	if (TERM(",") && VECTOR2())
	{
		//std::cout << "RESTVECTOR" << " " << input << "\n";
		return true;
	}
	input = start;
	return true;
}

bool LUA()
{
	int consumed = matchLua(input);
	if (consumed > 0)
	{
		input += consumed;

		consumeWhitespace();


		//std::cout << "LUA" << " " << input << "\n";

		return true;
	}
	return false;
}

bool NUM()
{
	int consumed = matchDecimal(input);
	if (consumed > 0)
	{
		input += consumed;

		consumeWhitespace();

		//std::cout << "NUM" << " " << input << "\n";
		return true;
	}
	return false;
}

bool STRING()
{
	int consumed = matchString(input);
	if (consumed > 0)
	{
		input += consumed;

		consumeWhitespace();

		//std::cout << "STRING "<< input << "\n";
		return true;
	}
	return false;
}

bool IDENTIFIER()
{
	int consumed = matchIdentifier(input);
	if (consumed > 0)
	{
		input += consumed;

		consumeWhitespace();

		//std::cout << "IDENTIFIER" << " " << input << "\n";
		return true;
	}
	return false;
}

bool loadScene(const std::string & path, irr::IrrlichtDevice * d)
{



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
	input = scene.c_str();

	bool run = true;
	while (run)
	{
		if (DEFINITION())
		{
			std::cout << "DEFINITION syntax ok\n";
		}
		else
		{
			std::cout << "DEFINITION syntax error!\n";
			run = false;
		}

		if (input == '\0')
		{
			run = false;
		}
	}
}
