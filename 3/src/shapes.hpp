#ifndef SHAPES_H
#define SHAPES_H

#include <cassert>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

class NamedPointException: public std::exception {
	public:
		explicit NamedPointException(const std::string& message) : message(message) {};
		virtual const char* what() const throw() { return message.c_str(); };

	private:
		std::string message;
};

class Point {
	public:
		double x, y;

		Point() : x(0), y(0) {};
		Point(double x, double y) : x(x), y(y) {};

		/**
		 * Rotation of a point around origin.
		 *
		 * @return the rotated point
		 */
		Point rotation(double cos, double sin) const { return Point(cos * x - sin * y, sin * x + cos * y); };
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
};

struct Domain { Point min; Point max; };

class Shape {
	public:
		virtual ~Shape() = default;

		/**
		 * Search a named point in the shape.
		 *
		 * @throw a ShapeException if the string isn't a valid point name
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
};

typedef std::shared_ptr<const Shape> shape_ptr;

class Ellipse : public Shape {
	public:
		Ellipse(Point center, double a, double b) : a(a), b(b), a2(pow(a, 2)) , b2(pow(b, 2)) {
			assert(a >= 0);
			assert(b >= 0);

			this->center = center;
		};

		virtual Point point(const std::string& name) const;
		virtual bool has(const Point& P) const;
		virtual Domain domain() const { return {this->absolute(Point(-a, -b)), this->absolute(Point(a, b))}; };

	protected:
		double a, b, a2, b2;
};

class Circle : public Ellipse {
	public:
		Circle(Point center, double radius) : Ellipse(center, radius, radius) {};

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
};

class Polygon : public Shape {
	public:
		Polygon(const std::vector<Point>& vertices);

		virtual Point point(const std::string& name) const {
			if (name == "c")
				return center;
			else
				throw NamedPointException("invalid named point " + name);
		};
		virtual bool has(const Point& P) const { return center == P; };
		virtual Domain domain() const;

	protected:
		unsigned n;
		std::vector<Point> vertices;

		Polygon() {};

		/**
		 * @return the midpoint of the segment joining the ith and jth vertices of the polygon
		 */
		Point midpoint(size_t i, size_t j) const { return (vertices[i] + vertices[j]) / 2; };
};

class Rectangle : public Polygon {
	public:
		Rectangle(Point center, double width, double height);

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
		Domain domain() const { return {vertices[2], vertices[0]}; };

	private:
		double width, height;
};

class Triangle : public Polygon {
	public:
		Triangle(const std::vector<Point>& vertices) : Polygon(vertices) {};

		Point point(const std::string& name) const;
		bool has(const Point& P) const;
};

class Shift : public Shape {
	public:
		Shift(const Point& P, const shape_ptr& shape) : shape(shape) { center = P; };

		Point point(const std::string& name) const { return this->absolute(shape->point(name)); };
		bool has(const Point& P) const { return shape->has(this->relative(P)); };
		Domain domain() const;

	private:
		shape_ptr shape;
};

class Rotation : public Shape {
	public:
		Rotation(double theta, const Point& P, const shape_ptr& shape) : sin_theta(sin(theta)), cos_theta(cos(theta)), shape(shape) { center = P; };

		Point point(const std::string& name) const { return this->absolute(shape->point(name)); };
		bool has(const Point& P) const { return shape->has(this->relative(P)); };
		Domain domain() const;

	private:
		double sin_theta, cos_theta;
		shape_ptr shape;

		Point absolute(const Point& P) const { return P.rotation(cos_theta, sin_theta, center); };
		Point relative(const Point& P) const { return P.rotation(cos_theta, -sin_theta, center); };
};

class Union : public Shape {
	public:
		Union(std::vector<shape_ptr>& set) : set(set) {}

		Point point(const std::string& name) const { return this->absolute(set[0]->point(name)); };
		bool has(const Point& P) const;
		Domain domain() const;

	private:
		std::vector<shape_ptr> set;
};

class Difference : public Shape {
	public:
		Difference(shape_ptr& in, shape_ptr& out) : in(in), out(out) {};

		Point point(const std::string& name) const { return this->absolute(in->point(name)); };
		bool has(const Point& P) const { return in->has(P) && !out->has(P); };
		Domain domain() const { return in->domain(); }

	private:
		shape_ptr in, out;
};

#endif