#include <iostream>

#include <algorithm>

#include "paint.hpp"

Cursor::Cursor (const vector<string>& new_input) {
	input = new_input;
	l = 0;
	L = input.size();
	c = 0;
	C = input[l].length();
	w = 0;
}

string Cursor::at() {
	return to_string(l + 1) + ":" + to_string(c + 1 - w) + ":";
}

string Cursor::graphic() {
	return '\t' + input[l] + '\n' + '\t' + string(c - w, ' ') + '^';
}

char Cursor::nextChar() {
	unsigned int b = c;
	while (b < C && isspace(input[l][b]))
		b++;

	w = 0;
	
	if (b == C || input[l][b] == '#') {
		if (this->last())
			return ' ';
		else {
			this->backspace();
			return this->nextChar();
		}
	}

	return input[l][b];
}

string Cursor::nextWord() {
	while (c < C && isspace(input[l][c]))
		c++;

	if (c == C || input[l][c] == '#') {
		if (this->last())
			return string();
		else {
			this->backspace();
			return this->nextWord();
		}
	}

	unsigned int b = c;
	c++;

	char ch = input[l][b];
	if (ch != ')' && ch != '}' && ch != '(' && ch != '{')
		while (c < C) {
			ch = input[l][c];
			if (isspace(ch) || ch == ')' || ch == '}' || ch == '(' || ch == '{' || ch == '#')
				break;
			c++;
		}

	w = c - b;

	return input[l].substr(b, c - b);
}

bool Cursor::last() {
	return l == L - 1;
}

void Cursor::backspace() {
	c = 0;
	C = input[++l].length();
}

void Name::parse(Cursor& cursor, vector<string>& names) {
	string name = cursor.nextWord();
	Name::valid(name);

	if (find(names.begin(), names.end(), name) != names.end())
		throw string("error: already used name " + name);

	names.push_back(name);
}

void Name::valid(const string& name) {
	if (name.empty())
		throw string("error: missing name");

	auto it = name.begin();
	if (!isalpha(*it))
		throw string("error: invalid name " + name);

	while (++it != name.end())
		if (!isalnum(*it) && *it != '_')
			throw string("error: invalid name " + name);
}

void Name::exist(const string& name, const vector<string>& names) {
	Name::valid(name);

	if (find(names.begin(), names.end(), name) == names.end())
		throw string("error: unknown name " + name);
}

void Name::parseExist(Cursor& cursor, const vector<string>& names) {
	string name = cursor.nextWord();
	Name::exist(name, names);
}

void Number::parse(Cursor& cursor, const vector<string>& shapes) {
	char op = cursor.nextChar();

	if (isalpha(op)) {
		string word = cursor.nextWord();

		size_t pos1 = word.find('.');

		if (pos1 == string::npos)
			throw string("error: invalid number " + word);

		size_t pos2 = word.find('.', pos1 + 1);

		if (pos2 == string::npos)
			throw string("error: invalid number " + word);

		Name::exist(word.substr(0, pos1), shapes);
		Name::valid(word.substr(pos1 + 1, pos2 - pos1 - 1));

		word = word.substr(pos2 + 1);

		if (word != "x" && word != "y")
			throw string("error: invalid projector " + word);
	} else if (op == '(' || op == '{') {
		Point::parse(cursor, shapes);

		char op = cursor.nextChar();

		if (op != '.')
			throw string("error: missing projector");

		string word = cursor.nextWord().substr(1);

		if (word != "x" && word != "y")
			throw string("error: invalid projector " + word);
	} else {
		string word = cursor.nextWord();

		if (op == '-' || op == '+')
			word = word.substr(1);

		if (word[0] == '.') {
			word = word.substr(1);
			Number::valid(word);
		} else {
			size_t pos = word.find('.');

			if (pos == string::npos)
				Number::valid(word);
			else {
				Number::valid(word.substr(0, pos));

				string end = word.substr(pos + 1);
				if (!end.empty())
					Number::valid(end);
			}
		}
	}
}

void Number::valid(const string& number) {
	if (number.empty())
		throw string("error: invalid number");

	for (auto it = number.begin(); it != number.end(); it++)
		if (!isdigit(*it))
			throw string("error: invalid number " + number);
}

void Point::parse(Cursor& cursor, const vector<string>& shapes) {
	string word = cursor.nextWord();

	if (word == "{") {
		for (int i = 0; i < 2; i++)
			Number::parse(cursor, shapes);

		if (cursor.nextWord() != "}")
			throw string("error: missing }");
	} else if (word == "(") {
		string op = cursor.nextWord();

		if (op == "+" || op == "-") {
			Point::parse(cursor, shapes);

			while (cursor.nextChar() != ')')
				Point::parse(cursor, shapes);

			cursor.nextWord();
		} else if (op == "*" || op == "/") {
			Point::parse(cursor, shapes);
			Number::parse(cursor, shapes);

			if (cursor.nextWord() != ")")
				throw string("error: missing )");
		} else {
			throw string("error: invalid operator " + op);
		}
	} else {
		size_t pos = word.find('.');

		if (pos == string::npos)
			throw string("error: invalid point " + word);

		Name::exist(word.substr(0, pos), shapes);
		Name::valid(word.substr(pos + 1));
	}
}

