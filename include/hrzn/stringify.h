/*
MIT License

Copyright (c) 2022 TheShouting

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include "hrzn.h"

#include <sstream>
#include <iomanip>

namespace hrzn {

	template <typename T>
	std::ostream& operator<<(std::ostream& output, const ITuple<T>& tuple) {
		output << tuple.x << tuple.y;
		return output;
	}

	template <typename T>
	std::istream& operator>>(std::istream& input, ITuple<T>& tuple) {
		input >> tuple.x >> tuple.y;
		return input;
	}

	std::ostream& operator<<(std::ostream& output, const hRotation& rot) {
		output << rot.tau;
		return output;
	}

	std::istream& operator>>(std::istream& input, hRotation& rot) {
		input >> rot.tau;
		return input;
	}

	std::ostream& operator<<(std::ostream& output, const Area& rect) {
		output << rect.x1 << rect.y1 << rect.x2 << rect.y2;
		return output;
	}

	std::istream& operator>>(std::istream& input, Area& rect) {
		input >> rect.x1 >> rect.y1 >> rect.x2 >> rect.y2;
		return input;
	}

	template <typename T>
	std::string toString(const ITuple<T>& tuple) {
		std::stringstream ss;
		ss << "Tuple{" << tuple.x << ',' << tuple.y << '}';
		return ss.str();
	}

	std::string toString(const hRotation& rot) {
		std::stringstream ss;
		ss << "Rotation{" << rot.tau << "*TAU}";
		return ss.str();
	}

	std::string toString(const Area& rect) {
		std::stringstream ss;
		ss << "Rect{" << rect.x1 << ',' << rect.y1 << ',' << rect.x2 << ',' << rect.y2 << '}';
		return ss.str();
	}

	template <typename T>
	std::string toString(const IMatrix<T>& mat, bool output_contents = false, bool line_breaks=true) {
		std::stringstream ss;
		char br = line_breaks ? '\n' : ' ';
		ss << "Matrix";
		if (output_contents) {
			ss << br;
			for (int y = mat.start().y; y < mat.end().y - 1; y++) {
				ss << '{';
				for (int x = mat.start().x; x < mat.end().x; x++)
					ss << mat.at(x, y) << ',';
				ss << mat.at(mat.x2, y) << '}' << br;
			}
		}
		else {
			ss << '{' << mat.x1 << ',' << mat.y1 << ',' << mat.x2 << ',' << mat.y2 << '}';
		}
		return ss.str();
	}

	template <typename T>
	std::ostream& streamOutRow(std::ostream& output, const IMatrix<T>& mat, int row) {
		for (int x = mat.start().x; x < mat.end().x; x++) {
			output << mat.at(x, row);
		}
		return output;
	}

	template <typename T>
	std::ostream& streamOutColumn(std::ostream& output, const IMatrix<T>& mat, int col) {
		for (int y = mat.start().y; y < mat.end().y; y++) {
			output << mat.at(col, y);
		}
		return output;
	}


	/// <summary>
	/// Create a formatted string of arbitrary size to be used as a horizontal line.
	/// </summary>
	/// <param name="length">Specifies the length of the line.</param>
	/// <param name="symbol">The character which composes the length of the horizontal line.</param>
	/// <param name="start">Character to start the line.</param>
	/// <param name="end">Character to end the line with.</param>
	/// <returns>A string of the formatted line.</returns>
	std::string makeStringLine(int length, char symbol, char start, char end) {
		std::stringstream ss;
		if (start) {
			length--;
			ss << start;
		}
		for (int i = 0; i < length-1; i++) {
			ss << symbol;
		}
		if (end) {
			ss << end;
		}
		else {
			ss << symbol;
		}
		return ss.str();
	}

	/// <summary>
	/// Convert a Matrix of values to a formatted table in a string.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="mat">A reference to the Matrix object to be used for outputting.</param>
	/// <param name="padding">Specifies the amount of space between each column.</param>
	/// <param name="enumerate">Show Column and Row index numbers.</param>
	/// <param name="top_line">Character to use for the top border of the table.</param>
	/// <param name="siding">Character to use for the left and right borders of the table.</param>
	/// <param name="corner">Character to use for each corner of the table.</param>
	/// <param name="bottom_line">Character to use for the bottom border of the table.</param>
	/// <returns>A string containing the formatted table ready for printing or writing.</returns>
	template <typename T>
	std::string toStringTable(const IMatrix<T>& mat, int padding,
			bool enumerate = false,
			char top_line = '-', char siding = '|', char corner = '+', char bottom_line = '-')
	{

		std::stringstream ss;

		int rail_pad = (int)(siding != 0);


		if (top_line)
			ss << makeStringLine((mat.width() + enumerate + 1) * padding + rail_pad, top_line, corner, corner) << '\n';

		if (enumerate) {
			ss << siding << std::setw(padding) << "";
			for (int x = mat.x1; x <= mat.x2; x++)
				ss << std::setw(padding) << x;
			ss << std::setw(padding) << siding << '\n';
		}

		for (int y = mat.y1; y <= mat.y2; y++) {
			ss << siding;
			if (enumerate) {
				ss << std::setw(padding) << y;
			}
			for (int x = mat.x1; x <= mat.x2; x++)
				ss << std::setw(padding) << mat.at(x, y);
			ss << std::setw(padding) << siding << '\n';
		}

		if (bottom_line)
			ss << makeStringLine((mat.width() + enumerate + 1) * padding + rail_pad, bottom_line, corner, corner) << '\n';

		return ss.str();
	}

	/// <summary>
	/// Convert a Mask of boolean values to a formatted table in a string.
	/// </summary>
	/// <param name="mat">A reference to the Mask object to be used for outputting.</param>
	/// <param name="padding">Specifies the amount of space between each column.</param>
	/// <param name="enumerate">Show Column and Row index numbers.</param>
	/// <param name="filled">Character to represent filled cells.</param>
	/// <param name="empty"><Character to represent empty cells/param>
	/// <param name="top_line">Character to use for the top border of the table.</param>
	/// <param name="siding">Character to use for the left and right borders of the table.</param>
	/// <param name="corner">Character to use for each corner of the table.</param>
	/// <param name="bottom_line">Character to use for the bottom border of the table.</param>
	/// <returns>A string containing the formatted table ready for printing or writing.</returns>
	/// <returns></returns>
	std::string toStringMask(const IMatrix<bool>& mat, int padding,
		bool enumerate = false,
		char filled = '#', char empty = '.',
		char top_line = '-', char siding = '|', char corner = '+', char bottom_line = '-')
	{

		std::stringstream ss;

		int rail_pad = (int)(siding != 0);


		if (top_line)
			ss << makeStringLine((mat.width() + enumerate + 1) * padding + rail_pad, top_line, corner, corner) << '\n';

		if (enumerate) {
			ss << siding << std::setw(padding) << "";
			for (int x = mat.x1; x <= mat.x2; x++)
				ss << std::setw(padding) << x;
			ss << std::setw(padding) << siding << '\n';
		}

		for (int y = mat.y1; y <= mat.y2; y++) {
			ss << siding;
			if (enumerate) {
				ss << std::setw(padding) << y;
			}
			for (int x = mat.x1; x <= mat.x2; x++)
				ss << std::setw(padding) << (mat.at(x, y) ? filled : empty);
			ss << std::setw(padding) << siding << '\n';
		}

		if (bottom_line)
			ss << makeStringLine((mat.width() + enumerate + 1) * padding + rail_pad, bottom_line, corner, corner) << '\n';

		return ss.str();
	}

} // namespace hrzn