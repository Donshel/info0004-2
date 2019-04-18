#include <iostream>
#include <fstream>
#include <bitset>

#include "paint.hpp"

using namespace std;

static ostream& operator <<(ostream& out, const Color& color);
static ostream& operator <<(ostream& out, const Paint& paint);

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

	Paint paint;

	try {
		paint = Paint(input);
	} catch (ParseException& e) {
		cerr << filename + ':' + string(e.what()) << endl;
		exit(1);
	}

	paint.report();

	ofstream output;
    output.open(filename.substr(0, filename.find_last_of('.')) + ".ppm");

    output << paint;

    output.close();

	return 0;
}

inline ostream& operator <<(ostream& out, const Color& color) {
	return out << color.r << color.g << color.b;
}

ostream& operator <<(ostream& out, const Paint& paint) {
	unsigned long w = paint._width(), h = paint._height();

    // Header
    out << "P6 " << w << ' ' << h << " 255\n";

    // Pixels
    for (size_t y = 0; y < h; y++)
        for (size_t x = 0; x < w; x++)
            out << paint.pixel(x, h - y - 1); // Reverse Y direction

    return out;
}
