#include "cursor.hpp"

using namespace std;

Cursor::Cursor() {}

Cursor::Cursor(const vector<string>& input) {
	this->input = input;
	l = 0;
	L = input.size();
	c = 0;
	C = input[l].length();
	w = 0;
}

string Cursor::at() {
	return to_string(l + 1) + ":" + to_string(c + 1 - w) + ":";
}

string Cursor::graphic() {
	return '\t' + input[l] + '\n' + '\t' + string(c - w, ' ') + '^';
}

char Cursor::nextChar() {
	unsigned int b = c;
	while (b < C && isspace(input[l][b]))
		b++;

	w = 0;
	
	if (b == C || input[l][b] == '#') {
		if (this->last())
			return ' ';
		else {
			this->backspace();
			return this->nextChar();
		}
	}

	return input[l][b];
}

string Cursor::nextWord() {
	while (c < C && isspace(input[l][c]))
		c++;

	if (c == C || input[l][c] == '#') {
		if (this->last())
			return string();
		else {
			this->backspace();
			return this->nextWord();
		}
	}

	unsigned int b = c;
	c++;

	char ch = input[l][b];
	if (ch != ')' && ch != '}' && ch != '(' && ch != '{')
		while (c < C) {
			ch = input[l][c];
			if (isspace(ch) || ch == ')' || ch == '}' || ch == '(' || ch == '{' || ch == '#')
				break;
			c++;
		}

	w = c - b;

	return input[l].substr(b, c - b);
}

bool Cursor::last() {
	return l == L - 1;
}

void Cursor::backspace() {
	c = 0;
	C = input[++l].length();
}
