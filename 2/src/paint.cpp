#include <iostream>

#include <algorithm>

#include "paint.hpp"

Cursor::Cursor (const vector<string>& new_input) {
	input = new_input;
	l = 0;
	L = input.size();
	c = 0;
	C = input[l].length();
}

string Cursor::at() {
	return ":" + to_string(l + 1) + ":" + to_string(c + 1);
}

char Cursor::nextChar() {
	unsigned int b = c;
	while (b < C && isspace(input[l][b]))
		b++;

	if (b == C) {
		if (this->last())
			return ' ';
		else
			return input[l + 1][0];
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

	unsigned int dot_count = 0;

	char ch = input[l][b];
	if (ch != ')' && ch != '}' && ch != '(' && ch != '{')
		while (c < C) {
			ch = input[l][c];
			if (isspace(ch) || ch == ')' || ch == '}' || ch == '(' || ch == '{' || (ch == '.' && dot_count++ > 0) || ch == '#')
				break;
			c++;
		}

	return input[l].substr(b, c - b);
}

bool Cursor::last() {
	return l == L - 1;
}

void Cursor::backspace() {
	c = 0;
	C = input[++l].length();
}

void Name::valid(const string& name) {
	if (name.empty())
		throw string("error: empty name");

	auto it = name.begin();
	if (!isalpha(*it))
		throw string("error: invalid name " + name);

	while (++it != name.end())
		if (!isalnum(*it) && *it != '_')
			throw string("error: invalid name " + name);
}

void Number::parse(Cursor& cursor, const vector<string>& shapes) {
	char op = cursor.nextChar();

	if (isdigit(op) || op == '-' || op == '+' || op == '.') {
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
	} else {
		Point::parse(cursor, shapes);

		string word = cursor.nextWord();

		if (word[0] != '.')
			throw string("error: invalid projector " + word);

		word = word.substr(1);

		if (word != "x" && word != "y")
			throw string("error: invalid projector " + word);
	}
}

void Number::valid(const string& word) {
	if (word.empty())
		throw string("error: invalid number");

	for (auto it = word.begin(); it != word.end(); it++)
		if (!isdigit(*it))
			throw string("error: invalid number " + word);
}

void Point::parse(Cursor& cursor, const vector<string>& shapes) {
	string word = cursor.nextWord();

	if (word == "{") {
		Number::parse(cursor, shapes);
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

		string shape_name = word.substr(0, pos);
		Name::valid(shape_name);

		if (find(shapes.begin(), shapes.end(), shape_name) == shapes.end())
			throw string("error: unknown shape " + shape_name);

		string point_name = word.substr(pos + 1);
		Name::valid(point_name);
	}
}

void Color::keyParse(Cursor& cursor, vector<string>& colors, const vector<string>& shapes) {
	Color::nameParse(cursor, colors, shapes);
	Color::parse(cursor, colors, shapes);
}

void Color::parse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes) {
	string word = cursor.nextWord();

	if (word == "{") {
		Number::parse(cursor, shapes);
		Number::parse(cursor, shapes);
		Number::parse(cursor, shapes);

		if (cursor.nextWord() != "}")
			throw string("error: missing }");
	} else {
		Name::valid(word);

		if (find(colors.begin(), colors.end(), word) == colors.end())
			throw string("error: unknown color " + word);
	}
}

void Color::nameParse(Cursor& cursor, vector<string>& colors, const vector<string>& shapes) {
	string word = cursor.nextWord();
	Name::valid(word);

	if (find(colors.begin(), colors.end(), word) != colors.end() || find(shapes.begin(), shapes.end(), word) != shapes.end())
		throw string("error: already used name " + word);

	colors.push_back(word);
}

void Fill::keyParse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes) {
	Shape::parse(cursor, shapes);
	Color::parse(cursor, colors, shapes);
}

void Shape::parse(Cursor& cursor, const vector<string>& shapes) {
	string word = cursor.nextWord();
	Name::valid(word);

	if (find(shapes.begin(), shapes.end(), word) == shapes.end())
		throw string("error: unknown shape " + word);
}

void Shape::nameParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	string word = cursor.nextWord();
	Name::valid(word);

	if (find(colors.begin(), colors.end(), word) != colors.end() || find(shapes.begin(), shapes.end(), word) != shapes.end())
		throw string("error: already used name " + word);

	shapes.push_back(word);
}

void Circle::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);
	Point::parse(cursor, shapes);
	Number::parse(cursor, shapes);
}

void Rectangle::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);
	Point::parse(cursor, shapes);
	for (int i = 0; i < 2; i++)
		Number::parse(cursor, shapes);
}


void Triangle::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);
	for (int i = 0; i < 3; i++)
		Point::parse(cursor, shapes);
}

void Shift::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);
	Point::parse(cursor, shapes);
	Shape::parse(cursor, shapes);
}

void Rotation::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);
	Number::parse(cursor, shapes);
	Point::parse(cursor, shapes);
	Shape::parse(cursor, shapes);
}

void Union::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);

	if (cursor.nextWord() != "{")
		throw string("error: missing {");

	Shape::parse(cursor, shapes);
	while (cursor.nextChar() != '}')
		Shape::parse(cursor, shapes);

	cursor.nextWord();
}

void Difference::keyParse(Cursor& cursor, const vector<string>& colors, vector<string>& shapes) {
	Shape::nameParse(cursor, colors, shapes);

	for (int i = 0; i < 2; i++)
		Shape::parse(cursor, shapes);
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
				Circle::keyParse(cursor, colors, shapes);
			else if (keyword == Rectangle::keyword)
				Rectangle::keyParse(cursor, colors, shapes);
			else if (keyword == Triangle::keyword)
				Triangle::keyParse(cursor, colors, shapes);
			else if (keyword == Shift::keyword)
				Shift::keyParse(cursor, colors, shapes);
			else if (keyword == Rotation::keyword)
				Rotation::keyParse(cursor, colors, shapes);
			else if (keyword == Union::keyword)
				Union::keyParse(cursor, colors, shapes);
			else if (keyword == Difference::keyword)
				Difference::keyParse(cursor, colors, shapes);
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
		throw string(cursor.at() + " " + e);
	}
}