#include <iostream>
#include <algorithm>

#include "paint.hpp"

using namespace std;

ParseException::ParseException(const string& str) {
	message = str;
}

const char* ParseException::what() const throw() {
	return message.c_str();
}

void Name::parse(Cursor& cursor, vector<string>& names) {
	string name = cursor.nextWord();
	Name::valid(name);

	if (find(names.begin(), names.end(), name) != names.end())
		throw ParseException("already used name " + name);

	names.push_back(name);
}

void Name::valid(const string& name) {
	if (name.empty())
		throw ParseException("expected name, got empty");

	auto it = name.begin();
	if (!isalpha(*it))
		throw ParseException("invalid first character " + string(1, *it));

	while (++it != name.end())
		if (!isalnum(*it) && *it != '_')
			throw ParseException("invalid character " + string(1, *it));
}

void Name::exist(const string& name, const vector<string>& names) {
	Name::valid(name);

	if (find(names.begin(), names.end(), name) == names.end())
		throw ParseException("unknown name " + name);
}

void Name::parseExist(Cursor& cursor, const vector<string>& names) {
	string name = cursor.nextWord();
	Name::exist(name, names);
}

void Number::parse(Cursor& cursor, const vector<string>& shapes) {
	char op = cursor.nextChar();

	if (op == '(' || op == '{' || isalpha(op)) {
		string word = "", proj;
		
		try {
			if (op == '(' || op == '{') {
				Point::parse(cursor, shapes);

				word = cursor.nextWord();

				if (word[0] != '.')
					throw ParseException("expected .x or .y, got " + word);

				proj = word.substr(1);
			} else {
				word = cursor.nextWord();

				size_t pos = word.find_last_of('.');

				if (pos == string::npos)
					throw ParseException("expected point coordinate, got " + word);

				Point::valid(word.substr(0, pos), shapes);

				proj = word.substr(pos + 1);
			}

			if (proj != "x" && proj != "y")
				throw ParseException("expected x or y, got " + proj);

		} catch (ParseException& e) {
			throw ParseException(string(e.what()) + " -> invalid number " + word);
		}
	} else {
		string word = cursor.nextWord();

		try {
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
		} catch (ParseException& e) {
			throw ParseException(string(e.what()) + " -> invalid number " + word);
		}
	}
}

void Number::valid(const string& number) {
	if (number.empty())
		throw ParseException("expected digits, got empty");

	for (auto it = number.begin(); it != number.end(); it++)
		if (!isdigit(*it))
			throw ParseException("invalid character " + string(1, *it));
}

void Point::parse(Cursor& cursor, const vector<string>& shapes) {
	string word = cursor.nextWord();

	try {
		if (word == "{") {
			for (int i = 0; i < 2; i++)
				Number::parse(cursor, shapes);

			if (cursor.nextWord() != "}")
				throw ParseException("missing }");
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
					throw ParseException("missing )");
			} else {
				throw ParseException("expected operator (+, -, * or /), got " + op);
			}
		} else {
			Point::valid(word, shapes);
		}
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid point");
	}
}

void Point::valid(const string& point, const vector<string>& shapes) {
	size_t pos = point.find('.');

	if (pos == string::npos)
		throw ParseException("expected point, got" + point);

	Name::exist(point.substr(0, pos), shapes);
	Name::valid(point.substr(pos + 1));
}

void Color::keyParse(Cursor& cursor, vector<string>& colors, const vector<string>& shapes) {
	try {
		Name::parse(cursor, colors);
		Color::parse(cursor, colors, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid color declaration");
	}
}

void Color::parse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes) {
	string word = cursor.nextWord();

	try {
		if (word == "{") {
			for (int i = 0; i < 3; i++)
				Number::parse(cursor, shapes);

			if (cursor.nextWord() != "}")
				throw ParseException("missing }");
		} else {
			Name::exist(word, colors);
		}
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid color");
	}
}

void Fill::keyParse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes) {
	try {
		Name::parseExist(cursor, shapes);
		Color::parse(cursor, colors, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid fill declaration");
	}
}

void Circle::keyParse(Cursor& cursor, vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);
		Point::parse(cursor, shapes);
		Number::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid circle declaration");
	}
}

void Rectangle::keyParse(Cursor& cursor, vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);
		Point::parse(cursor, shapes);

		for (int i = 0; i < 2; i++)
			Number::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid rectangle declaration");
	}
}


void Triangle::keyParse(Cursor& cursor,vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);

		for (int i = 0; i < 3; i++)
			Point::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid triangle declaration");
	}
}

void Shift::keyParse(Cursor& cursor, vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);
		Point::parse(cursor, shapes);
		Name::parseExist(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid shift declaration");
	}
}

void Rotation::keyParse(Cursor& cursor, vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);
		Number::parse(cursor, shapes);
		Point::parse(cursor, shapes);
		Name::parseExist(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid rotation declaration");
	}
}

void Union::keyParse(Cursor& cursor, vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);

		if (cursor.nextWord() != "{")
			throw ParseException("missing {");

		Name::parseExist(cursor, shapes);
		while (cursor.nextChar() != '}')
			Name::parseExist(cursor, shapes);

		cursor.nextWord();
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid union declaration");
	}
}

void Difference::keyParse(Cursor& cursor, vector<string>& shapes) {
	try {
		Name::parse(cursor, shapes);

		for (int i = 0; i < 2; i++)
			Name::parseExist(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid difference declaration");
	}
}

void Frame::keyParse(Cursor& cursor, const vector<string>& shapes) {
	try {
		for (int i = 0; i < 2; i++)
			Number::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid frame declaration");
	}
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
			throw ParseException("expected " + Frame::keyword + " keyword, got " + keyword);

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
			} else if (keyword.empty()) {
				cout << "Number of shapes: " << shapes.size() << endl;
				cout << "Number of colors: " << colors.size() << endl;
				cout << "Number of fills: " << fillCount << endl;
				break;
			} else {
				throw ParseException("invalid keyword " + keyword);
			}
		}
	} catch (ParseException& e) {
		throw ParseException(cursor.at() + " error: " + string(e.what()) + '\n' + cursor.graphic());
	}
}