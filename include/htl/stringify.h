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

#include "basic_types.h"
#include "containers.h"

#include <sstream>
#include <iomanip>

namespace hrzn {

	template <typename T>
	inline std::ostream& operator<<(std::ostream& output, const i_tuple2<T>& tuple) {
		output << tuple.x << tuple.y;
		return output;
	}

	template <typename T>
	inline std::istream& operator>>(std::istream& input, i_tuple2<T>& tuple) {
		input >> tuple.x >> tuple.y;
		return input;
	}

	inline std::ostream& operator<<(std::ostream& output, const angle& rot) {
		output << rot.tau;
		return output;
	}

	inline std::istream& operator>>(std::istream& input, angle& rot) {
		input >> rot.tau;
		return input;
	}

	inline std::ostream& operator<<(std::ostream& output, const rect_i& rect) {
		output << rect.x << rect.y << rect.w << rect.h;
		return output;
	}

	inline std::istream& operator>>(std::istream& input, rect_i& rect) {
		input >> rect.x >> rect.y >> rect.w >> rect.h;
		return input;
	}

	template <typename T>
	inline std::string to_string(const i_tuple2<T>& tuple) {
		std::stringstream ss;
		ss << "Tuple{" << tuple.x << ',' << tuple.y << '}';
		return ss.str();
	}

	inline std::string to_string(const angle& rot) {
		std::stringstream ss;
		ss << "Rotation{" << rot.tau << "*TAU}";
		return ss.str();
	}

	inline std::string to_string(const rect_i& rect) {
		if (rect) {
			std::stringstream ss;
			ss << "i_rectangle{ x" << rect.x << ", y" << rect.y << ", w" << rect.w << ", h" << rect.h << "  }";
			return ss.str();
		}
		else {
			return "i_rectangle{empty}";
		}
	}

	template <typename T>
	inline std::string to_string(const I_Map<T>& mat, bool output_contents = false, bool line_breaks=true) {
		std::stringstream ss;
		char br = line_breaks ? '\n' : ' ';
		ss << "I_Map";
		if (output_contents) {
			ss << br;
			for (int y = mat.first().y; y < mat.last().y; y++) {
				ss << '{';
				for (int x = mat.first().x; x < mat.last().x - 1; x++)
					ss << mat.at(x, y) << ',';
				ss << mat.at(mat.x2 - 1, y) << '}' << br;
			}
		}
		else {
			ss << "{ first: " << mat.x1() << ", " << mat.y1() << " | last: " << mat.x2() << ", " << mat.y2() << " }";
		}
		return ss.str();
	}

	template <typename T>
	inline std::ostream& stream_out_row(std::ostream& output, const I_Map<T>& mat, int row) {
		for (int x = mat.first().x; x < mat.last().x; x++) {
			output << mat.at(x, row);
		}
		return output;
	}

	template <typename T>
	inline std::ostream& stream_out_column(std::ostream& output, const I_Map<T>& mat, int col) {
		for (int y = mat.first().y; y < mat.last().y; y++) {
			output << mat.at(col, y);
		}
		return output;
	}


	/// <summary>
	/// Create a formatted string of arbitrary size to be used as a horizontal line.
	/// </summary>
	/// <param name="length">Specifies the length of the line.</param>
	/// <param name="symbol">The character which composes the length of the horizontal line.</param>
	/// <param name="first">Character to first the line.</param>
	/// <param name="last">Character to last the line with.</param>
	/// <returns>A string of the formatted line.</returns>
	inline std::string make_string_line(int length, char symbol, char first, char last) {
		std::stringstream ss;
		if (first) {
			length--;
			ss << first;
		}
		for (int i = 0; i < length-1; i++) {
			ss << symbol;
		}
		if (last) {
			ss << last;
		}
		else {
			ss << symbol;
		}
		return ss.str();
	}


	/// <summary>
	/// Styling options for displaying string table.
	/// </summary>
	struct string_table_style {
		unsigned int padding = 2;
		bool enumerate = false;
		char filled = '#';
		char empty = '.';
		char top_line = '-';
		char siding = '|';
		char corner = '+';
		char bottom_line = '-';
		int precision = 2;
	}; // struct string_table_style


	/// <summary>
	/// Convert a Matrix of values to a formatted table in a string.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns>A string containing the formatted table ready for printing or writing.</returns>
	template <typename T>
	inline std::string to_string_table(const I_Map<T>& mat, const string_table_style & style = string_table_style())
	{
		std::stringstream ss;
		ss.precision(style.precision);
		ss.setf(std::ios::fixed);

		int rail_pad = (int)(style.siding != 0);

		// Top table border
		if (style.top_line)
			ss << make_string_line((mat.width() + style.enumerate + 1) * style.padding + rail_pad, style.top_line, style.corner, style.corner) << '\n';

		// Column numbering
		if (style.enumerate) {
			ss << style.siding << std::setw(style.padding) << "";
			for (int x = mat.x1; x < mat.x2; x++)
				ss << std::setw(style.padding) << x;
			ss << std::setw(style.padding) << style.siding << '\n';
		}

		// I_Map content
		for (int y = mat.y1; y < mat.y2; y++) {
			ss << style.siding;
			if (style.enumerate) {
				ss << std::setw(style.padding) << y;
			}
			for (int x = mat.x1; x < mat.x2; x++)
				ss << std::setw(style.padding) << mat.at(x, y);
			ss << std::setw(style.padding) << style.siding << '\n';
		}

		// Bottom table border
		if (style.bottom_line)
			ss << make_string_line((mat.width() + style.enumerate + 1) * style.padding + rail_pad, style.bottom_line, style.corner, style.corner) << '\n';

		return ss.str();
	}

	/// <summary>
	/// Convert a Mask of boolean values to a formatted table in a string.
	/// </summary>
	/// <returns>A string containing the formatted table ready for printing or writing.</returns>
	/// <returns></returns>
	inline std::string to_string_mask(const I_Map<bool>& mat, const string_table_style & style = string_table_style())
	{

		std::stringstream ss;

		int rail_pad = (int)(style.siding != 0);


		if (style.top_line)
			ss << make_string_line((mat.width() + style.enumerate + 1) * style.padding + rail_pad, style.top_line, style.corner, style.corner) << '\n';

		if (style.enumerate) {
			ss << style.siding << std::setw(style.padding) << "";
			for (int x = mat.x; x < mat.last().x; x++)
				ss << std::setw(style.padding) << x;
			ss << std::setw(style.padding) << style.siding << '\n';
		}

		for (int y = mat.y; y < mat.last().y; y++) {
			ss << style.siding;
			if (style.enumerate) {
				ss << std::setw(style.padding) << y;
			}
			for (int x = mat.x; x < mat.last().y; x++)
				ss << std::setw(style.padding) << (mat.at(x, y) ? style.filled : style.empty);
			ss << std::setw(style.padding) << style.siding << '\n';
		}

		if (style.bottom_line)
			ss << make_string_line((mat.width() + style.enumerate + 1) * style.padding + rail_pad, style.bottom_line, style.corner, style.corner) << '\n';

		return ss.str();
	}

} // namespace hrzn