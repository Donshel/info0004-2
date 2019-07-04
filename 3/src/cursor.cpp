#include "cursor.hpp"

using namespace std;

Cursor::Cursor() {}

Cursor::Cursor(const vector<string>& input) {
	this->input = input;
	lin = 0;
	lin_max = input.size();
	col = 0;
	col_max = input[lin].length();
	word = 0;
}

string Cursor::at() {
	return to_string(lin + 1) + ":" + to_string(col + 1 - word) + ":";
}

string Cursor::graphic() {
	return '\t' + input[lin] + '\n' + '\t' + string(col - word, ' ') + '^';
}

char Cursor::nextChar() {
	unsigned temp = col;
	while (temp < col_max && isspace(input[lin][temp]))
		temp++;

	word = 0;
	
	if (temp == col_max || input[lin][temp] == '#') {
		if (this->last())
			return ' ';
		else {
			this->backspace();
			return this->nextChar();
		}
	}

	return input[lin][temp];
}

string Cursor::nextWord() {
	while (col < col_max && isspace(input[lin][col]))
		col++;

	if (col == col_max || input[lin][col] == '#') {
		if (this->last())
			return string();
		else {
			this->backspace();
			return this->nextWord();
		}
	}

	unsigned prev = col++;

	char ch = input[lin][prev];
	if (ch != ')' && ch != '}' && ch != '(' && ch != '{')
		while (col < col_max) {
			ch = input[lin][col];
			if (isspace(ch) || ch == ')' || ch == '}' || ch == '(' || ch == '{' || ch == '#')
				break;
			col++;
		}

	word = col - prev;

	return input[lin].substr(prev, word);
}

bool Cursor::last() {
	return lin == lin_max - 1;
}

void Cursor::backspace() {
	col = 0;
	col_max = input[++lin].length();
}
