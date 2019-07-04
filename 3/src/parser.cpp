#include "parser.hpp"

using namespace std;

/* private */

/**
 * Parse as a name the given string.
 *
 * @throw a ParseException if the given string isn't a valid name
 */
static void name(const string& str) {
	if (str.empty())
		throw ParseException("expected name, got empty");

	auto it = str.begin();
	if (!isalpha(*it))
		throw ParseException("invalid first character " + string(1, *it));

	while (++it != str.end())
		if (!isalnum(*it) && *it != '_')
			throw ParseException("invalid character " + string(1, *it));
}

/**
 * Parse as a number the given string.
 *
 * @throw a ParseException if the string isn't a valid number
 * @return the string as a number
 */
static double number(const string& str) {
	unsigned has_point = 0;

	for (auto it = str.begin(); it != str.end(); it++)
		if (!isdigit(*it) && (*it != '.' || has_point++ > 0))
			throw ParseException("invalid number " + str);

	if (str.length() == has_point)
		throw ParseException("expected digit(s)");

	return stod(str);
}

/**
 * Parse as a number the next token(s) given by cursor.
 *
 * @throw a ParseException if the token(s) is(are)n't a valid number
 * @return the token(s) as a number
 */
static double number(Cursor& cursor, const unordered_map<string, shape_ptr>& shapes);

/**
 * Parse as a named point the given string.
 *
 * @throw a ParseException if the string isn't a valid named point
 * @return the string as a point
 */
static Point point(const string& str, const unordered_map<string, shape_ptr>& shapes) {
	size_t pos = str.find('.');

	if (pos == string::npos)
		throw ParseException("expected point, got " + str);

	string name = str.substr(0, pos);
	auto it = shapes.find(name);

	if (it == shapes.end())
		throw ParseException("unknown shape " + name);

	const Shape* shape = it->second.get();

	try {
		return shape->point(str.substr(pos + 1));
	} catch (NamedPointException& e) {
		throw ParseException(string(e.what()));
	}
}

/**
 * Parse as a point the next token(s) given by cursor.
 *
 * @throw a ParseException if the token(s) is(are)n't a valid point
 * @return the token(s) as a point
 */
static Point point(Cursor& cursor, const unordered_map<string, shape_ptr>& shapes);

double number(Cursor& cursor, const unordered_map<string, shape_ptr>& shapes) {
	char op = cursor.nextChar();

	if (isdigit(op) || op == '.') {
		return number(cursor.nextWord());
	} else if (op == '+') {
		return number(cursor.nextWord().substr(1));
	} else if (op == '-') {
		return -number(cursor.nextWord().substr(1));
	} else {
		string word, proj;
		Point P;

		if (isalpha(op)) {
			word = cursor.nextWord();

			size_t pos = word.find_last_of('.');

			if (pos == string::npos)
				throw ParseException("expected point coordinate, got " + word);

			P = point(word.substr(0, pos), shapes);

			proj = word.substr(pos + 1);
		} else if (op == '(' || op == '{') {
			P = point(cursor, shapes);

			word = cursor.nextWord();

			if (word[0] != '.')
				throw ParseException("expected .x or .y, got " + word);

			proj = word.substr(1);
		} else
			throw ParseException("expected point coordinate, got " + word);

		if (proj == "x")
			return P.x;
		else if (proj == "y")
			return P.y;
		else
			throw ParseException("expected x or y, got " + proj);
	}
}

Point point(Cursor& cursor, const unordered_map<string, shape_ptr>& shapes) {
	string word = cursor.nextWord();

	if (word == "{") {
		double x = number(cursor, shapes);
		double y = number(cursor, shapes);

		if (cursor.nextWord() != "}")
			throw ParseException("missing }");

		return Point(x, y);
	} else if (word == "(") {
		string op = cursor.nextWord();
		Point P = point(cursor, shapes);

		if (op == "+") {
			while (cursor.nextChar() != ')')
				P += point(cursor, shapes);

			cursor.nextWord();
		} else if (op == "-") {
			while (cursor.nextChar() != ')')
				P -= point(cursor, shapes);

			cursor.nextWord();
		} else if (op == "*") {
			P *= number(cursor, shapes);

			if (cursor.nextWord() != ")")
				throw ParseException("missing )");
		} else if (op == "/") {
			P /= number(cursor, shapes);

			if (cursor.nextWord() != ")")
				throw ParseException("missing )");
		} else
			throw ParseException("expected operator (+, -, * or /), got " + op);

		return P;
	} else
		return point(word, shapes);
}

