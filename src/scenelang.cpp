#include "scenelang.hpp"

/*
LANG: DEFINITION LANG | empty
DEFINITION : PROTOTYPE "{" BLOCK "}"
PROTOTYPE : IDENTIFIER "(" PARAMETERS ")"
PARAMETERS : PARAM RESTPARAMETERS | empty
RESTPARAMETERS : "," PARAMETERS
PARAM : STRING | LUA | PROTOTYPE
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
bool LANG();
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


bool TERM(const char *lit)
{
	int i;
	for (i = 0; lit[i] != 0; i++)
		if (input[i] != lit[i])
			return false;
	input += i;
	return true;
}

bool LANG()
{
	auto start = input;
	if (DEFINITION() && LANG())
		return true;
	input = start;
	return true;
}

bool DEFINITION()
{
	if (PROTOTYPE() && TERM("{") && BLOCK() && TERM("}"))
		return true;

	return false;
}

bool PROTOTYPE()
{
	if (IDENTIFIER() && TERM("(") && PARAMETERS() && TERM(")"))
		return true;

	return false;
}

bool PARAMETERS()
{
	if (PARAM() && RESTPARAMETERS())
		return true;
	return false;
}

bool RESTPARAMETERS()
{
	auto start = input;
	if (TERM(",") && PARAMETERS())
		return true;
	input = start;
	return true;
}

bool PARAM()
{
	if (STRING() || LUA() || PROTOTYPE())
	{
		return true;
	}

	return false;
}

bool BLOCK()
{
	if (DATA() || DECLARATIONS() || LUA())
	{
		return true;
	}

	return false;
}

bool DATA()
{
	auto start = input;
	if (VECTOR() && RESTDATA())
	{
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
		return true;
	}
	input = start;

	return true;
}

bool VECTOR()
{
	if (TERM("(") && VECTOR2() && TERM(")"))
	{
		return true;
	}
	return false;
}

bool VECTOR2()
{
	if (NUM() && RESTVECTOR())
	{
		return true;
	}
	return false;
}

bool RESTVECTOR()
{
	auto start = input;
	if (TERM(",") && VECTOR2())
	{
		return true;
	}
	input = start;
	return true;
}

bool LUA()
{
	return false;
}

bool NUM()
{
	return false;
}

bool STRING()
{
	return false;
}

bool IDENTIFIER()
{
	return false;
}