void Color::keyParse(Cursor& cursor, vector<string>& colors, const vector<string>& shapes) {
	Name::parse(cursor, colors);
	Color::parse(cursor, colors, shapes);
}

void Color::parse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes) {
	string word = cursor.nextWord();

	if (word == "{") {
		for (int i = 0; i < 3; i++)
			Number::parse(cursor, shapes);

		if (cursor.nextWord() != "}")
			throw string("error: missing }");
	} else {
		Name::exist(word, colors);
	}
}

void Fill::keyParse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes) {
	Name::parseExist(cursor, shapes);
	Color::parse(cursor, colors, shapes);
}

void Circle::keyParse(Cursor& cursor, vector<string>& shapes) {
	Name::parse(cursor, shapes);
	Point::parse(cursor, shapes);
	Number::parse(cursor, shapes);
}

void Rectangle::keyParse(Cursor& cursor, vector<string>& shapes) {
	Name::parse(cursor, shapes);
	Point::parse(cursor, shapes);
	for (int i = 0; i < 2; i++)
		Number::parse(cursor, shapes);
}


void Triangle::keyParse(Cursor& cursor,vector<string>& shapes) {
	Name::parse(cursor, shapes);
	for (int i = 0; i < 3; i++)
		Point::parse(cursor, shapes);
}

void Shift::keyParse(Cursor& cursor, vector<string>& shapes) {
	Name::parse(cursor, shapes);
	Point::parse(cursor, shapes);
	Name::parseExist(cursor, shapes);
}

void Rotation::keyParse(Cursor& cursor, vector<string>& shapes) {
	Name::parse(cursor, shapes);
	Number::parse(cursor, shapes);
	Point::parse(cursor, shapes);
	Name::parseExist(cursor, shapes);
}

void Union::keyParse(Cursor& cursor, vector<string>& shapes) {
	Name::parse(cursor, shapes);

	if (cursor.nextWord() != "{")
		throw string("error: missing {");

	Name::parseExist(cursor, shapes);
	while (cursor.nextChar() != '}')
		Name::parseExist(cursor, shapes);

	cursor.nextWord();
}

void Difference::keyParse(Cursor& cursor, vector<string>& shapes) {
	Name::parse(cursor, shapes);

	for (int i = 0; i < 2; i++)
		Name::parseExist(cursor, shapes);
}

void Frame::keyParse(Cursor& cursor, const vector<string>& shapes) {
	Number::parse(cursor, shapes);
	Number::parse(cursor, shapes);
}

const string Color::keyword = "color";
const string Fill::keyword = "fill";
const string Circle::keyword = "circ";
const string Rectangle::keyword = "rect";
const string Triangle::keyword = "tri";
const string Shift::keyword = "shift";
const string Rotation::keyword = "rot";
const string Union::keyword = "union";
const string Difference::keyword = "diff";
const string Frame::keyword = "size";

void Paint::parse(const vector<string>& input) {
	Cursor cursor(input);

	string keyword = cursor.nextWord();

	try {
		if (keyword != Frame::keyword)
			throw string("error: expected " + Frame::keyword + " keyword");

		vector<string> colors;
		vector<string> shapes;
		unsigned int fillCount = 0;

		Frame::keyParse(cursor, shapes);

		while (true) {
			keyword = cursor.nextWord();
			if (keyword == Color::keyword)
				Color::keyParse(cursor, colors, shapes);
			else if (keyword == Circle::keyword)
				Circle::keyParse(cursor, shapes);
			else if (keyword == Rectangle::keyword)
				Rectangle::keyParse(cursor, shapes);
			else if (keyword == Triangle::keyword)
				Triangle::keyParse(cursor, shapes);
			else if (keyword == Shift::keyword)
				Shift::keyParse(cursor, shapes);
			else if (keyword == Rotation::keyword)
				Rotation::keyParse(cursor,shapes);
			else if (keyword == Union::keyword)
				Union::keyParse(cursor, shapes);
			else if (keyword == Difference::keyword)
				Difference::keyParse(cursor, shapes);
			else if (keyword == Fill::keyword) {
				fillCount++;
				Fill::keyParse(cursor, colors, shapes);
			} else if (keyword == string()) {
				cout << "Number of shapes: " << shapes.size() << endl;
				cout << "Number of colors: " << colors.size() << endl;
				cout << "Number of fills: " << fillCount << endl;
				break;
			} else {
				throw "error: invalid keyword " + keyword;
			}
		}
	} catch (string& e) {
		throw string(cursor.at() + ' ' + e + '\n' + cursor.graphic());
	}
}