/**
 * Parse as a color the next token(s) given by cursor.
 *
 * @throw a ParseException if the token(s) is(are)n't a valid color
 * @return the token(s) as a pointer to color
 */
static color_ptr colorPointer(Cursor& cursor, const unordered_map<string, color_ptr>& colors, const unordered_map<string, shape_ptr>& shapes) {
	string word = cursor.nextWord();

	if (word == "{") {
		double rgb[3];

		for (size_t i = 0; i < 3; i++)
			if ((rgb[i] = number(cursor, shapes)) < 0 || rgb[i] > 1)
				throw ParseException("out of range color component " + to_string(rgb[i]));

		if (cursor.nextWord() != "}")
			throw ParseException("missing }");

		uint8_t r = round(rgb[0] * 255), g = round(rgb[1] * 255), b = round(rgb[2] * 255);

		return make_shared<Color>(Color(r, g, b));
	} else {
		auto it = colors.find(word);

		if (it == colors.end())
			throw ParseException("unknown color " + word);

		return it->second;
	}
}

/**
 * Parse as a shape name the next token given by cursor.
 *
 * @throw a ParseException if the token is an unknown shape name
 * @return a pointer to the shape which name is the token
 */
static shape_ptr shapePointer(Cursor& cursor, const unordered_map<string, shape_ptr>& shapes) {
	string word = cursor.nextWord();
	name(word);

	auto it = shapes.find(word);

	if (it == shapes.end())
		throw ParseException("unknown shape " + word);

	return it->second;
}

/**
 * Parse as a shape name the next token given by cursor.
 *
 * @throw a ParseException if the token is an already used shape name
 * @return the token as a name
 */
static string shapeName(Cursor& cursor, const unordered_map<string, shape_ptr>& shapes) {
	string word = cursor.nextWord();
	name(word);

	auto it = shapes.find(word);

	if (it != shapes.end())
		throw ParseException("already used shape name " + word);

	return word;
}

/* public */

void parse::size(Cursor& cursor, const std::unordered_map<std::string, shape_ptr>& shapes, size_t& width, size_t& height) {
	double w, h, r;

	r = modf(number(cursor, shapes), &w);
	if (w < 0 || r != 0)
		throw ParseException("expected positive integer width, got " + to_string(w + r));

	r = modf(number(cursor, shapes), &h);
	if (h < 0 || r != 0)
		throw ParseException("expected positive integer height, got " + to_string(h + r));

	width = w;
	height = h;
}

void parse::color(Cursor& cursor, unordered_map<string, color_ptr>& colors, const unordered_map<string, shape_ptr>& shapes) {
	string word = cursor.nextWord();
	name(word);

	auto it = colors.find(word);

	if (it != colors.end())
		throw ParseException("already used color name " + word);

	colors[word] = colorPointer(cursor, colors, shapes);
}

void parse::ellipse(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);
	Point center = point(cursor, shapes);

	double a = number(cursor, shapes);
	if (a < 0)
		throw ParseException("expected positive semi-major radius, got " + to_string(a));

	double b = number(cursor, shapes);
	if (b < 0)
		throw ParseException("expected positive semi-minor radius, got " + to_string(b));

	if (a < b)
		throw ParseException("semi-minor radius has to be smaller than semi-major radius");

	shapes[name] = make_shared<Ellipse>(Ellipse(center, a, b));
}

void parse::circle(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);
	Point center = point(cursor, shapes);

	double radius = number(cursor, shapes);
	if (radius < 0)
		throw ParseException("expected positive radius, got " + to_string(radius));

	shapes[name] = make_shared<Circle>(Circle(center, radius));
}

