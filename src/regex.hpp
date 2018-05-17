#pragma once

#include <string>

//         first             second        third
// -?([0-9]+\.[0-9]*) | ([0-9]*\.[0-9]+) | [0-9]+

int matchDecimal(char const * text);

// [a-zA-Z_]+[a-zA-Z_0-9]*
int matchIdentifier(char const * text);

// "\w*"|'\w*'
// \w -> alphanumeric + all junk here
int matchString(char const * text);

int matchLua(char const * text);

int matchWhitespace(char const * text);

void testRegex(int(*func)(char const *), const std::string& str);