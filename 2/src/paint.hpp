#ifndef PAINTER
#define PAINTER

#include <string>
#include <vector>

using namespace std;

class Cursor {
	public:
		/**
		 * Class constructor.
		 *
		 * @param new_input the input to read
		 */
		Cursor (const vector<string>& new_input);

		/**
		 * @return the current position formatted as LINE:COL:
		 */
		string at();

		/**
		 * @return the current position graphically
		 */
		string graphic();

		/**
		 * Move the cursor to the start of the next token.
		 *
		 * @return the first character of the next token
		 */
		char nextChar();

		/**
		 * Move the cursor to the end of the next token.
		 *
		 * @return the next token
		 */
		string nextWord();

	private:
		vector<string> input;
		unsigned int l, L, c, C, w;

		/**
		 * @return true if the cursor is within the last line, false otherwise
		 */
		bool last();

		/**
		 * Move the cursor to the start of the next line.
		 */
		void backspace();
};

class Name {
	public:
		/**
		 * Parse as a new name the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid name or is already used
		 */
		static void parse(Cursor& cursor, vector<string>& names);

		/**
		 * @throw an error string if the token(s) is(are)n't a valid name
		 */
		static void valid(const string& name);

		/**
		 * @throw an error string if the token(s) is(are)n't a valid name or is unknown
		 */
		static void exist(const string& name, const vector<string>& names);

		/**
		 * Parse as an existing name the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid name or is unknown
		 */
		static void parseExist(Cursor& cursor, const vector<string>& names);
};

class Number {
	public:
		/**
		 * Parse as a number the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid number
		 */
		static void parse(Cursor& cursor, const vector<string>& shapes);

	private:
		/**
		 * @throw an error string if the string has non-digit characters
		 */
		static void valid(const string& number);
};

class Point {
	public:
		/**
		 * Parse as a point the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid point
		 */
		static void parse(Cursor& cursor, const vector<string>& shapes);
};

class Color {
	public:
		static const string keyword;

		/**
		 * Parse as a color declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid color declaration
		 */
		static void keyParse(Cursor& cursor, vector<string>& colors, const vector<string>& shapes);

		/**
		 * Parse as a color the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid color
		 */
		static void parse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes);
};

class Fill {
	public:
		static const string keyword;

		/**
		 * Parse as a fill declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid fill declaration
		 */
		static void keyParse(Cursor& cursor, const vector<string>& colors, const vector<string>& shapes);
};

class Shape {};

class Circle : public Shape {
	public:
		static const string keyword;

		/**
		 * Parse as a circle declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid circle declaration
		 */
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

		/**
		 * Parse as a frame declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid frame declaration
		 */
		static void keyParse(Cursor& cursor, const vector<string>& shapes);
};

class Paint {
	public :

		/**
		 * Parse as a .paint file an input.
		 *
		 * @throw an error string at the first grammar mistake
		 */
		static void parse(const vector<string>& input);
};

#endif