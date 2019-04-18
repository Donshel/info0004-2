#ifndef TOKENS
#define TOKENS

#include <memory>
#include <cstdint>
#include <map>

#include "cursor.hpp"

class ParseException: public std::exception {
	public:
		explicit ParseException(const std::string& str);
		virtual const char* what() const throw();
	private:
		std::string message;
};

// forward declaration
class Color;
class Shape;

class Name {
	public:
		/**
		 * Parse as a name the next token given by cursor.
		 *
		 * @throw a ParseException if the token isn't a valid name
		 * @return the token as a name
		 */
		static std::string parse(Cursor& cursor);

		/**
		 * @throw a ParseException if the given string isn't a valid name
		 */
		static void valid(const std::string& name);
};

class Number {
	public:
		/**
		 * Parse as a number the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid number
		 * @return the token(s) as a number
		 */
		static double parse(Cursor& cursor, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		/**
		 * Parse as an integer the given string.
		 *
		 * @throw a ParseException if the string isn't a valide integer
		 * @return the string as a integer
		 */
		static long integer(const std::string& number);
};

class Point {
	public:
		Point();
		Point(double x, double y);

		/**
		 * @return x, respectively y, coordinate of the point
		 */
		double _x() const;
		double _y() const;

		/**
		 * Rotation of a point around origin.
		 *
		 * @return the rotated point
		 */
		Point rotation(double theta) const;

		/**
		 * Rotation of a point around another.
		 *
		 * @return the rotated point
		 */
		Point rotation(double theta, const Point& P) const;

		Point operator +(const Point& P) const;
		Point operator -(const Point& P) const;
		Point operator *(double n) const;
		Point operator /(double n) const;

		Point& operator +=(const Point& P);
		Point& operator -=(const Point& P);
		Point& operator *=(double n);
		Point& operator /=(double n);

		/**
		 * Parse as a point the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid point
		 * @return the token(s) as a point
		 */
		static Point parse(Cursor& cursor, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		/**
		 * Parse as a named point the given string.
		 *
		 * @throw a ParseException if the string isn't a valid named point
		 * @return the string as a point
		 */
		static Point named(const std::string& point, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);

	private:
		double x, y;
};

class Color {
	public:
		Color();

		static const std::string keyword;

		/**
		 * Parse as a color declaration the next token(s) given by cursor and add it to colors.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid color declaration
		 */
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Color>>& colors, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		/**
		 * Parse as a color the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid color
		 * @return the token(s) as a color
		 */
		static Color parse(Cursor& cursor, const std::map<std::string, std::unique_ptr<const Color>>& colors, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);

	private:
		uint8_t RGB[3];

		Color(double* RGB);
};

class Shape {
	public:
		Point _center() const;
		double _phi() const;

		Shape& shift(const Point& P);
		Shape& rotation(double theta);
		Shape& rotation(double theta, const Point& P);

		/**
		 * Search a named point in the shape.
		 *
		 * @throw a ParseException if the string isn't a valid point name
		 * @return the point which name is the string in the shape
		 */
		virtual Point point(const std::string& name) const;

		/**
		 * Parse as a shape name the next token given by cursor.
		 *
		 * @throw a ParseException if the token is an unknown shape name
		 * @return a reference to the shape which name is the token
		 */
		static Shape parse(Cursor& cursor, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		/**
		 * Parse as a shape declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid shape declaration
		 */
		// static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

	protected:
		Point center;
		double phi = 0;

		/**
		 * Tranform a point relative to the shape into an absolute point.
		 */
		Point absolute(const Point& P) const;

		/**
		 * Parse as a shape name the next token given by cursor.
		 *
		 * @throw a ParseException if the token is an already used shape name
		 */
		static std::string name(Cursor& cursor, const std::map<std::string, std::unique_ptr<const Shape>>& shapes);
};

class Ellipse : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		virtual Point point(const std::string& name) const;

	protected:
		double a, b;

		Ellipse(Point center, double a, double b);

		Point border(double theta) const;
};

class Circle : public Ellipse {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		Point point(const std::string& name) const;

	protected:
		Circle(Point center, double radius);
};

class Polygon : public Shape {
	protected:
		std::vector<Point> vertices;

		Polygon();
		Polygon(const std::vector<Point>& vertices);

		/**
		 * @return the n_th midpoint of the polygon
		 */
		Point midpoint(size_t n) const;
};

class Rectangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		Point point(const std::string& name) const;

	protected:
		Rectangle(Point center, double width, double height);
};

class Triangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		Point point(const std::string& name) const;
		
	protected:
		Triangle(const std::vector<Point>& vertices);
};

class Shift : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);
};

class Rotation : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);
};

class Union : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		Point point(const std::string& name) const;

	protected:
		std::vector<Shape> set;

		Union(std::vector<Shape>& set);
};

class Difference : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, std::unique_ptr<const Shape>>& shapes);

		Point point(const std::string& name) const;

	protected:
		Shape in, out;

		Difference(Shape& in, Shape& out);
};

class Fill {
	public:
		static const std::string keyword;

		/**
		 * Parse as a fill declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid fill declaration
		 */
		static void keyParse(Cursor& cursor, const std::map<std::string, std::unique_ptr<Color>>& colors, const std::map<std::string, std::unique_ptr<Shape>>& shapes, std::vector<Fill>& fills);

	private:
		Shape shape;
		Color color;

		Fill(const Shape& shape, const Color& color);
};

#endif