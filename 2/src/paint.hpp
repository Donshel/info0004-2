#ifndef PAINTER
#define PAINTER

#include <string>
#include <vector>

using namespace std;

class Cursor {
	public:
		Cursor (const vector<string>& new_input);
		string at();
		string graphic();
		char nextChar();
		string nextWord();
	private:
		vector<string> input;
		unsigned int l, L, c, C, w;
		bool last();
		void backspace();
};

class Name {
	public:
		static void parse(Cursor& cursor, vector<string>& names);
		static void valid(const string& name);
		static void exist(const string& name, const vector<string>& names);
		static void parseExist(Cursor& cursor, const vector<string>& names);
};

class Number {
	public:
		static void parse(Cursor& cursor, const vector<string>& shapes);
	private:
		static void valid(const string& number);
};

class Point {
	public:
		static void parse(Cursor& cursor, const vector<string>& shapes);
};

class Color {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& colors, const vector<string>& shapes);
		static void parse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes);
};

class Fill {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes);
};

class Shape {};

class Circle : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Rectangle : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Triangle : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Shift : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Rotation : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Union : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Difference : public Shape {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, vector<string>& shapes);
};

class Frame {
	public:
		static const string keyword;
		static void keyParse(Cursor& cursor, const vector<string>& shapes);
};

class Paint {
	public :
		static void parse(const vector<string>& input);
};

#endif