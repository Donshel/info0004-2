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

double Number::parse(Cursor& cursor, const map<string, shape_ptr>& shapes) {
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
				n = point.x;
			else if (proj == "y")
				n = point.y;
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

inline Point Point::rotation(double theta) const {
	double c = cos(theta);
	double s = sin(theta);

	return Point(c * x - s * y, s * x + c * y);
}

inline Point Point::rotation(double theta, const Point& P) const {
	return (*this - P).rotation(theta) + P;
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

Point Point::parse(Cursor& cursor, const map<string, shape_ptr>& shapes) {
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

Point Point::named(const string& word, const map<string, shape_ptr>& shapes) {
	size_t pos = word.find('.');

	if (pos == string::npos)
		throw ParseException("expected point, got " + word);

	string name = word.substr(0, pos);
	Name::valid(name);
	auto it = shapes.find(name);

	if (it == shapes.end())
		throw ParseException("unknown shape " + name);

	const Shape* shape = it->second.get();

	string point = word.substr(pos + 1);
	Name::valid(point);

	return shape->point(point);
}

void Color::keyParse(Cursor& cursor, map<string, color_ptr>& colors, const map<string, shape_ptr>& shapes) {
	string name;
	color_ptr color;

	try {
		name = cursor.nextWord();
		Name::valid(name);

		auto it = colors.find(name);

		if (it != colors.end())
			throw ParseException("already used color name " + name);

		color = Color::parse(cursor, colors, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid color declaration");
	}

	colors[name] = color;
}

color_ptr Color::parse(Cursor& cursor, const map<string, color_ptr>& colors, const map<string, shape_ptr>& shapes) {
	string word = cursor.nextWord();
	color_ptr color;

	try {
		if (word == "{") {
			double RGB[3];

			for (size_t i = 0; i < 3; i++)
				if ((RGB[i] = Number::parse(cursor, shapes)) < 0 || RGB[i] > 1)
					throw ParseException("out of range color component " + to_string(RGB[i]));

			if (cursor.nextWord() != "}")
				throw ParseException("missing }");

			uint8_t r = round(RGB[0] * 255), g = round(RGB[1] * 255), b = round(RGB[2] * 255);

			color = color_ptr(new Color(r, g, b));
		} else {
			Name::valid(word);

			auto it = colors.find(word);

			if (it == colors.end())
				throw ParseException("unknown color " + word);

			color = it->second;
		}
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid color");
	}

	return color;
}

Point Shape::point(const string& name) const { // fake definition : Shape::point won't ever be called
	if (name == "c")
		return center;
	else
		throw ParseException("invalid shape named point " + name);
}

bool Shape::has(const Point& P) const { // fake definition : Shape::in won't ever be called
	return center == P;
}

shape_ptr Shape::parse(Cursor& cursor, const map<string, shape_ptr>& shapes) {
	string name = cursor.nextWord();
	Name::valid(name);

	auto it = shapes.find(name);

	if (it == shapes.end())
		throw ParseException("unknown shape " + name);

	return it->second;
}

string Shape::name(Cursor& cursor, const map<string, shape_ptr>& shapes) {
	string name = cursor.nextWord();
	Name::valid(name);

	auto it = shapes.find(name);

	if (it != shapes.end())
		throw ParseException("already used shape name " + name);

	return name;
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
		P = Point(sqrt(pow(a, 2) - pow(b, 2)), 0);
	else if (name == "f2")
		P = Point(-sqrt(pow(a, 2) - pow(b, 2)), 0);
	else
		throw ParseException("invalid ellipse named point " + name);

	return this->absolute(P);
}

bool Ellipse::has(const Point& P) const {
	Point Q = this->relative(P);

	return pow(Q.x * b, 2) + pow(Q.y * a, 2) <= pow(a * b, 2);
}

void Ellipse::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
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

	shapes[name] = shape_ptr(new Ellipse(center, a, b));
}

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

bool Circle::has(const Point& P) const {
	Point Q = this->relative(P);

	return pow(Q.x, 2) + pow(Q.y, 2) <= pow(a, 2);
}

void Circle::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
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

	shapes[name] = shape_ptr(new Circle(center, radius));
}

Rectangle::Rectangle(Point center, double width, double height) : width(width / 2), height(height / 2) {
	this->center = center;
	n = 4;

	vertices.push_back(this->absolute(Point(this->width, this->height)));
	vertices.push_back(this->absolute(Point(this->width, -this->height)));
	vertices.push_back(this->absolute(Point(-this->width, -this->height)));
	vertices.push_back(this->absolute(Point(-this->width, this->height)));
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

	return P;
}

bool Rectangle::has(const Point& P) const {
	Point Q = this->relative(P);

	return abs(Q.x) <= width && abs(Q.y) <= height;
}

void Rectangle::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
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

	shapes[name] = shape_ptr(new Rectangle(center, width, height));
}

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

	return P;
}

