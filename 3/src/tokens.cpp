#define _USE_MATH_DEFINES
#include <cmath>

#include "tokens.hpp"

using namespace std;

ParseException::ParseException(const string& str) {
	message = str;
}

const char* ParseException::what() const throw() {
	return message.c_str();
}

const string Color::keyword = "color";
const string Fill::keyword = "fill";
const string Ellipse::keyword = "elli";
const string Circle::keyword = "circ";
const string Rectangle::keyword = "rect";
const string Triangle::keyword = "tri";
const string Shift::keyword = "shift";
const string Rotation::keyword = "rot";
const string Union::keyword = "union";
const string Difference::keyword = "diff";

string Name::parse(Cursor& cursor) {
	string name = cursor.nextWord();
	Name::valid(name);

	return name;
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

double Number::parse(Cursor& cursor, const map<string, unique_ptr<const Shape>>& shapes) {
	char op = cursor.nextChar();
	double n;

	if (op == '(' || op == '{' || isalpha(op)) {
		string word = "", proj;
		Point point;

		try {
			if (op == '(' || op == '{') {
				point = Point::parse(cursor, shapes);

				word = cursor.nextWord();

				if (word[0] != '.')
					throw ParseException("expected .x or .y, got " + word);

				proj = word.substr(1);
			} else {
				word = cursor.nextWord();

				size_t pos = word.find_last_of('.');

				if (pos == string::npos)
					throw ParseException("expected point coordinate, got " + word);

				point = Point::named(word.substr(0, pos), shapes);

				proj = word.substr(pos + 1);
			}

			if (proj == "x")
				n = point._x();
			else if (proj == "y")
				n = point._y();
			else
				throw ParseException("expected x or y, got " + proj);

		} catch (ParseException& e) {
			throw ParseException(string(e.what()) + " -> invalid number " + word);
		}
	} else {
		string number = cursor.nextWord();
		string word = number;

		try {
			if (op == '-' || op == '+')
				word = word.substr(1);

			size_t pos = word.find_last_of('.');

			if (pos == 0)
				Number::integer(word.substr(1));
			else
				if (pos == string::npos || pos == word.length() - 1)
					Number::integer(word);
				else {
					Number::integer(word.substr(0, pos + 1));
					Number::integer(word.substr(pos + 1));
				}

			n = stod(number);
		} catch (ParseException& e) {
			throw ParseException(string(e.what()) + " -> invalid number " + word);
		}
	}

	return n;
}

long Number::integer(const string& number) {
	string word = number;

	if (word[0] == '+' || word[0] == '-')
		word = word.substr(1);

	size_t l = word.length();

	if (l == 0)
		throw ParseException("expected integer, got " + number);

	size_t pos = word.find_last_of('.');
	if (pos != string::npos) {
		if (l == 1 || (pos != l - 1 && stod(word.substr(pos + 1)) != 0))
			throw ParseException("expected integer, got " + number);
	} else
		pos = l;

	for (size_t i = 0; i < pos; i++)
		if (!isdigit(word[i]))
			throw ParseException("invalid character " + string(1, word[i]));

	return stod(number);
}

Point::Point() : Point::Point(0, 0) {}

Point::Point(double x, double y) {
	this->x = x;
	this->y = y;
}

inline double Point::_x() const {
	return x;
}

inline double Point::_y() const {
	return y;
}

Point Point::rotation(double theta) const {
	double r = sqrt(pow(this->x, 2) + pow(this->y, 2));
	double phi = atan2(this->y, this->x) + theta;

	return Point(r * cos(phi), r * sin(phi));
}

Point Point::rotation(double theta, const Point& P) const {
	return (*this - P).rotation(theta) + P;
}

inline Point Point::operator +(const Point& P) const {
	return Point(this->x + P.x, this->y + P.y);
}

inline Point Point::operator -(const Point& P) const {
	return Point(this->x - P.x, this->y - P.y);
}

inline Point Point::operator *(double n) const {
	return Point(this->x * n, this->y * n);
}

inline Point Point::operator /(double n) const {
	return Point(this->x / n, this->y / n);
}

Point& Point::operator +=(const Point& P) {
	this->x += P.x;
	this->y += P.y;
	return *this;
}

Point& Point::operator -=(const Point& P) {
	this->x -= P.x;
	this->y -= P.y;
	return *this;
}

Point& Point::operator *=(double n) {
	this->x *= n;
	this->y *= n;
	return *this;
}

Point& Point::operator /=(double n) {
	this->x /= n;
	this->y /= n;
	return *this;
}

Point Point::parse(Cursor& cursor, const map<string, unique_ptr<const Shape>>& shapes) {
	string word = cursor.nextWord();
	Point point(0, 0);

	try {
		if (word == "{") {
			double x = Number::parse(cursor, shapes);
			double y = Number::parse(cursor, shapes);

			if (cursor.nextWord() != "}")
				throw ParseException("missing }");

			point = Point(x, y);
		} else if (word == "(") {
			string op = cursor.nextWord();

			if (op == "-" || op == "+") {
				point = Point::parse(cursor, shapes);

				if (op == "+")
					while (cursor.nextChar() != ')')
						point += Point::parse(cursor, shapes);
				else
					while (cursor.nextChar() != ')')
						point -= Point::parse(cursor, shapes);

				cursor.nextWord();
			} else if (op == "*" || op == "/") {
				point = Point::parse(cursor, shapes);

				if (op == "*")
					point *= Number::parse(cursor, shapes);
				else
					point /= Number::parse(cursor, shapes);

				if (cursor.nextWord() != ")")
					throw ParseException("missing )");
			} else {
				throw ParseException("expected operator (+, -, * or /), got " + op);
			}
		} else {
			point = Point::named(word, shapes);
		}
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid point");
	}

	return point;
}

Point Point::named(const string& word, const map<string, unique_ptr<const Shape>>& shapes) {
	size_t pos = word.find('.');

	if (pos == string::npos)
		throw ParseException("expected point, got" + word);

	string name = word.substr(0, pos);
	Name::valid(name);
	auto it = shapes.find(name);

	if (it == shapes.end())
		throw ParseException("unknown shape " + name);

	const Shape* shape = (*it).second.get();

	string point = word.substr(pos + 1);
	Name::valid(point);

	return (*shape).point(point);
}

Color::Color() {
	for (size_t i = 0; i < 3; i++)
		RGB[i] = 0;
}

Color::Color(double* RGB) {
	for (size_t i = 0; i < 3; i++)
		this->RGB[i] = (char) RGB[i] * 255;
}

void Color::keyParse(Cursor& cursor, map<string, unique_ptr<const Color>>& colors, const map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	Color* color = new Color();

	try {
		name = cursor.nextWord();
		Name::valid(name);

		auto it = colors.find(name);

		if (it != colors.end())
			throw ParseException("already used color name " + name);

		*color = Color::parse(cursor, colors, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid color declaration");
	}

	colors[name] = unique_ptr<const Color>(color);
}

Color Color::parse(Cursor& cursor, const map<string, unique_ptr<const Color>>& colors, const map<string, unique_ptr<const Shape>>& shapes) {
	string word = cursor.nextWord();
	Color color;

	try {
		if (word == "{") {
			double RGB[3];
			for (size_t i = 0; i < 3; i++)
				if ((RGB[i] = Number::parse(cursor, shapes)) < 0 || RGB[i] > 1)
					throw ParseException("out of range color component " + to_string(RGB[i]));

			if (cursor.nextWord() != "}")
				throw ParseException("missing }");

			color = Color(RGB);
		} else {
			Name::valid(word);

			auto it = colors.find(word);

			if (it == colors.end())
				throw ParseException("unknown color " + word);

			color = *(*it).second;
		}
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid color");
	}

	return color;
}

inline Point Shape::_center() const {
	return center;
}

inline double Shape::_phi() const {
	return phi;
}

Shape& Shape::shift(const Point& P) {
	center += P;

	return *this;
}

Shape& Shape::rotation(double theta) {
	phi += theta;

	return *this;
}

Shape& Shape::rotation(double theta, const Point& P) {
	center = center.rotation(theta, P);
	phi += theta;

	return *this;
}

Point Shape::absolute(const Point& P) const {
	return center + P.rotation(phi);
}

Point Shape::point(const string& name) const { // fake definition : Shape::point is never called
	if (name == "c")
		return center;
	else
		return Point();
}

Shape Shape::parse(Cursor& cursor, const map<string, unique_ptr<const Shape>>& shapes) {
	string name = cursor.nextWord();
	Name::valid(name);

	auto it = shapes.find(name);

	if (it == shapes.end())
		throw ParseException("unknown shape " + name);

	return *(*it).second;
}

string Shape::name(Cursor& cursor, const map<string, unique_ptr<const Shape>>& shapes) {
	string name = cursor.nextWord();
	Name::valid(name);

	auto it = shapes.find(name);

	if (it != shapes.end())
		throw ParseException("already used shape name " + name);

	return name;
}

Ellipse::Ellipse(Point center, double a, double b) {
	this->center = center;
	this->a = a;
	this->b = b;
}

Point Ellipse::point(const string& name) const {
	double c = sqrt(pow(a, 2) - pow(b, 2));
	double theta;

	if (name == "c")
		return center;
	else if (name == "e")
		theta = 0;
	else if (name == "ne")
		theta = M_PI / 4;
	else if (name == "n")
		theta = M_PI / 2;
	else if (name == "nw")
		theta = 3 * M_PI / 4;
	else if (name == "w")
		theta = M_PI;
	else if (name == "sw")
		theta = 5 * M_PI / 4;
	else if (name == "s")
		theta = 3 * M_PI / 2;
	else if (name == "se")
		theta = 7 * M_PI / 4;
	else if (name == "f1")
		return this->absolute(Point(c, 0));
	else if (name == "f2")
		return this->absolute(Point(-c, 0));
	else
		throw ParseException("invalid ellipse named point " + name);

	return this->absolute(this->border(theta));
}

inline Point Ellipse::border(double theta) const {
	return center + Point(a * cos(theta), b * sin(theta));
}

void Ellipse::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	Point center;
	double a, b;

	try {
		name = Shape::name(cursor, shapes);
		center = Point::parse(cursor, shapes);

		a = Number::parse(cursor, shapes);
		if (a < 0)
			throw ParseException("expected positive semi-major radius, got " + to_string(a));

		b = Number::parse(cursor, shapes);
		if (b < 0)
			throw ParseException("expected positive semi-minor radius, got " + to_string(b));

		if (a < b)
			throw ParseException("semi-minor radius has to be smaller than semi-major radius");
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid ellipse declaration");
	}

	shapes[name] = unique_ptr<const Shape>(new Ellipse(center, a, b));
}

Circle::Circle(Point center, double radius) : Ellipse(center, radius, radius) {}

Point Circle::point(const string& name) const {
	Point P;

	try {
		if (name == "f1" || name == "f2")
			throw ParseException("");

		P = Ellipse::point(name);
	} catch (ParseException& e) {
		throw ParseException("invalid circle named point " + name);
	}

	return P;
}

void Circle::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	Point center;
	double radius;

	try {
		name = Shape::name(cursor, shapes);
		center = Point::parse(cursor, shapes);

		radius = Number::parse(cursor, shapes);
		if (radius < 0)
			throw ParseException("expected positive radius, got " + to_string(radius));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid circle declaration");
	}

	shapes[name] = unique_ptr<const Shape>(new Circle(center, radius));
}

