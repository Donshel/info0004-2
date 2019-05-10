#include <iostream>
#include <fstream>
#include <bitset>
#include <chrono>

#include "paint.hpp"

using namespace std;

static ostream& operator <<(ostream& out, const Color* color) { return out << color->r << color->g << color->b; };
static ostream& operator <<(ostream& out, const Paint& paint);

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
    output.open(ppm);

    output << paint;

    output.close();

    end = chrono::steady_clock::now();
	diff = end - start;
	time = chrono::duration <double, milli> (diff).count();

	cout << "Wrote " + ppm + " in " << time << " ms" << endl;

	return 0;
}

ostream& operator <<(ostream& out, const Paint& paint) {
	unsigned long w = paint.width, h = paint.height;

    // Header
    out << "P6 " << w << ' ' << h << " 255\n";

    // Pixels
    for (size_t y = 0; y < h; y++)
        for (size_t x = 0; x < w; x++)
            out << paint.pixel(x, h - y - 1);

    return out;
}
