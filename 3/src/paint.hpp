#ifndef PAINT_H
#define PAINT_H

#include "color.hpp"
#include "image.hpp"
#include "shapes.hpp"

typedef std::shared_ptr<const Color> color_ptr;

struct Fill {
	shape_ptr shape;
	color_ptr color;
};

class Paint {
	public:
		Paint() {};
		Paint(size_t width, size_t height, std::vector<Fill>& fills) : width(width), height(height), fills(fills) {};

		/**
		 * Transform the paint into an image.
		 * 
		 * @return the image
		 */
		Image image() const;

	private:
		size_t width, height;
		std::vector<Fill> fills;
};

#endif