Polygon::Polygon() {}

Polygon::Polygon(const vector<Point>& vertices) {
	center = Point();

	for (auto it = vertices.begin(); it != vertices.end(); it++)
		center += *it;

	center /= vertices.size();

	for (auto it = vertices.begin(); it != vertices.end(); it++)
		this->vertices.push_back(*it - center);
}

Point Polygon::midpoint(size_t n) const {
	int l = vertices.size();
	return (vertices[n % l] + vertices[(n + 1) % l]) / 2;
}

Rectangle::Rectangle(Point center, double width, double height) {
	this->center = center;

	vertices.push_back(Point(width / 2, height / 2));
	vertices.push_back(Point(width / 2, -height / 2));
	vertices.push_back(Point(-width / 2, -height / 2));
	vertices.push_back(Point(-width / 2, height / 2));
}

Point Rectangle::point(const string& name) const {
	Point P;

	if (name == "c")
		return center;
	else if (name == "ne")
		P = vertices[0];
	else if (name == "se")
		P = vertices[1];
	else if (name == "sw")
		P = vertices[2];
	else if (name == "nw")
		P = vertices[3];
	else if (name == "e")
		P = this->midpoint(0);
	else if (name == "s")
		P = this->midpoint(1);
	else if (name == "w")
		P = this->midpoint(2);
	else if (name == "n")
		P = this->midpoint(3);
	else
		throw ParseException("invalid rectangle named point " + name);

	return this->absolute(P);
}

