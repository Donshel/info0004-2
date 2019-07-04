#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include <vector>

class Cursor {
	public:
		Cursor();
		Cursor(const std::vector<std::string>& input);

		/**
		 * @return the current position formatted as LINE:COL:
		 */
		std::string at();

		/**
		 * @return the current position graphically
		 */
		std::string graphic();

		/**
		 * Move the cursor to the start of the next word.
		 *
		 * @return the first character of the next word
		 */
		char nextChar();

		/**
		 * Move the cursor to the end of the next word.
		 *
		 * @return the next word
		 */
		std::string nextWord();

	private:
		std::vector<std::string> input;
		unsigned lin, lin_max, col, col_max, word;

		/**
		 * @return true if the cursor is within the last line, false otherwise
		 */
		bool last();

		/**
		 * Move the cursor to the start of the next line.
		 */
		void backspace();
};

#endif