void parse::rectangle(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);
	Point center = point(cursor, shapes);

	double width = number(cursor, shapes);
	if (width < 0)
		throw ParseException("expected positive width, got " + to_string(width));

	double height = number(cursor, shapes);
	if (height < 0)
		throw ParseException("expected positive height, got " + to_string(height));

	shapes[name] = make_shared<Rectangle>(Rectangle(center, width, height));
}

void parse::triangle(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);

	vector<Point> vertices;
	for (size_t i = 0; i < 3; i++)
		vertices.push_back(point(cursor, shapes));

	shapes[name] = make_shared<Triangle>(Triangle(vertices));
}

void parse::shift(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);
	Point P = point(cursor, shapes);
	shape_ptr shape = shapePointer(cursor, shapes);

	shapes[name] = make_shared<Shift>(Shift(P, shape));
}

void parse::rotation(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);
	double theta = M_PI * number(cursor, shapes) / 180;
	Point P = point(cursor, shapes);
	shape_ptr shape = shapePointer(cursor, shapes);

	shapes[name] = make_shared<Rotation>(Rotation(theta, P, shape));
}

void parse::uniion(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);

	if (cursor.nextWord() != "{")
		throw ParseException("missing {");

	vector<shape_ptr> set = {shapePointer(cursor, shapes)};

	while (cursor.nextChar() != '}')
		set.push_back(shapePointer(cursor, shapes));

	cursor.nextWord();

	shapes[name] = make_shared<Union>(Union(set));
}

void parse::difference(Cursor& cursor, unordered_map<string, shape_ptr>& shapes) {
	string name = shapeName(cursor, shapes);
	shape_ptr in = shapePointer(cursor, shapes);
	shape_ptr out = shapePointer(cursor, shapes);

	shapes[name] = make_shared<Difference>(Difference(in, out));
}

void parse::fill(Cursor& cursor, const unordered_map<string, color_ptr>& colors, const unordered_map<string, shape_ptr>& shapes, vector<Fill>& fills) {
	shape_ptr shape = shapePointer(cursor, shapes);
	color_ptr color = colorPointer(cursor, colors, shapes);

	fills.push_back({shape, color});
}

namespace keyword {
	const string size = "size";
	const string color = "color";
	const string ellipse = "elli";
	const string circle = "circ";
	const string rectangle = "rect";
	const string triangle = "tri";
	const string shift = "shift";
	const string rotation = "rot";
	const string uniion = "union";
	const string difference = "diff";
	const string fill = "fill";
}

Paint parse::paint(const vector<string>& input) {
	Cursor cursor = Cursor(input);

	size_t width, height;

	unordered_map<string, color_ptr> colors;
	unordered_map<string, shape_ptr> shapes;
	vector<Fill> fills;
	
	try {
		string word = cursor.nextWord();

		if (word == keyword::size)
			parse::size(cursor, shapes, width, height);
		else
			throw ParseException("invalid keyword " + word + ", expected " + keyword::size);

		while (true) {
			word = cursor.nextWord();

			if (word == keyword::color)
				parse::color(cursor, colors, shapes);
			else if (word == keyword::ellipse)
				parse::ellipse(cursor, shapes);
			else if (word == keyword::circle)
				parse::circle(cursor, shapes);
			else if (word == keyword::rectangle)
				parse::rectangle(cursor, shapes);
			else if (word == keyword::triangle)
				parse::triangle(cursor, shapes);
			else if (word == keyword::shift)
				parse::shift(cursor, shapes);
			else if (word == keyword::rotation)
				parse::rotation(cursor, shapes);
			else if (word == keyword::uniion)
				parse::uniion(cursor, shapes);
			else if (word == keyword::difference)
				parse::difference(cursor, shapes);
			else if (word == keyword::fill)
				parse::fill(cursor, colors, shapes, fills);
			else if (word.empty())
				break;
			else
				throw ParseException("invalid keyword " + word);
		}
	} catch (ParseException& e) {
		throw ParseException(cursor.at() + " error: " + string(e.what()) + '\n' + cursor.graphic());
	}

	return Paint(width, height, fills);
}