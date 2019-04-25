#include <iostream>

#include "paint.hpp"

using namespace std;

Paint::Paint(const vector<string>& input) {
	cursor = Cursor(input);
	this->parse();
}

void Paint::report() const {
	cout << "Number of shapes: " << shapes.size() << endl;
	cout << "Number of colors: " << colors.size() << endl;
	cout << "Number of fills: " << fills.size() << endl;
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

const string Paint::keyword = "size";

void Paint::keyParse() {
	long width, height;

	try {
		width = Number::integer(cursor.nextWord());
		if (width < 0)
			throw ParseException("expected positive width, got " + to_string(width));

		height = Number::integer(cursor.nextWord());
		if (height < 0)
			throw ParseException("expected positive height, got " + to_string(height));
	} catch (ParseException& e) {
		throw ParseException(string(e.what()) + " -> invalid size declaration");
	}

	this->width = width;
	this->height = height;
}

const Color* Paint::pixel(double x, double y) const {
	Point P = Point(x + 0.5, y + 0.5);

	for (auto it = this->fills.rbegin(); it != this->fills.rend(); it++)
		if (it->shape->has(P))
			return it->color.get();

	return new Color();
}
