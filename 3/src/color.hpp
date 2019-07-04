#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

class Color {
	public:
		uint8_t r, g, b;

		Color() : r(0), g(0), b(0) {}
		Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

#endif