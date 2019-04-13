#include <iostream>
#include <fstream>

#include "paint.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "painter-check: fatal-error: no input file" << endl;
		exit(1);
	}

	string filename = argv[1];
	string line;
	ifstream file;

	file.open(filename);

	if (!file.is_open()) {
		cerr << "painter-check: error: " + filename + ": No such file or directory" << endl;
		exit(1);
	}

	vector<string> input;
	while (getline(file, line))
		input.push_back(line);

	file.close();

	try {
		Paint::parse(input);
	} catch (string& e) {
		cerr << filename + ':' + e << endl;
		exit(1);
	}

	return 0;
}