void Rectangle::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	Point center;
	double width, height;

	try {
		name = Shape::name(cursor, shapes);
		center = Point::parse(cursor, shapes);

		width = Number::parse(cursor, shapes);
		if (width < 0)
			throw ParseException("expected positive width, got " + to_string(width));

		height = Number::parse(cursor, shapes);
		if (height < 0)
			throw ParseException("expected positive height, got " + to_string(height));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid rectangle declaration");
	}

	shapes[name] = unique_ptr<const Shape>(new Rectangle(center, width, height));
}

Triangle::Triangle(const vector<Point>& vertices) : Polygon(vertices) {}

Point Triangle::point(const string& name) const {
	Point P;

	if (name == "c")
		return center;
	else if (name == "v0")
		P = vertices[0];
	else if (name == "v1")
		P = vertices[1];
	else if (name == "v2")
		P = vertices[2];
	else if (name == "s01")
		P = this->midpoint(0);
	else if (name == "s12")
		P = this->midpoint(1);
	else if (name == "s02")
		P = this->midpoint(2);
	else
		throw ParseException("invalid triangle named point " + name);

	return this->absolute(P);
}

void Triangle::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	vector<Point> vertices;

	try {
		name = Shape::name(cursor, shapes);

		for (size_t i = 0; i < 3; i++)
			vertices.push_back(Point::parse(cursor, shapes));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid triangle declaration");
	}

	shapes[name] = unique_ptr<const Shape>(new Triangle(vertices));
}

