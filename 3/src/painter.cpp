#include "parser.hpp"

#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "painter-check: fatal-error: no input file" << endl;
		exit(1);
	}

	// .paint reading

	string filename = argv[1];
	string line;
	ifstream file;

	auto start = chrono::steady_clock::now();

	file.open(filename);

	if (!file.is_open()) {
		cerr << "painter-check: error: " + filename + ": No such file or directory" << endl;
		exit(1);
	}

	vector<string> input;
	while (getline(file, line))
		input.push_back(line);

	file.close();

	auto end = chrono::steady_clock::now();
	auto time = chrono::duration <double, milli> (end - start).count();

	cout << ".paint file read in " << time << " ms" << endl;

	// Input parsing

	Paint paint;

	start = chrono::steady_clock::now();

	try {
		paint = parse::paint(input);
	} catch (ParseException& e) {
		cerr << filename + ':' + string(e.what()) << endl;
		exit(1);
	}

	end = chrono::steady_clock::now();
	time = chrono::duration <double, milli> (end - start).count();

	cout << "Input parsed in " << time << " ms" << endl;

	// Image computation

	start = chrono::steady_clock::now();

	Image im = paint.image();

	end = chrono::steady_clock::now();
	time = chrono::duration <double, milli> (end - start).count();

	cout << "Image computed in " << time << " ms" << endl;

	// .ppm writting

	ofstream output;

	start = chrono::steady_clock::now();

	output.open(filename.substr(0, filename.find_last_of('.')) + ".ppm", ios::binary);

	output << im;

	output.close();

	end = chrono::steady_clock::now();
	time = chrono::duration <double, milli> (end - start).count();

	cout << ".ppm file written in " << time << " ms" << endl;

	return 0;
}
