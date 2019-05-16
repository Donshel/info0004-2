#define _USE_MATH_DEFINES

#include "tokens.hpp"

using namespace std;

const string Color::keyword = "color";
const string Shift::keyword = "shift";
const string Rotation::keyword = "rot";
const string Ellipse::keyword = "elli";
const string Circle::keyword = "circ";
const string Rectangle::keyword = "rect";
const string Triangle::keyword = "tri";
const string Union::keyword = "union";
const string Difference::keyword = "diff";
const string Fill::keyword = "fill";

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
		string word, proj;
		Point P;

		try {
			if (op == '(' || op == '{') {
				P = Point::parse(cursor, shapes);

				word = cursor.nextWord();

				if (word[0] != '.')
					throw ParseException("expected .x or .y, got " + word);

				proj = word.substr(1);
			} else {
				word = cursor.nextWord();

				size_t pos = word.find_last_of('.');

				if (pos == string::npos)
					throw ParseException("expected point coordinate, got " + word);

				P = Point::named(word.substr(0, pos), shapes);

				proj = word.substr(pos + 1);
			}

			if (proj == "x")
				n = P._x;
			else if (proj == "y")
				n = P._y;
			else
				throw ParseException("expected x or y, got " + proj);

		} catch (ParseException& e) {
			throw ParseException(string(e.what()) + " -> invalid number " + word);
		}
	} else {
		string number = cursor.nextWord(), word = number;

		if (op == '-' || op == '+')
			word = word.substr(1);

		int has_point = 0;

		for (auto it = word.begin(); it != word.end(); it++)
			if ((!isdigit(*it) && *it != '.') || (*it == '.' && has_point++ > 0))
				throw ParseException("invalid number " + number);

		n = stod(number);
	}

	return n;
}

Point& Point::operator +=(const Point& P) {
	_x += P._x;
	_y += P._y;
	return *this;
}

Point& Point::operator -=(const Point& P) {
	_x -= P._x;
	_y -= P._y;
	return *this;
}

Point& Point::operator *=(double n) {
	_x *= n;
	_y *= n;
	return *this;
}

