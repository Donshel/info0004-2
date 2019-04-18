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

typedef std::shared_ptr<const Shape> shape_ptr;
typedef std::shared_ptr<const Color> color_ptr;

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
		Point() : x(0), y(0) {}
		Point(double x, double y) : x(x), y(y) {}

		/**
		 * @return x, respectively y, coordinate of the point
		 */
		double _x() const;
		double _y() const;

		/**
		 * @return the norm of a vector
		 */
		double norm() const;

		/**
		 * @return the angle of a vector
		 */
		double angle() const;

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
		static double cross(const Point& v1, const Point& v2);

		bool operator ==(const Point& P) const;

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
		static Point parse(Cursor& cursor, const std::map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a named point the given string.
		 *
		 * @throw a ParseException if the string isn't a valid named point
		 * @return the string as a point
		 */
		static Point named(const std::string& point, const std::map<std::string, shape_ptr>& shapes);

	private:
		double x, y;
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
		virtual Shape* clone() const;

		Point _center() const;
		double _phi() const;

		void shift(const Point& P);
		void rotation(double theta);
		void rotation(double theta, const Point& P);

		/**
		 * Search a named point in the shape.
		 *
		 * @throw a ParseException if the string isn't a valid point name
		 * @return the point which name is the string in the shape
		 */
		virtual Point point(const std::string& name) const;

		/**
		 * @return true if P is within the shape
		 */
		virtual bool has(const Point& P) const;

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
		double phi = 0;

		/**
		 * Tranform a point relative to the shape into an absolute point.
		 */
		Point absolute(const Point& P) const;

		/**
		 * Tranform an absolute point into a point relative to the shape.
		 */
		Point relative(const Point& P) const;

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

	protected:
		double a, b;

		Ellipse() {}
		Ellipse(Point center, double a, double b) : a(a), b(b) { this->center = center; }

		Point border(double theta) const;

		virtual Shape* clone() const;
};

class Circle : public Ellipse {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;

	protected:
		Circle() {}
		Circle(Point center, double radius) : Ellipse(center, radius, radius) {}

		virtual Shape* clone() const;
};

class Polygon : public Shape {
	protected:
		std::vector<Point> vertices;

		Polygon() {}
		Polygon(const std::vector<Point>& vertices);

		/**
		 * @return the n_th midpoint of the polygon
		 */
		Point midpoint(size_t n) const;
};

class Rectangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;

	protected:
		double width, height;

		Rectangle() {}
		Rectangle(Point center, double width, double height);

		virtual Shape* clone() const;
};

class Triangle : public Polygon {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		
	protected:
		Triangle() {}
		Triangle(const std::vector<Point>& vertices) : Polygon(vertices) {}

		virtual Shape* clone() const;
};

class Shift : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);
};

class Rotation : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);
};

class Union : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;

	protected:
		std::vector<shape_ptr> set;

		Union() {}
		Union(std::vector<shape_ptr>& set) : set(set) {}

		virtual Shape* clone() const;
};

class Difference : public Shape {
	public:
		static const std::string keyword;
		static void keyParse(Cursor& cursor, std::map<std::string, shape_ptr>& shapes);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;

	protected:
		shape_ptr in, out;

		Difference() {}
		Difference(shape_ptr& in, shape_ptr& out) : in(in), out(out) {}

		virtual Shape* clone() const;
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