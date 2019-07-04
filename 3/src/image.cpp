#include "image.hpp"

#include <string>

using namespace std;

std::ostream& operator<<(std::ostream& out, const Color& c) {
	return out << c.r << c.g << c.b;
}

ostream& operator<<(ostream& out, const Image& img)
{
	assert(img.height() > 0);
	assert(img.width() > 0);

	// Write header
	out << "P6 " << img.width() << " " << img.height() << " 255\n";

	// Write pixels
	for (size_t y = 0; y < img.height(); ++y)
		for (size_t x = 0; x < img.width(); ++x)
			out << img(x, img.height() - y - 1); // Reverse Y direction

	return out;
}
