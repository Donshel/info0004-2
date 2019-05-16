#ifndef TOKENS
#define TOKENS

#include <array>
#include <cmath>
#include <cstdint>
#include <map>
#include <memory>

#include "cursor.hpp"

class ParseException: public std::exception {
	public:
		explicit ParseException(const std::string& message) : _message(message) {};
		virtual const char* what() const throw() { return _message.c_str(); };

	private:
		std::string _message;
};

// forward declaration
class Point;
class Color;
class Shape;

typedef std::shared_ptr<const Color> color_ptr;
typedef std::shared_ptr<const Shape> shape_ptr;

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

	private:
		/**
		 * Parse as a number the given string.
		 *
		 * @throw a ParseException if the string isn't a valid number
		 * @return the string as a number
		 */
		static double ston(const std::string& number);
};

class Point {
	public:
		double _x, _y;

		Point() : _x(0), _y(0) {}
		Point(double x, double y) : _x(x), _y(y) {}

		/**
		 * Rotation of a point around origin.
		 *
		 * @return the rotated point
		 */
		Point rotation(double cos, double sin) const { return Point(cos * _x - sin * _y, sin * _x + cos * _y); };

		Point rotation(double theta) const { return this->rotation(cos(theta), sin(theta)); };

		/**
		 * Rotation of a point around another.
		 *
		 * @return the rotated point
		 */
		Point rotation(double cos, double sin, const Point& P) const { return (*this - P).rotation(cos, sin) + P; };

		Point rotation(double theta, const Point& P) const { return (*this - P).rotation(theta) + P; };

		/**
		 * @return the cross product between two vectors
		 */
		static double cross(const Point& v1, const Point& v2) { return v1._x * v2._y - v2._x * v1._y; };

		bool operator ==(const Point& P) const { return _x == P._x && _y == P._y; };

		Point operator +(const Point& P) const { return Point(_x + P._x, _y + P._y); };
		Point operator -(const Point& P) const { return Point(_x - P._x, _y - P._y); };
		Point operator *(double n) const { return Point(_x * n, _y * n); };
		Point operator /(double n) const { return Point(_x / n, _y / n); };

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
		uint8_t _r, _g, _b;

		Color() : _r(0), _g(0), _b(0) {}
		Color(uint8_t r, uint8_t g, uint8_t b) : _r(r), _g(g), _b(b) {}
		
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
		 * @return the token(s) as a pointer to color
		 */
		static color_ptr parse(Cursor& cursor, const std::map<std::string, color_ptr>& colors, const std::map<std::string, shape_ptr>& shapes);
};

struct Domain { Point min; Point max; };

class Shape {
	public:
		/**
		 * Search a named point in the shape.
		 *
		 * @throw a ParseException if the string isn't a valid point name
		 * @return the point which name is the string in the shape
		 */
		virtual Point point(const std::string& name) const = 0;

		/**
		 * @return true if P is within the shape
		 */
		virtual bool has(const Point& P) const = 0;

		/**
		 * Compute the two opposite vertices of a rectangle that fully contains the shape.
		 * 
		 * @return a Domain
		 */
		virtual Domain domain() const = 0;

		/**
		 * Parse as a shape name the next token given by cursor.
		 *
		 * @throw a ParseException if the token is an unknown shape name
		 * @return a pointer to the shape which name is the token
		 */
		static shape_ptr parse(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a shape declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid shape declaration
		 */
		// static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

	protected:
		Point _center;

		/**
		 * Tranform a point relative to the shape into an absolute point.
		 */
		virtual Point absolute(const Point& P) const { return P + _center; };

		/**
		 * Tranform an absolute point into a point relative to the shape.
		 */
		virtual Point relative(const Point& P) const { return P - _center; };

		/**
		 * Parse as a shape name the next token given by cursor.
		 *
		 * @throw a ParseException if the token is an already used shape name
		 */
		static std::string name(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);
};

class Shift : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const { return this->absolute(_shape->point(name)); };
		bool has(const Point& P) const { return _shape->has(this->relative(P)); };
		Domain domain() const;

	private:
		shape_ptr _shape;

		Shift(const Point& P, shape_ptr& shape) : _shape(shape) { _center = P; }
};

class Rotation : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const { return this->absolute(_shape->point(name)); };
		bool has(const Point& P) const { return _shape->has(this->relative(P)); };
		Domain domain() const;

	private:
		double _sin, _cos;
		shape_ptr _shape;

		Point absolute(const Point& P) const { return P.rotation(_cos, _sin, _center); };
		Point relative(const Point& P) const { return P.rotation(_cos, -_sin, _center); };

		Rotation(double theta, const Point& P, shape_ptr& shape) : _sin(sin(theta)), _cos(cos(theta)), _shape(shape) { _center = P; }
};

class Ellipse : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		virtual Point point(const std::string& name) const;
		virtual bool has(const Point& P) const;
		virtual Domain domain() const { return {this->absolute(Point(-_a, -_b)), this->absolute(Point(_a, _b))}; };

	protected:
		double _a, _b;

		Ellipse(Point center, double a, double b) : _a(a), _b(b) { _center = center; }
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
	friend Domain Rotation::domain() const;

	public:
		virtual Point point(const std::string& name) const { return name == "c" ? _center : Point(); }
		virtual bool has(const Point& P) const { return _center == P; };
		virtual Domain domain() const;

	protected:
		unsigned _n;
		std::vector<Point> _vertices;

		Polygon() {}
		Polygon(const std::vector<Point>& vertices) : _vertices(vertices) { _n = vertices.size(); }

		/**
		 * @return the n_th midpoint of the polygon
		 */
		Point midpoint(size_t m) const { return (_vertices[m % _n] + _vertices[(m + 1) % _n]) / 2; };
};

class Rectangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		Domain domain() const { return {_vertices[2], _vertices[0]}; };

	private:
		double _width, _height;

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

class Union : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		Domain domain() const;

	private:
		std::vector<shape_ptr> _set;

		Union(std::vector<shape_ptr>& set) : _set(set) {}
};

class Difference : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const { return _in->has(P) && !_out->has(P); };
		Domain domain() const { return _in->domain(); };

	private:
		shape_ptr _in, _out;

		Difference(shape_ptr& in, shape_ptr& out) : _in(in), _out(out) {}
};

class Fill {
	public:
		shape_ptr _shape;
		color_ptr _color;
		
		static const std::string keyword;

		/**
		 * Parse as a fill declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid fill declaration
		 */
		static void keyParse(Cursor& cursor, const std::map<std::string, color_ptr>& colors, const std::map<std::string, shape_ptr>& shapes, std::vector<Fill>& fills);

	private:
		Fill(shape_ptr& shape, color_ptr& color) : _shape(shape), _color(color) {}
};

#endif