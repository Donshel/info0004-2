#define USE_MATH_DEFINES

#include "shapes.hpp"

using namespace std;

Point& Point::operator +=(const Point& P) {
	x += P.x;
	y += P.y;
	return *this;
}

Point& Point::operator -=(const Point& P) {
	x -= P.x;
	y -= P.y;
	return *this;
}

Point& Point::operator *=(double n) {
	x *= n;
	y *= n;
	return *this;
}

Point& Point::operator /=(double n) {
	x /= n;
	y /= n;
	return *this;
}

Point Ellipse::point(const string& name) const {
	Point P;

	if (name == "c")
		P = Point(0, 0);
	else if (name == "e")
		P = Point(a, 0);
	else if (name == "ne")
		P = Point(a / 2 * M_SQRT2, b / 2 * M_SQRT2);
	else if (name == "n")
		P = Point(0, b);
	else if (name == "nw")
		P = Point(-a / 2 * M_SQRT2 , b / 2 * M_SQRT2);
	else if (name == "w")
		P = Point(-a, 0);
	else if (name == "sw")
		P = Point(-a / 2 * M_SQRT2 , -b / 2 * M_SQRT2);
	else if (name == "s")
		P = Point(0, -b);
	else if (name == "se")
		P = Point(a / 2 * M_SQRT2 , -b / 2 * M_SQRT2);
	else if (name == "f1")
		P = Point(sqrt(a2 - b2), 0);
	else if (name == "f2")
		P = Point(-sqrt(a2 - b2), 0);
	else
		throw NamedPointException("invalid named point " + name);

	return this->absolute(P);
}

inline bool Ellipse::has(const Point& P) const {
	Point Q = this->relative(P);

	return pow(Q.x, 2) * b2 + pow(Q.y, 2) * a2 <= a2 * b2;
}

Point Circle::point(const string& name) const {
	if (name == "f1" || name == "f2")
		throw NamedPointException("invalid named point " + name);

	return Ellipse::point(name);
}

inline bool Circle::has(const Point& P) const {
	Point Q = this->relative(P);

	return pow(Q.x, 2) + pow(Q.y, 2) <= a2;
}

Polygon::Polygon(const std::vector<Point>& vertices) : n(vertices.size()), vertices(vertices) {
	for (auto it = vertices.begin(); it != vertices.end(); it++)
		center += *it;

	center /= n;
}

Domain Polygon::domain() const {
	Domain dom = {vertices[0], vertices[0]};

	for (auto it = vertices.begin() + 1; it != vertices.end(); it++) {
		if (it->x < dom.min.x)
			dom.min.x = it->x;
		else if (it->x > dom.max.x)
			dom.max.x = it->x;

		if (it->y < dom.min.y)
			dom.min.y = it->y;
		else if (it->y > dom.max.y)
			dom.max.y = it->y;
	}

	return dom;
}

Rectangle::Rectangle(Point center, double width, double height) : width(width / 2), height(height / 2) {
	assert(width >= 0);
	assert(height >= 0);

	this->center = center;
	n = 4;

	vertices = {
		this->absolute(Point(this->width, this->height)),
		this->absolute(Point(this->width, -this->height)),
		this->absolute(Point(-this->width, -this->height)),
		this->absolute(Point(-this->width, this->height))
	};
}

Point Rectangle::point(const string& name) const {
	if (name == "c")
		return center;
	else if (name == "ne")
		return vertices[0];
	else if (name == "se")
		return vertices[1];
	else if (name == "sw")
		return vertices[2];
	else if (name == "nw")
		return vertices[3];
	else if (name == "e")
		return this->midpoint(0, 1);
	else if (name == "s")
		return this->midpoint(1, 2);
	else if (name == "w")
		return this->midpoint(2, 3);
	else if (name == "n")
		return this->midpoint(3, 0);
	else
		throw NamedPointException("invalid named point " + name);
}

inline bool Rectangle::has(const Point& P) const {
	Point Q = this->relative(P);

	return abs(Q.x) <= width && abs(Q.y) <= height;
}

Point Triangle::point(const string& name) const {
	if (name == "c")
		return center;
	else if (name == "v0")
		return vertices[0];
	else if (name == "v1")
		return vertices[1];
	else if (name == "v2")
		return vertices[2];
	else if (name == "s01")
		return this->midpoint(0, 1);
	else if (name == "s12")
		return this->midpoint(1, 2);
	else if (name == "s02")
		return this->midpoint(0, 2);
	else
		throw NamedPointException("invalid named point " + name);
}

bool Triangle::has(const Point& P) const {
	double cross;
	bool b[n];

	Point vect[] = {P - vertices[0], P - vertices[1], P - vertices[2]};

	for (size_t i = 0; i < n; i++) {
		cross = Point::cross(vect[i], vect[(i + 1) % n]);
		if (cross == 0)
			if (vect[i].x * vect[(i + 1) % n].x <= 0)
				return true;
			else
				return false;
		else
			b[i] = cross > 0;

		if (i > 0 && b[i - 1] != b[i])
			return false;
	}

	return true;
}

Domain Shift::domain() const {
	Domain dom = shape->domain();

	return {this->absolute(dom.min), this->absolute(dom.max)};
}

Domain Rotation::domain() const {
	Domain dom = shape->domain();

	vector<Point> vertices = {
		this->absolute(Point(dom.min.x, dom.max.y)),
		this->absolute(Point(dom.max.x, dom.min.y)),
		this->absolute(dom.min),
		this->absolute(dom.max)
	};

	return Polygon(vertices).domain();
}

bool Union::has(const Point& P) const {
	for (auto it = set.begin(); it != set.end(); it++)
		if ((*it)->has(P))
			return true;

	return false;
}

Domain Union::domain() const {
	Domain dom = set[0]->domain(), temp;

	for (auto it = set.begin() + 1; it != set.end(); it++) {
		temp = (*it)->domain();

		if (temp.min.x < dom.min.x)
			dom.min.x = temp.min.x;

		if (temp.min.y < dom.min.y)
			dom.min.y = temp.min.y;

		if (temp.max.x > dom.max.x)
			dom.max.x = temp.max.x;

		if (temp.max.y > dom.max.y)
			dom.max.y = temp.max.y;
	}

	return dom;
}