bool Triangle::has(const Point& P) const {
	double c;
	bool b[3];

	Point v[] = {P - vertices[0], P - vertices[1], P - vertices[2]};

	for (size_t i = 0; i < 3; i++) {
		c = Point::cross(v[i], v[(i + 1) % 3]);
		if (c == 0)
			if (v[i].x * v[(i + 1) % 3].x <= 0)
				return true;
			else
				return false;
		else
			b[i] = c > 0;
	}

	return (b[0] == b[1]) && (b[1] == b[2]);
}

void Triangle::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
	string name;
	vector<Point> vertices;

	try {
		name = Shape::name(cursor, shapes);

		for (size_t i = 0; i < 3; i++)
			vertices.push_back(Point::parse(cursor, shapes));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid triangle declaration");
	}

	shapes[name] = shape_ptr(new Triangle(vertices));
}

void Shift::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
	string name;
	Point P;
	shape_ptr shape;

	try {
		name = Shape::name(cursor, shapes);
		P = Point::parse(cursor, shapes);
		shape = Shape::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid shift declaration");
	}

	shapes[name] = shape_ptr(new Shift(P, shape));
}

void Rotation::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
	string name;
	double theta;
	Point P;
	shape_ptr shape;

	try {
		name = Shape::name(cursor, shapes);
		theta = M_PI * Number::parse(cursor, shapes) / 180;
		P = Point::parse(cursor, shapes);
		shape = Shape::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid rotation declaration");
	}

	shapes[name] = shape_ptr(new Rotation(theta, P, shape));
}

Point Union::point(const string& name) const {
	Point P;

	try {
		P = this->absolute(set[0]->point(name));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + "-> invalid union named point");
	}

	return P;
}

bool Union::has(const Point& P) const {
	for (auto it = set.begin(); it != set.end(); it++)
		if ((*it)->has(P))
			return true;

	return false;
}

void Union::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
	string name;
	vector<shape_ptr> set;

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

	shapes[name] = shape_ptr(new Union(set));
}

Point Difference::point(const string& name) const {
	Point P;

	try {
		P = this->absolute(in->point(name));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + "-> invalid difference named point");
	}

	return P;
}

void Difference::keyParse(Cursor& cursor, map<string, shape_ptr>& shapes) {
	string name;
	shape_ptr in, out;

	try {
		name = Shape::name(cursor, shapes);
		in = Shape::parse(cursor, shapes);
		out = Shape::parse(cursor, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid difference declaration");
	}

	shapes[name] = shape_ptr(new Difference(in, out));
}

void Fill::keyParse(Cursor& cursor, const map<string, color_ptr>& colors, const map<string, shape_ptr>& shapes, vector<Fill>& fills) {
	shape_ptr shape;
	color_ptr color;

	try {
		shape = Shape::parse(cursor, shapes);
		color = Color::parse(cursor, colors, shapes);
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid fill declaration");
	}

	fills.push_back(Fill(shape, color));
}
