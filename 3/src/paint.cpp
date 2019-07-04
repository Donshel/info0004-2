#include "paint.hpp"

using namespace std;

Image Paint::image() const {
	Image im = Image(width, height);
	unique_ptr<bool[]> isset = make_unique<bool[]>(width * height);

	int w = width - 1, h = height - 1, x_min, y_min, x_max, y_max;
	Domain dom;
	Color color;

	for (auto it = fills.rbegin(); it != fills.rend(); it++) {
		dom = it->shape->domain();
		color = *(it->color);

		x_min = max(int(dom.min.x), 0);
		y_min = max(int(dom.min.y), 0);
		x_max = min(int(++dom.max.x), w);
		y_max = min(int(++dom.max.y), h);

		for (int y = y_min; y <= y_max; y++)
			for (int x = x_min, i = y * width + x; x <= x_max; x++, i++)
				if (!isset[i] && it->shape->has(Point(double(x) + 0.5, double(y) + 0.5))) {
					im(x, y) = color;
					isset[i] = true;
				}
	}

	return im;
}
