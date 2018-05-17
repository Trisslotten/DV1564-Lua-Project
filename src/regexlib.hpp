#pragma once

#include <iostream>
#include <vector>
#include <string>

class Regex
{
public:
	virtual int match(char const *) = 0;

	virtual ~Regex()
	{
	}

};

class CharClass : public Regex
{
	std::string contents;
public:

	CharClass(const std::string& c) : contents(c)
	{
	}

	~CharClass()
	{
	}
	int match(char const *);
};



class Star : public Regex
{
	Regex* operand;
public:


	Star(Regex* operand) : operand(operand)
	{
	}

	~Star()
	{
		/*
		delete operand;
		operand = nullptr;
		*/
	}

	int match(char const *);
};



class Seq : public Regex
{
	std::vector<Regex*> cells;
public:

	Seq(std::initializer_list<Regex*> list)
	{
		cells.insert(cells.end(), list.begin(), list.end());
	}
	Seq(std::vector<Regex*> list) : cells(list)
	{}

	~Seq()
	{
		/*
		for (auto c : cells)
		{
		delete c;
		}
		*/
	}

	int match(char const *);
};

class Plus : public Regex
{
	Seq* seq;
public:
	Plus(Regex* operand)
	{
		seq = new Seq({ operand, new Star(operand) });
	}

	int match(char const *);
};

class OneZero : public Regex
{
	Regex* operand;
public:
	OneZero(Regex* operand): operand(operand)
	{}
	int match(char const *);
};

class Term : public Regex
{
	Seq* seq;
public:
	Term(const std::string& term)
	{
		std::vector<Regex*> terms;
		for (int i = 0; i < term.size(); i++)
		{
			std::string chr;
			chr += term[i];
			terms.push_back(new CharClass(chr));
		}
		seq = new Seq(terms);
	}

	int match(char const *);
};

class Without : public Regex
{
	Regex* operand;
	Regex* without;
public:
	Without(Regex* operand, Regex* without) : operand(operand), without(without)
	{}
	int match(char const *);
};