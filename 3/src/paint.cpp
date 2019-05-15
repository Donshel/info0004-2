#include "paint.hpp"

using namespace std;

Paint::Paint(const vector<string>& input) {
	cursor = Cursor(input);
	this->parse();
}

string Paint::report() const {
	string str;

	str = "Number of shapes: " + to_string(shapes.size()) + '\n';
	str += "Number of colors: " + to_string(colors.size()) + '\n';
	str += "Number of fills: " + to_string(fills.size()) + '\n';

	return str;
}

void Paint::parse() {
	string word = cursor.nextWord();

	try {
		if (word != Paint::keyword)
			throw ParseException("expected " + Paint::keyword + " keyword, got " + word);

		this->keyParse();

		while (true) {
			word = cursor.nextWord();

			if (word == Color::keyword)
				Color::keyParse(cursor, colors, shapes);
			else if (word == Ellipse::keyword)
				Ellipse::keyParse(cursor, shapes);
			else if (word == Circle::keyword)
				Circle::keyParse(cursor, shapes);
			else if (word == Rectangle::keyword)
				Rectangle::keyParse(cursor, shapes);
			else if (word == Triangle::keyword)
				Triangle::keyParse(cursor, shapes);
			else if (word == Shift::keyword)
				Shift::keyParse(cursor, shapes);
			else if (word == Rotation::keyword)
				Rotation::keyParse(cursor,shapes);
			else if (word == Union::keyword)
				Union::keyParse(cursor, shapes);
			else if (word == Difference::keyword)
				Difference::keyParse(cursor, shapes);
			else if (word == Fill::keyword)
				Fill::keyParse(cursor, colors, shapes, fills);
			else if (word.empty())
				break;
			else
				throw ParseException("invalid keyword " + word);
		}
	} catch (ParseException& e) {
		throw ParseException(cursor.at() + " error: " + string(e.what()) + '\n' + cursor.graphic());
	}
}

Image Paint::image() const {
	Image im = Image(_width, _height);
	unique_ptr<bool[]> isset = make_unique<bool[]>(_width * _height);
	int w = _width - 1, h = _height - 1, x_min, y_min, x_max, y_max;

	Domain dom;
	Color color;

	for (auto it = fills.rbegin(); it != fills.rend(); it++) {
		dom = it->_shape->domain();
		color = *(it->_color);

		x_min = max(int(dom.min._x), 0);
		y_min = max(int(dom.min._y), 0);
		x_max = min(int(++dom.max._x), w);
		y_max = min(int(++dom.max._y), h);

		for (int y = y_min; y <= y_max; y++)
			for (int x = x_min, i = y * _width + x; x <= x_max; x++, i++)
				if (!isset[i] && it->_shape->has(Point(double(x) + 0.5, double(y) + 0.5))) {
					im(x, y) = color;
					isset[i] = true;
				}
	}

	return im;
}

const string Paint::keyword = "size";

void Paint::keyParse() {
	double width, height, rest;

	try {
		rest = modf(Number::parse(cursor, shapes), &width);
		if (width < 0 || rest != 0)
			throw ParseException("expected positive integer width, got " + to_string(width + rest));

		rest = modf(Number::parse(cursor, shapes), &height);
		if (height < 0 || rest != 0)
			throw ParseException("expected positive integer height, got " + to_string(height + rest));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid size declaration");
	}

	_width = width;
	_height = height;
}
