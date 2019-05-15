#ifndef PAINT
#define PAINT

#include "image.hpp"
#include "tokens.hpp"

class Paint {
	public:		
		Paint() {}
		Paint(const std::vector<std::string>& input);

		/**
		 * @return a small string report of the paint
		 */
		std::string report() const;

		/**
		 * Transform the paint into an image.
		 * 
		 * @return the image
		 */
		Image image() const;

	private:
		size_t _width, _height;
		static const std::string keyword;

		/**
		 * cursor is the cursor to the input text
		 * shapes is the map of shapes sorted by name
		 * colors is the map of colors sorted by name
		 * fills is the vector of fills
		 */
		Cursor cursor;
		std::map<std::string, color_ptr> colors;
		std::map<std::string, shape_ptr> shapes;
		std::vector<Fill> fills;

		/**
		 * Parse as a .paint file.
		 *
		 * @throw an error string at the first grammar mistake
		 */
		void parse();

		/**
		 * Parse as a size declaration the next tokens given by cursor.
		 *
		 * @throw an error string if the tokens aren't a valid size declaration
		 */
		void keyParse();
};

#endif