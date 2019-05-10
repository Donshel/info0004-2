#include <iostream>
#include <fstream>
#include <chrono>

#include "paint.hpp"

using namespace std;

int main(int argc, char* argv[]) {

	// Parse
	auto start = chrono::steady_clock::now();

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

	Paint paint;

	try {
		paint = Paint(input);
	} catch (ParseException& e) {
		cerr << filename + ':' + string(e.what()) << endl;
		exit(1);
	}

	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	auto time = chrono::duration <double, milli> (diff).count();

	cout << "Parsed " + filename + " in " << time << " ms" << endl;

	cout << "----------" << endl;

	paint.report();

	cout << "----------" << endl;

	string ppm = filename.substr(0, filename.find_last_of('.')) + ".ppm";

	// Write
	start = chrono::steady_clock::now();

	ofstream output;
    output.open(ppm, ios::binary);

    output << paint.image();

    output.close();

    end = chrono::steady_clock::now();
	diff = end - start;
	time = chrono::duration <double, milli> (diff).count();

	cout << "Wrote " + ppm + " in " << time << " ms" << endl;

	return 0;
}