Point& Point::operator /=(double n) {
	_x /= n;
	_y /= n;
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
	auto it = shapes.find(name);

	if (it == shapes.end())
		throw ParseException("unknown shape " + name);

	const Shape* shape = it->second.get();

	string point = word.substr(pos + 1);

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

			color = make_shared<Color>(Color(r, g, b));
		} else {
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

shape_ptr Shape::parse(Cursor& cursor, const map<string, shape_ptr>& shapes) {
	string name = cursor.nextWord();
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
		P = Point(_a, 0);
	else if (name == "ne")
		P = Point(_a / 2 * M_SQRT2, _b / 2 * M_SQRT2);
	else if (name == "n")
		P = Point(0, _b);
	else if (name == "nw")
		P = Point(-_a / 2 * M_SQRT2 , _b / 2 * M_SQRT2);
	else if (name == "w")
		P = Point(-_a, 0);
	else if (name == "sw")
		P = Point(-_a / 2 * M_SQRT2 , -_b / 2 * M_SQRT2);
	else if (name == "s")
		P = Point(0, -_b);
	else if (name == "se")
		P = Point(_a / 2 * M_SQRT2 , -_b / 2 * M_SQRT2);
	else if (name == "f1")
		P = Point(sqrt(pow(_a, 2) - pow(_b, 2)), 0);
	else if (name == "f2")
		P = Point(-sqrt(pow(_a, 2) - pow(_b, 2)), 0);
	else
		throw ParseException("invalid ellipse named point " + name);

	return this->absolute(P);
}

inline bool Ellipse::has(const Point& P) const {
	Point Q = this->relative(P);

	return pow(Q._x * _b, 2) + pow(Q._y * _a, 2) <= pow(_a * _b, 2);
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

	shapes[name] = make_shared<Ellipse>(Ellipse(center, a, b));
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

inline bool Circle::has(const Point& P) const {
	Point Q = this->relative(P);

	return pow(Q._x, 2) + pow(Q._y, 2) <= pow(_a, 2);
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

	shapes[name] = make_shared<Circle>(Circle(center, radius));
}

Domain Polygon::domain() const {
	Domain dom = {_vertices[0], _vertices[0]};

	for (auto it = _vertices.begin() + 1; it != _vertices.end(); it++) {
		if (it->_x < dom.min._x)
			dom.min._x = it->_x;
		else if (it->_x > dom.max._x)
			dom.max._x = it->_x;

		if (it->_y < dom.min._y)
			dom.min._y = it->_y;
		else if (it->_y > dom.max._y)
			dom.max._y = it->_y;
	}

	return dom;
}

Rectangle::Rectangle(Point center, double width, double height) : _width(width / 2), _height(height / 2) {
	_center = center;
	_n = 4;

	_vertices = {
		this->absolute(Point(_width, _height)),
		this->absolute(Point(_width, -_height)),
		this->absolute(Point(-_width, -_height)),
		this->absolute(Point(-_width, _height))
	};
}

Point Rectangle::point(const string& name) const {
	Point P;

	if (name == "c")
		return _center;
	else if (name == "ne")
		P = _vertices[0];
	else if (name == "se")
		P = _vertices[1];
	else if (name == "sw")
		P = _vertices[2];
	else if (name == "nw")
		P = _vertices[3];
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

inline bool Rectangle::has(const Point& P) const {
	Point Q = this->relative(P);

	return abs(Q._x) <= _width && abs(Q._y) <= _height;
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

	shapes[name] = make_shared<Rectangle>(Rectangle(center, width, height));
}

Point Triangle::point(const string& name) const {
	Point P;

	if (name == "c")
		return _center;
	else if (name == "v0")
		P = _vertices[0];
	else if (name == "v1")
		P = _vertices[1];
	else if (name == "v2")
		P = _vertices[2];
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

	Point v[] = {P - _vertices[0], P - _vertices[1], P - _vertices[2]};

	for (size_t i = 0; i < 3; i++) {
		c = Point::cross(v[i], v[(i + 1) % 3]);
		if (c == 0)
			if (v[i]._x * v[(i + 1) % 3]._x <= 0)
				return true;
			else
				return false;
		else
			b[i] = c > 0;

		if (i > 0 && b[i - 1] != b[i])
			return false;
	}

	return true;
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

	shapes[name] = make_shared<Triangle>(Triangle(vertices));
}

inline Domain Shift::domain() const {
	Domain dom = _shape->domain();

	return {this->absolute(dom.min), this->absolute(dom.max)};
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

	shapes[name] = make_shared<Shift>(Shift(P, shape));
}

Domain Rotation::domain() const {
	Domain dom = _shape->domain();

	vector<Point> vertices = {
		Point(dom.min._x, dom.max._y).rotation(_cos, _sin, _center),
		Point(dom.max._x, dom.min._y).rotation(_cos, _sin, _center),
		dom.min.rotation(_cos, _sin, _center),
		dom.max.rotation(_cos, _sin, _center)
	};

	return Polygon(vertices).domain();
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

	shapes[name] = make_shared<Rotation>(Rotation(theta, P, shape));
}

Point Union::point(const string& name) const {
	Point P;

	try {
		P = this->absolute(_set[0]->point(name));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + "-> invalid union named point");
	}

	return P;
}

bool Union::has(const Point& P) const {
	for (auto it = _set.begin(); it != _set.end(); it++)
		if ((*it)->has(P))
			return true;

	return false;
}

Domain Union::domain() const {
	Domain dom = _set[0]->domain(), temp;

	for (auto it = _set.begin() + 1; it != _set.end(); it++) {
		temp = (*it)->domain();

		if (temp.min._x < dom.min._x)
			dom.min._x = temp.min._x;

		if (temp.min._y < dom.min._y)
			dom.min._y = temp.min._y;

		if (temp.max._x > dom.max._x)
			dom.max._x = temp.max._x;

		if (temp.max._y > dom.max._y)
			dom.max._y = temp.max._y;
	}

	return dom;
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

	shapes[name] = make_shared<Union>(Union(set));
}

Point Difference::point(const string& name) const {
	Point P;

	try {
		P = this->absolute(_in->point(name));
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

	shapes[name] = make_shared<Difference>(Difference(in, out));
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
