#include "regex.hpp"

#include "regexlib.hpp"
#include <string>
#include <iostream>

std::string alphabet()
{
	std::string result;
	for (int i = 0; i < 26; i++)
	{
		result += char(i) + 'a';
		result += char(i) + 'A';
	}
	return result;
}

int matchHex(char const * text)
{
	static Regex* hexChar = new CharClass("012345679abcdefABCDEF");
	static Seq hex({
		new CharClass("0"),
		new CharClass("x"),
		new Plus(hexChar)
				   });
	return hex.match(text);
}

int matchDecimal(char const * text)
{
	static Regex* minus = new OneZero(new CharClass("-"));
	static Regex* number = new CharClass("0123456789");
	static Regex* anyNumNumbers = new Star(number);
	static Regex* atleastOneNumber = new Plus(number);
	static Regex* integer = new Seq({ minus, atleastOneNumber });
	static Regex* dot = new CharClass(".");
	static Regex* numLeft = new Seq({ minus, atleastOneNumber, dot, anyNumNumbers });
	static Regex* numRight = new Seq({ minus, anyNumNumbers, dot, atleastOneNumber });

	int first = numLeft->match(text);
	if (first > 0)
	{
		return first;
	}

	int second = numRight->match(text);
	if (second > 0)
	{
		return second;
	}

	int third = integer->match(text);
	if (third > 0)
	{
		return third;
	}

	return -1;
}

int matchIdentifier(char const * text)
{
	static std::string beginning = alphabet() + "_";
	static std::string rest = beginning + "0123456789";

	static Regex* identifier = new Seq({
		new Plus(new CharClass(beginning)),
		new Star(new CharClass(rest))
									   });

	return identifier->match(text);
}

int matchString(char const * text)
{
	// https://stackoverflow.com/questions/28485801/what-characters-are-legal-to-use-in-string-literals
	static std::string stringContent = alphabet() + "0123456789" + "!#%&h()*+,-./:;<=>?[\]^_{|}~ \n\t\r";

	static Regex* doubleQuot = new Seq({ new CharClass("\""), new Star(new CharClass(stringContent + "'")), new CharClass("\"") });
	static Regex* singleQuot = new Seq({ new CharClass("'"),  new Star(new CharClass(stringContent + "\"")), new CharClass("'") });

	int dq = doubleQuot->match(text);
	if (dq > 0)
	{
		return dq;
	}

	int sq = singleQuot->match(text);
	if (sq > 0)
	{
		return sq;
	}

	return -1;
}
int matchLua(char const * text)
{
	static std::string stringContent = alphabet() + "0123456789" + "!#%&()*+,-./:;<=>?[\]^_{|}~ '\"\n\t\r";
	static Regex* reg = new Seq({ new Term("Lua(<"), new Star(new Without(new CharClass(stringContent), new Term(">)"))), new Term(">)") });
	return reg->match(text);
}

int matchWhitespace(char const * text)
{
	static Regex* reg = new Star(new CharClass(" \n\r\t"));
	return reg->match(text);
}

void testRegex(int(*func)(char const *), const std::string& str)
{
	std::cout << "test '" << str << "': " << func(str.c_str()) << "\n\n";
}
