#ifndef PAINTER
#define PAINTER

#include "cursor.hpp"

class Name {
	public:
		/**
		 * Parse as a new name the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid name or is already used
		 */
		static void parse(Cursor& cursor, std::vector<std::string>& names);

		/**
		 * @throw an error string if the token(s) is(are)n't a valid name
		 */
		static void valid(const std::string& name);

		/**
		 * @throw an error string if the token(s) is(are)n't a valid name or is unknown
		 */
		static void exist(const std::string& name, const std::vector<std::string>& names);

		/**
		 * Parse as an existing name the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid name or is unknown
		 */
		static void parseExist(Cursor& cursor, const std::vector<std::string>& names);
};

class Number {
	public:
		/**
		 * Parse as a number the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid number
		 */
		static void parse(Cursor& cursor, const std::vector<std::string>& shapes);

	private:
		/**
		 * @throw an error string if the std::string has non-digit characters
		 */
		static void valid(const std::string& number);
};

class Point {
	public:
		/**
		 * Parse as a point the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid point
		 */
		static void parse(Cursor& cursor, const std::vector<std::string>& shapes);

		/**
		 * Parse as a named point the given string.
		 *
		 * @throw an error string if the string isn't a valid named point
		 */
		static void valid(const std::string& point, const std::vector<std::string>& shapes);
};

class Color {
	public:
		static const std::string keyword;

		/**
		 * Parse as a color declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid color declaration
		 */
		static void keyParse(Cursor& cursor, std::vector<std::string>& colors, const std::vector<std::string>& shapes);

		/**
		 * Parse as a color the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid color
		 */
		static void parse(Cursor& cursor, const std::vector<std::string>& colors, const std::vector<std::string>& shapes);
};

class Fill {
	public:
		static const std::string keyword;

		/**
		 * Parse as a fill declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid fill declaration
		 */
		static void keyParse(Cursor& cursor, const std::vector<std::string>& colors, const std::vector<std::string>& shapes);
};

class Shape {};

class Circle : public Shape {
	public:
		static const std::string keyword;

		/**
		 * Parse as a circle declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid circle declaration
		 */
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Rectangle : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Triangle : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Shift : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Rotation : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Union : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Difference : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::vector<std::string>& shapes);
};

class Frame {
	public:
		static const std::string keyword;

		/**
		 * Parse as a frame declaration the next token(s) given by cursor.
		 *
		 * @throw an error string if the token(s) is(are)n't a valid frame declaration
		 */
		static void keyParse(Cursor& cursor, const std::vector<std::string>& shapes);
};

class Paint {
	public :

		/**
		 * Parse as a .paint file an input.
		 *
		 * @throw an error string at the first grammar mistake
		 */
		static void parse(const std::vector<std::string>& input);
};

#endif