#ifndef PARSER_H
#define PARSER_H

#include "cursor.hpp"
#include "paint.hpp"

#include <unordered_map>

class ParseException: public std::exception {
	public:
		explicit ParseException(const std::string& message) : message(message) {};
		virtual const char* what() const throw() { return message.c_str(); };

	private:
		std::string message;
};

namespace parse {
		/**
		 * Parse as a size declaration the next tokens given by cursor.
		 *
		 * @throw a ParseException if the tokens aren't a valid size declaration
		 */
		void size(Cursor& cursor, const std::unordered_map<std::string, shape_ptr>& shapes, size_t& width, size_t& height);

		/**
		 * Parse as a color declaration the next token(s) given by cursor and add it to colors.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid color declaration
		 */
		void color(Cursor& cursor, std::unordered_map<std::string, color_ptr>& colors, const std::unordered_map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a shape declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid shape declaration
		 */
		void ellipse(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void circle(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void rectangle(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void triangle(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void shift(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void rotation(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void uniion(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);
		void difference(Cursor& cursor, std::unordered_map<std::string, shape_ptr>& shapes);

		/**
		 * Parse as a fill declaration the next token(s) given by cursor.
		 *
		 * @throw a ParseException if the token(s) is(are)n't a valid fill declaration
		 */
		void fill(Cursor& cursor, const std::unordered_map<std::string, color_ptr>& colors, const std::unordered_map<std::string, shape_ptr>& shapes, std::vector<Fill>& fills);

		/**
		 * Parse as a paint file the given vector of strings.
		 *
		 * @throw a ParseException at the first invalid declaration
		 * @return the computed paint
		 */
		Paint paint(const std::vector<std::string>& input);
};

#endif