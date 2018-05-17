#include "regexlib.hpp"


int CharClass::match(char const *text)
{
	return contents.find(*text) == std::string::npos ? -1 : 1;
}

int Star::match(char const *text)
{
	int n, consumed = 0;
	while ((n = operand->match(text)) > 0)
	{
		consumed += n;
		text += n;
	}
	return consumed;
}

int Seq::match(char const *text)
{
	int chars, consumed = 0;
	for (auto c : cells)
	{
		if ((chars = c->match(text)) < 0)  return -1;
		consumed += chars;
		text += chars;
	}
	return consumed;
}

int Plus::match(char const *text)
{
	return seq->match(text);
}

int OneZero::match(char const *text)
{
	int consumed = operand->match(text);
	return consumed > 0 ? consumed : 0;
}

int Term::match(char const *text)
{
	return seq->match(text);
}

int Without::match(char const *text)
{
	if (without->match(text) > 0)
		return 0;
	return operand->match(text);
}