void Shift::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	Point P;
	Shape* shape = new Shape();

	try {
		name = Shape::name(cursor, shapes);
		P = Point::parse(cursor, shapes);
		*shape = Shape::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid shift declaration");
	}

	(*shape).shift(P);
	shapes[name] = unique_ptr<const Shape>(shape);
}

void Rotation::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	double theta;
	Point P;
	Shape* shape = new Shape();

	try {
		name = Shape::name(cursor, shapes);
		theta = M_PI * Number::parse(cursor, shapes) / 180;
		P = Point::parse(cursor, shapes);
		*shape = Shape::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid rotation declaration");
	}

	(*shape).rotation(theta, P);
	shapes[name] = unique_ptr<const Shape>(shape);
}

Union::Union(vector<Shape>& set) {
	center = set[0]._center();
	phi = set[0]._phi();

	for (auto it = set.begin(); it != set.end(); it++) {
		(*it).shift(center * -1);
		(*it).rotation(-phi);
	}

	this->set = set;
}

Point Union::point(const string& name) const {
	Point P;

	try {
		P = this->absolute(set[0].point(name));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + "-> invalid union named point");
	}

	return P;
}

void Union::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	vector<Shape> set;

	try {
		name = Shape::name(cursor, shapes);

		if (cursor.nextWord() != "{")
			throw ParseException("missing {");

		set.push_back(Shape::parse(cursor, shapes));

		while (cursor.nextChar() != '}')
			set.push_back(Shape::parse(cursor, shapes));

		cursor.nextWord();
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid union declaration");
	}

	shapes[name] = unique_ptr<const Shape>(new Union(set));
}

Difference::Difference(Shape& in, Shape& out) {
	center = in._center();
	phi = in._phi();

	in.shift(center * -1);
	in.rotation(-phi);

	out.shift(center * -1);
	out.rotation(-phi);

	this->in = in;
	this->out = out;
}

Point Difference::point(const string& name) const {
	Point P;

	try {
		P = this->absolute(in.point(name));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + "-> invalid difference named point");
	}

	return P;
}

void Difference::keyParse(Cursor& cursor, map<string, unique_ptr<const Shape>>& shapes) {
	string name;
	Shape in, out;

	try {
		name = Shape::name(cursor, shapes);
		in = Shape::parse(cursor, shapes);
		out = Shape::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid difference declaration");
	}

	shapes[name] = unique_ptr<const Shape>(new Difference(in, out));
}

Fill::Fill(const Shape& shape, const Color& color) {
	this->shape = shape;
	this->color = color;
}

void Fill::keyParse(Cursor& cursor, const map<string, unique_ptr<Color>>& colors, const map<string, unique_ptr<Shape>>& shapes, vector<Fill>& fills) {
	Shape shape;
	Color color;

	try {
		shape = Shape::parse(cursor, shapes);
		color = Color::parse(cursor, colors, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid fill declaration");
	}

	fills.push_back(Fill(shape, color));
}
