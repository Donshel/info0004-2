#ifndef TOKENS
#define TOKENS

#include <memory>
#include <cstdint>
#include <array>
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
class Point;
class Color;
class Shape;

typedef std::shared_ptr<const Color> color_ptr;
typedef std::shared_ptr<const Shape> shape_ptr;
typedef std::array<Point, 2> shape_dom;

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
		static double parse(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);

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
		double x, y;

		Point() : x(0), y(0) {}
		Point(double x, double y) : x(x), y(y) {}

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

		/**
		 * @return the cross product between two vectors
		 */
		static double cross(const Point& v1, const Point& v2) { return v1.x * v2.y - v2.x * v1.y; };

		bool operator ==(const Point& P) const { return x == P.x && y == P.y; };

		Point operator +(const Point& P) const { return Point(x + P.x, y + P.y); };
		Point operator -(const Point& P) const { return Point(x - P.x, y - P.y); };
		Point operator *(double n) const { return Point(x * n, y * n); };
		Point operator /(double n) const { return Point(x / n, y / n); };

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
		static Point parse(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a named point the given string.
		 *
		 * @throw a ParseException if the string isn't a valid named point
		 * @return the string as a point
		 */
		static Point named(const std::string& point, const std::map<std::string, shape_ptr>& shapes);
};

class Color {
	public:
		uint8_t r, g, b;

		Color() : r(0), g(0), b(0) {}
		
		static const std::string keyword;

		/**
		 * Parse as a color declaration the next token(s) given by cursor and add it to colors.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid color declaration
		 */
		static void keyParse(Cursor& cursor, std::map<std::string, color_ptr>& colors, const std::map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a color the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid color
		 * @return the token(s) as a color
		 */
		static color_ptr parse(Cursor& cursor, const std::map<std::string, color_ptr>& colors, const std::map<std::string, shape_ptr>& shapes);

	private:
		Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

class Shape {
	public:
		/**
		 * Search a named point in the shape.
		 *
		 * @throw a ParseException if the string isn't a valid point name
		 * @return the point which name is the string in the shape
		 */
		virtual Point point(const std::string& name) const { return name == "c" ? center : Point(); }

		/**
		 * @return true if P is within the shape
		 */
		virtual bool has(const Point& P) const { return center == P; }

		/**
		 * Compute the two opposite vertices of a rectangle that fully contains the shape.
		 * 
		 * @return a shape_dom, i.e an array of two points
		 */
		virtual shape_dom domain() const { return {center, center}; }

		/**
		 * Parse as a shape name the next token given by cursor.
		 *
		 * @throw a ParseException if the token is an unknown shape name
		 * @return a cpy of the shape which name is the token
		 */
		static shape_ptr parse(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a shape declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid shape declaration
		 */
		// static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

	protected:
		Point center;

		/**
		 * Tranform a point relative to the shape into an absolute point.
		 */
		virtual Point absolute(const Point& P) const { return P + center; };

		/**
		 * Tranform an absolute point into a point relative to the shape.
		 */
		virtual Point relative(const Point& P) const { return P - center; };

		/**
		 * Parse as a shape name the next token given by cursor.
		 *
		 * @throw a ParseException if the token is an already used shape name
		 */
		static std::string name(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);
};

class Ellipse : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		virtual Point point(const std::string& name) const;
		virtual bool has(const Point& P) const;
		virtual shape_dom domain() const { return {this->absolute(Point(-a, -b)), this->absolute(Point(a, b))}; };

	protected:
		double a, b;

		Ellipse(Point center, double a, double b) : a(a), b(b) { this->center = center; }
};

class Circle : public Ellipse {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;

	private:
		Circle(Point center, double radius) : Ellipse(center, radius, radius) {}
};

class Polygon : public Shape {
	protected:
		unsigned n;
		std::vector<Point> vertices;

		Polygon() {}
		Polygon(const std::vector<Point>& vertices) : vertices(vertices) { n = vertices.size(); }
		virtual shape_dom domain() const;

		/**
		 * @return the n_th midpoint of the polygon
		 */
		Point midpoint(size_t m) const { return (vertices[m % n] + vertices[(m + 1) % n]) / 2; };
};

class Rectangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		shape_dom domain() const { return {vertices[2], vertices[0]}; };

	private:
		double width, height;

		Rectangle(Point center, double width, double height);
};

class Triangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		
	private:
		Triangle(const std::vector<Point>& vertices) : Polygon(vertices) {}
};

class Shift : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const { return this->absolute(shape->point(name)); };
		bool has(const Point& P) const { return shape->has(this->relative(P)); };
		shape_dom domain() const;

	private:
		shape_ptr shape;

		Shift(const Point& P, shape_ptr& shape) : shape(shape) { this->center = P; }
};

class Rotation : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const { return this->absolute(shape->point(name)); };
		bool has(const Point& P) const { return shape->has(this->relative(P)); };
		shape_dom domain() const;

	private:
		double theta;
		shape_ptr shape;

		Point absolute(const Point& P) const { return P.rotation(theta, center); };
		Point relative(const Point& P) const { return P.rotation(-theta, center); };

		Rotation(double theta, const Point& P, shape_ptr& shape) : theta(theta), shape(shape) { this->center = P; }
};

class Union : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		shape_dom domain() const;

	private:
		std::vector<shape_ptr> set;

		Union(std::vector<shape_ptr>& set) : set(set) {}
};

class Difference : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const { return in->has(P) && !out->has(P); };
		shape_dom domain() const { return in->domain(); };

	private:
		shape_ptr in, out;

		Difference(shape_ptr& in, shape_ptr& out) : in(in), out(out) {}
};

class Fill {
	public:
		shape_ptr shape;
		color_ptr color;
		
		static const std::string keyword;

		/**
		 * Parse as a fill declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid fill declaration
		 */
		static void keyParse(Cursor& cursor, const std::map<std::string, color_ptr>& colors, const std::map<std::string, shape_ptr>& shapes, std::vector<Fill>& fills);

	private:
		Fill(shape_ptr& shape, color_ptr& color) : shape(shape), color(color) {}
};

#endif