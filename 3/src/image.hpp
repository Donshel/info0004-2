#ifndef IMAGE_HH
#define IMAGE_HH

#include <cassert>
#include <memory>

#include "tokens.hpp"

class Image {
	public:
		Image() : _height(0), _width(0) { }

		Image(size_t width, size_t height) : _height(height), _width(width) {
			_pixels = std::make_unique<Color[]>(width * height);
		}

		size_t height() const { return _height; }

		size_t width() const { return _width; }

		// `img(x,y)` returns the pixel on row `x`, column `y`
		Color& operator() (size_t x, size_t y) {
			assert(x < _width);
			assert(y < _height);
			return _pixels[y * _width + x];
		}

		const Color& operator() (size_t x, size_t y) const {
			assert(x < _width);
			assert(y < _height);
			return _pixels[y * _width + x];
		}

	private:
		std::unique_ptr<Color[]> _pixels;
		size_t _height;
		size_t _width;
};

// Write color in binary 8-bit PPM format
std::ostream& operator<<(std::ostream& out, const Color& c);

// Write image in binary 8-bit PPM format
std::ostream& operator<<(std::ostream&, const Image&);

#endif
