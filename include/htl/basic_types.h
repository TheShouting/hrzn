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

#include <cmath>
#include <cassert>
#include <algorithm>
#include <array>

//#define HRZN_NOEXCEPTIONS
#ifndef HRZN_NOEXCEPTIONS
#include <stdexcept>
#endif // !HRZN_NOEXCEPTIONS

 
#define _TO_STRING_DEFERRED(n) #n
#define _TO_STRING(n) _TO_STRING_DEFERRED(n)
#define THROW_NOT_IMPLEMENTED(m) throw std::runtime_error(m " not implemented at line: " _TO_STRING(__LINE__) ", " _TO_STRING(__FILE__))

//#define HRZN_USE_DOUBLE_TYPE
//#define HRZN_USE_LONG_TYPE

// Set floating precision Size
//#define HRZN_USE_DOUBLE_TYPE
#ifdef HRZN_USE_DOUBLE_TYPE
	typedef double h_float;
#else
	typedef float h_float;
#endif // HRZN_USE_DOUBLE_TYPE

// Set integral size
//#define HRZN_USE_LONG_TYPE
#ifdef HRZN_USE_LONG_TYPE
	typedef long h_int;
#else
	typedef int h_int;
#endif // HRZN_USE_LONG_TYPE

// Aliased UNSIGNED INT type
typedef unsigned long int h_unsigned;

static constexpr h_float operator "" _hf(long double val) { return static_cast<::h_float>(val); }
static constexpr h_int operator "" _hi(unsigned long long int val) { return static_cast<::h_int>(val); }

namespace hrzn {

	static constexpr auto PI = 3.1415926535897932384626433832795_hf;
	static constexpr auto DEGTORAD = 0.01745329251994329576923690768489_hf;
	static constexpr auto RADTODEG = 57.295779513082320876798154814105_hf;
	static constexpr auto DEG = 360.0_hf;
	static constexpr auto RAD = 6.283185307179586476925286766559_hf;
	//static constexpr auto EPSILON = 1.192092896e-07_hf; // Epsilon is too small unless h_float=double 
	static constexpr auto EPSILON = 1.192092896e-06_hf; // TODO expand based on h_float precision

	static constexpr auto TOPLEFT = 0;
	static constexpr auto TOPRIGHT = 1;
	static constexpr auto LOWERRIGHT = 2;
	static constexpr auto LOWERLEFT = 3;

	static constexpr auto DIR_N = 0;
	static constexpr auto DIR_NE = 1;
	static constexpr auto DIR_E = 2;
	static constexpr auto DIR_SE = 3;
	static constexpr auto DIR_S = 4;
	static constexpr auto DIR_SW = 5;
	static constexpr auto DIR_W = 6;
	static constexpr auto DIR_NW = 7;


	namespace _lib {

		template<typename T, bool = std::is_floating_point_v<T>>
		struct vEpsilon {};

		template<typename T>
		struct vEpsilon<T, false> {
			inline static const T value = 1_hi;
		};

		template<typename T>
		struct vEpsilon<T, true> {
			inline static const T value = EPSILON;
		};


		inline constexpr h_float wrap_f(const h_float f) {
			//return std::fmod(std::fmod(f, 1._hf) + 1._hf, 1._hf);
			h_float a = f - (h_int)f + 1._hf;
			return a - (h_int)a;
		}
	}


	/******************************************************************************************************************
		Transform data types (Vectors/Tuples and Rotations)
	******************************************************************************************************************/

	/// <summary>
	/// A templated vector class for managing any 2D numeric data.
	/// </summary>
	/// <typeparam name="T">Template of a numeric type.</typeparam>
	template <typename T>
	struct i_tuple2 {

		T x, y;

		constexpr i_tuple2() : x(), y() {}

		template <typename TCast>
		constexpr i_tuple2(TCast _x, TCast _y) : x(static_cast<T>(_x)), y(static_cast<T>(_y)) {}

		explicit constexpr i_tuple2(T val) : x(val), y(val) {}

		constexpr i_tuple2(const i_tuple2<T>& t) = default;

		template <typename TCast>
		explicit constexpr i_tuple2(const i_tuple2<TCast>& t) : x(static_cast<T>(t.x)), y(static_cast<T>(t.y)) {}

		T operator [] (int i) const { 
#ifndef HRZN_NOEXCEPTIONS
			if (i != 0 && i != 1) throw std::out_of_range("Index is not 0 or 1.");
#endif // !HRZN_NOEXCEPTIONS
			return (&x)[i]; 
		}

		T& operator [] (int i) {
#ifndef HRZN_NOEXCEPTIONS
			if (i != 0 && i != 1) throw std::out_of_range("Index is not 0 or 1.");
#endif // !HRZN_NOEXCEPTIONS
			return (&x)[i];
		}

		operator bool() const { return x != T(0) || y != T(0); }

		template <typename TCast>
		void operator =(const i_tuple2<TCast>& t) {
			x = static_cast<T>(t.x);
			y = static_cast<T>(t.y);
		}

		/// Return a new object with the seperate sign values of the original x and y members.
		i_tuple2<T> signum() const { // TODO test signum axis
			return { (T(0) < this->x) - (this->x < T(0)), (T(0) < this->y) - (this->y < T(0)) };
		}

		void set(T _x, T _y) { this->x = _x; this->y = _y; }

		void set(i_tuple2<T> other) { this->x = other.x; this->y = other.y; }

		void shift(T xshift, T yshift) { this->x += xshift; this->y += yshift; }

		void shift(i_tuple2<T> const& amt) { this->shift(amt.x, amt.y); }

		void scale(T mag) { this->x *= mag; this->y *= mag; }

		i_tuple2<T> swizzle() const { return { this->y, this->x }; }

		auto length() const { return std::sqrt(this->x * this->x + this->y * this->y); }

		T length_sqr() const { return this->x * this->x + this->y * this->y; }

		T length_manhattan() const { return std::abs(this->x) + std::abs(this->y); }

		/// Return a unit vector of the original vector.
		i_tuple2<T> normal() const {
			h_float l = length();
			if (l < EPSILON)
				return { 0._hf, 0._hf };
			h_float il = 1._hf / l;
			return { static_cast<T>(this->x * il), static_cast<T>(this->y * il) };
		}

		/// Normailize in place and return the former length.
		double normalize() {
			double l = length();
			if (l < EPSILON)
				return 0.0;
			double il = 1.0 / l;
			this->x = static_cast<T>(this->x * il);
			this->y = static_cast<T>(this->y * il);
			return l;
		}

		/// Create a new object containing the smallest non-zero values of x and y.
		i_tuple2<T> epsilon_signed() const {
			return { _lib::vEpsilon<T>::value * (T)std::signbit((double)this->x) * -2 + 1, _lib::vEpsilon<T>::value * (T)std::signbit((double)this->y) * -2 + 1 };
		}

		/// Create a new object containing the smallest non-zero values of x and y.
		static constexpr i_tuple2<T> EPSILON2() { return { _lib::vEpsilon<T>::value, _lib::vEpsilon<T>::value }; }

		static constexpr i_tuple2<T> UP() { return { 0, 1 }; }

		static constexpr i_tuple2<T> DOWN() { return { 0, -1 }; }

		static constexpr i_tuple2<T> RIGHT() { return { 1, 0 }; }

		static constexpr i_tuple2<T> LEFT() { return { -1, 0 }; }

	}; // struct i_tuple2<T>


	// Operator overloads for i_tuple2 objects

	template <typename T>
	i_tuple2<T> operator - (const i_tuple2<T>& a) { return i_tuple2<T>(-a.x, -a.y); }

	template <typename T, typename TCast>
	bool operator ==(const i_tuple2<T>& a, const i_tuple2<TCast>& b) { return a.x == static_cast<T>(b.x) && a.y == static_cast<T>(b.y); }

	template <typename T, typename TCast>
	bool operator !=(const i_tuple2<T>& a, const i_tuple2<TCast>& b) { return !(a == b); }

	template <typename T, typename TCast>
	i_tuple2<T>& operator += (i_tuple2<T>& a, const i_tuple2<TCast>& b) {
		a.x += static_cast<T>(b.x); a.y += static_cast<T>(b.y);
		return a;
	}

	template <typename T, typename TCast>
	i_tuple2<T>& operator -= (i_tuple2<T>& a, const i_tuple2<TCast>& b) {
		a.x -= static_cast<T>(b.x); a.y -= static_cast<T>(b.y);
		return a;
	}

	template <typename T, typename TCast>
	i_tuple2<T> operator + (const i_tuple2<T>& a, const i_tuple2<TCast>& b) {
		return i_tuple2<T>(a.x + static_cast<T>(b.x), a.y + static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	i_tuple2<T> operator - (const i_tuple2<T>& a, const i_tuple2<TCast>& b) {
		return i_tuple2<T>(a.x - static_cast<T>(b.x), a.y - static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	i_tuple2<T> operator * (const i_tuple2<T>& a, const i_tuple2<TCast>& b) {
		return i_tuple2<T>(a.x * static_cast<T>(b.x), a.y * static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	i_tuple2<T> operator / (const i_tuple2<T>& a, const i_tuple2<TCast>& b) {
		return i_tuple2<T>(a.x / static_cast<T>(b.x), a.y / static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	i_tuple2<T> operator * (const i_tuple2<T>& a, TCast val) {
		static_assert(std::is_arithmetic_v<TCast>, "An arithmetic type is required for i_tuple2 operations");
		return i_tuple2<T>(a.x * static_cast<T>(val), a.y * static_cast<T>(val));
	}

	template <typename T, typename TCast>
	i_tuple2<T> operator / (const i_tuple2<T>& a, TCast val) {
		static_assert(std::is_arithmetic_v<TCast>, "An arithmetic type is required for i_tuple2 operations");
		return i_tuple2<T>(a.x / static_cast<T>(val), a.y / static_cast<T>(val));
	}

	template <typename T>
	i_tuple2<T> operator ~ (const i_tuple2<T>& a) { return i_tuple2<T>(a.y, a.x); }


#define MACRO_GENERATE_MATH_AND(F) \
	template <typename T> inline bool F(const i_tuple2<T>& a) { \
		return std::F(a.x) && std::F(a.y); }

#define MACRO_GENERATE_MATH_B(F) \
	template <typename T> inline i_tuple2<bool> F##_b(const i_tuple2<T>& a) { \
		return {std::F(a.x), std::F(a.y)}; }

#define MACRO_GENERATE_MATH_F(F) \
	template <typename T> inline i_tuple2<T> F(const i_tuple2<T>& a) { \
		return { std::F(a.x), std::F(a.y) }; }

	MACRO_GENERATE_MATH_F(sqrt);
	MACRO_GENERATE_MATH_F(abs);
	MACRO_GENERATE_MATH_F(round);
	MACRO_GENERATE_MATH_F(floor);
	MACRO_GENERATE_MATH_F(ceil);
	MACRO_GENERATE_MATH_F(trunc);

	MACRO_GENERATE_MATH_AND(isnormal);
	MACRO_GENERATE_MATH_AND(isfinite);

	MACRO_GENERATE_MATH_B(isnormal);
	MACRO_GENERATE_MATH_B(isfinite);
	MACRO_GENERATE_MATH_B(isnan);
	MACRO_GENERATE_MATH_B(isinf);
	MACRO_GENERATE_MATH_B(signbit);

#undef MACRO_GENERATE_MATH_F
#undef MACRO_GENERATE_MATH_AND
#undef MACRO_GENERATE_MATH_B

	/// Type aliases for Tuples of FLOAT types.
	using vector2 = i_tuple2<h_float>;
	/// Type alias for Tuples of INT types.
	using point2 = i_tuple2<h_int>;

	/// <summary>
	/// An array of unit vectors containing the vertices of a quadrilateral.
	/// </summary>
	static constexpr std::array<vector2, 4> h_quad = {
		vector2{0._hf, 0._hf},
		vector2{1._hf, 0._hf},
		vector2{1._hf, 1._hf},
		vector2{0._hf, 1._hf}
	};

	/// <summary>
	/// An array contained the point offsets for each corner in a box or area.
	/// </summary>
	static constexpr std::array<point2, 4> h_corner = {
		point2{0_hi, 0_hi},
		point2{1_hi, 0_hi},
		point2{1_hi, 1_hi},
		point2{0_hi, 1_hi}
	};

	/// <summary>
	/// An array containing all offsets for a 4-way neighborhood.
	/// </summary>
	static constexpr std::array<point2, 5> h_neighborhood4 = {
		point2{ 0_hi, -1_hi},
		point2{ 1_hi,  0_hi},
		point2{ 0_hi,  1_hi},
		point2{-1_hi,  0_hi},
		point2{ 0_hi,  0_hi}
	};

	/// <summary>
	/// An array containing all offsets for a 4-way neighborhood.
	/// </summary>
	static constexpr std::array<point2, 9> h_neighborhood8 = {
		point2{ 0_hi, -1_hi},
		point2{ 1_hi, -1_hi},
		point2{ 1_hi,  0_hi},
		point2{ 1_hi,  1_hi},
		point2{ 0_hi,  1_hi},
		point2{-1_hi,  1_hi},
		point2{-1_hi,  0_hi},
		point2{-1_hi, -1_hi},
		point2{ 0_hi,  0_hi}
	};


	/******************************************************************************************************************
		rect_i data types
	******************************************************************************************************************/

	/// <summary>
	/// A class for managing coordinates of a 2D area.
	/// </summary>
	template <typename T>
	struct i_rectangle {

		T x, y, w, h;

		constexpr i_rectangle() : x(0), y(0), w(0), h(0) {}

		constexpr i_rectangle(T _w, T _h) : x(0), y(0), w(_w), h(_h) {}

		constexpr i_rectangle(T _x, T _y, T _w, T _h) : x(_x), y(_y), w(_w), h(_h) {}

		explicit constexpr i_rectangle(i_tuple2<T> pos, i_tuple2<T> size) : x(pos.x), y(pos.y), w(size.x), h(size.y) {}

		constexpr i_rectangle(const i_rectangle<T>& r) = default;

		template <typename TCast>
		explicit constexpr i_rectangle(const i_rectangle<TCast> & r) : x(static_cast<T>(r.x)), y(static_cast<T>(r.y)), w(static_cast<T>(r.w)), h(static_cast<T>(r.h)) {}

		operator bool() const {
			return valid();
		}

		bool valid() const {
			return this->w > 0 && this->h > 0;
		}

		i_rectangle<T> normalized() const {
			return i_rectangle<T>{ 0, 0, std::abs(this->w), std::abs(this->h) };
		}

		i_tuple2<T> clamp(i_tuple2<T> pt) const {
			return { std::min(this->x + this->w, std::max(this->x, pt.x)), std::min(this->y + this->h, std::max(this->y, pt.y))};
		}

		i_tuple2<T> wrap(i_tuple2<T> pt) const {
			T x = ((T)this->w + ((pt.x - this->x) % (T)this->w)) % (T)this->w + this->x;
			T y = ((T)this->h + ((pt.y - this->y) % (T)this->h)) % (T)this->h + this->y;
			return { x, y };
		}

		T width() const {
			return std::abs(this->w);
		}

		T height() const {
			return std::abs(this->h);
		}

		i_tuple2<T> position() const {
			return { this->x, this->y };
		}

		i_tuple2<T> size() const {
			return { this->w, this->h };
		}

		i_tuple2<T> first() const {
			return { this->x, this->y };
		}

		i_tuple2<T> last() const { return { this->x + this->w, this->y + this->h }; }

		T area() const {
			return this->w * this->h;
		}

		i_tuple2<T> corner(std::size_t i) const {
#ifndef HRZN_NOEXCEPTIONS
			if (i < 0 || i > 3) throw std::out_of_range("Index is not a valid corner.");
#endif // !HRZN_NOEXCEPTIONS
			//return { this->x + h_corner[i].x * (this->w - 1), this->y + h_corner[i].y * (this->h - 1)};
			return { x + ((w  - _lib::vEpsilon<T>::value) * T(i % 2)), y + ((h - _lib::vEpsilon<T>::value) * T(i / 2) ) };
		}

		i_tuple2<T> center() const {
			return { this->w / 2 + this->x, this->h / 2 + this->y };
		}

		bool contains(T px, T py) const {
			return px >= this->x && px < (this->x + this->w)  && py >= this->y && py < this->y + this->h;
		}

		bool contains(i_tuple2<T> pt) const {
			return this->contains(pt.x, pt.y);
		}

		bool contains(i_rectangle<T> other) const {
			return
				other.valid() &&
				other.x >= this->x && 
				other.x + other.w < this->x + this->w && 
				other.y >= this->y && 
				other.y + other.h < this->y + this->h;
		}

		static constexpr i_rectangle<T> from_corners(i_tuple2<T> a, i_tuple2<T> b) {
			T x = std::min(a.x, b.x);
			T y = std::min(a.y, b.y);
			T w = std::max(a.x, b.x) - x;
			T h = std::max(a.x, b.x) - y;
			return i_rectangle<T>{x, y, w, h};
		}

	}; // struct rect_i


	template <typename T>
	inline bool operator==(i_rectangle<T> a, i_rectangle<T> b) {
		return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
	}

	template <typename T>
	inline i_rectangle<T> operator+(i_rectangle<T> a, i_tuple2<T> b) {
		return i_rectangle<T>{ a.x + b.x, a.y + b.y, a.w, a.h };
	}

	template <typename T>
	inline i_rectangle<T> operator-(i_rectangle<T> a, i_tuple2<T> b) {
		return i_rectangle<T>{ a.x - b.x, a.y - b.y, a.w, a.h };
	}


#define H_FOREACH_POINT(A, X, Y) for (h_int X, Y = A.y; Y < (A.y + A.h); ++Y) for (X = A.x; X < (A.x + A.w); ++X) 

	using rect_i = i_rectangle<h_int>;
	using rect_f = i_rectangle<h_float>;


	/// <summary>
	/// Create a new Area based on the overlap of two other Area objects.
	/// </summary>
	template <typename T>
	inline i_rectangle<T> intersect(i_rectangle<T> a, i_rectangle<T> b) {
		auto x = std::max(a.x, b.x);
		auto y = std::max(a.y, b.y);
		auto w = std::min(a.x + a.w, b.x + b.w) - x;
		auto h = std::min(a.y + a.h, b.y + b.h) - y;
		return { x, y, w, h };
	}

	/// <summary>
	/// Return true if both <type>rect_i</type> objects overlap.
	/// </summary>
	template <typename T>
	inline bool overlap(i_rectangle<T> a, i_rectangle<T> b) {
		return !(a.x > b.x + b.w || a.y > b.y + b.h || b.x > a.x + a.w || b.y > a.y + a.h);
	}

	/// <summary>
	/// Is true if <type>rect_i</type> b is completely contained within <type>rect_i</type> a.
	/// </summary>
	template <typename T>
	inline bool contains(i_rectangle<T> a, i_rectangle<T> b) {
		return b.x >= a.x && b.y >= a.y && b.last().x <= a.last().x && b.last().y <= a.last().y;
	}

	/// <summary>
	/// Is true if <type>poin2</type> b is completely contained within <type>rect_i</type> a.
	/// </summary>
	template <typename T>
	inline bool contains(i_rectangle<T> a, point2 b) {
		return b.x >= a.x && b.y >= a.y && b.x < a.last().x  && b.y < a.last().y;
	}

	/// <summary>
	/// Is true if <type>point2</type> is equal to any edge position of <type>rect_i</type>.
	/// </summary>
	template <typename T>
	inline bool is_edge(i_rectangle<T> area, point2 pos) {
		return pos.x == area.x || pos.x == area.last().x - 1_hi || pos.y == area.y || pos.y == area.last() - 1_hi;
	}

	/// <summary>
	/// Swap x and y values.
	/// </summary>
	template<typename T>
	inline i_tuple2<T> swizzle(i_tuple2<T> t) {
		return i_tuple2<T>{ t.y, t.x };
	}

	/// <summary>
	/// Swap width and height values (same as rotating 90 deg)
	/// </summary>
	template <typename T>
	inline i_rectangle<T> swizzle(i_rectangle<T> a) {
		return i_rectangle<T> { a.y, a.x, a.h, a.w };
	}

	/// <summary>
	/// Snaps a point to a recrangle edge if it is out of bounds.
	/// </summary>
	template <typename T>
	inline i_tuple2<T> clamp_point(i_tuple2<T> p, i_rectangle<T> area) {
		return i_tuple2<T>{
			std::min(std::max(p.x, area.x), area.x + area.w - _lib::vEpsilon<T>::value),
			std::min(std::max(p.y, area.y), area.y + area.h - _lib::vEpsilon<T>::value)
		 };
	}

	/// <summary>
	/// Wraps a point to the oppposite edge of an area if it is out of bounds.
	/// </summary>
	template <typename T>
	inline i_tuple2<T> wrap_point(i_tuple2<T> p, i_rectangle<T> area) {
		return i_tuple2<T>{
			((p.x - area.x) % area.w + area.w) % area.w + area.x,
			((p.y - area.y) % area.h + area.h) % area.h + area.y
		};
	}

	/// <summary>
	/// If a dimension is an odd number, add 1 to make it even.
	/// </summary>
	/// <returns></returns>
	template <typename T>
	inline rect_i make_even(rect_i r) { // TODO test make_even method for rect
		return rect_i{ r.x, r.y, r.w + (r.w % 2_hi), r.h + (r.h % 2_hi) };
	}

	/// <summary>
	/// Split a rect_i into 4 seperate reperate rectangles based on the defined center point.
	/// </summary>
	/// <param name="area">The <type>rect_i</type> to be split.</param>
	/// <param name="center">The center point were the split occurs.</param>
	/// <returns></returns>
	template <typename T>
	inline constexpr std::array<i_rectangle<T>, 4> quad_split(i_rectangle<T> area, point2 center) {
		return std::array<i_rectangle<T>, 4> {
			i_rectangle<T>{ area.x, area.y,		center.x - area.x,			center.y - area.y			},
			i_rectangle<T>{ center.x, area.y,	area.x + area.w - center.x,	center.y - area.y			},
			i_rectangle<T>{ area.x, center.y,	center.x - area.x,			area.y + area.h - center.y	},
			i_rectangle<T>{ center.x, center.y,	area.x + area.w - center.x,	area.y + area.h - center.y	}
		};
	}

	/// <summary>
	/// Split the area of a <type>rect_i</type> along its longest axis into two smaller ones.
	/// </summary>
	/// <param name="area">The <type>rect_i</type> to be split</param>
	/// <returns>A container with the new URect objects. If <paramref name="rec"/> is only a single cell (width and height are equal to 1), then both are simply a copy of the orignal parameter. </returns>
	template <typename T>
	inline std::array<i_rectangle<T>, 2> split(i_rectangle<T> area) {
		i_rectangle<T> a1 = area;
		i_rectangle<T> a2 = area;
		point2 cp = area.center();
		if (area.h > area.w) {
			a1.h = cp.y - area.h;
			a2.y = cp.y;
			a2.h = area.y + area.h - cp.y;
		} else if (area.valid()) {
			a1.w = cp.x - area.w;
			a2.x = cp.x;
			a2.w = area.x + area.w - cp.x;
		}
		return { a1, a2 };
	}

	template <typename T>
	i_rectangle<T> resize_from_center(i_rectangle<T> r, T _w, T _h) {
		if (r.valid()) {
			point2 ctr = r.center();
			return i_rectangle<T>{
				r.x = ctr.x - _w / 2,
				r.y = ctr.y - _h / 2,
				r.w = _w,
				r.h = _h
			};
		}
		return r;
	}

	template <typename T>
	i_rectangle<T> resize_from_center(i_rectangle<T> r, point2 size) {
		return resize_from_center<T>(r, size.x, size.y);
	}


	/******************************************************************************************************************
		angle data types
	******************************************************************************************************************/

	/// <summary>
	/// A class that stores an angle as number of rotations.
	/// </summary>
	struct angle {

		h_float tau;

		constexpr angle() : tau(0._hf) {}

		constexpr angle(h_float _tau) : tau(_lib::wrap_f(_tau)) {}

		constexpr angle(const angle& a) = default;

		inline auto deg() const { return tau * DEG; }

		inline auto rad() const { return tau * RAD; }

		auto rotate_deg(h_float deg) {
			tau = _lib::wrap_f(deg / DEG + tau);
			return tau * DEG;
		}

		auto rotate_rad(h_float rad) {
			tau = _lib::wrap_f(rad / RAD + tau);
			return tau * RAD;
		}

		void set(h_float t) {
			tau = _lib::wrap_f(t);
		}

		void set_deg(h_float deg) {
			tau = _lib::wrap_f(deg / DEG);
		}

		void set_rad(h_float rad) {
			tau = _lib::wrap_f(rad / RAD);
		}

		void spin(h_float rotations) {
			tau = _lib::wrap_f(tau + rotations);
		}

		angle flip() const {
			return angle(tau + 0.5_hf);
		}

		angle invert() const {
			angle a;
			a.tau = 1._hf - tau;
			return a;
		}

		vector2 get_forward_vector(h_float length = 1._hf) const {
			return vector2(std::sin(rad()) * length, std::cos(rad()) * length);
		}

		vector2 get_right_vector(h_float length = 1._hf) const {
			return vector2(std::sin(rad() + PI * 0.5_hf) * length, std::cos(rad() + PI * 0.5_hf) * length);
		}

		vector2 rotate_vector(vector2 const& vec) const {
			h_float a = rad();
			h_float cs = std::cos(a);
			h_float sn = std::sin(a);
			return { cs * vec.x + sn * vec.y, sn * -vec.x + cs * vec.y };
		}

		vector2 unrotate_vector(vector2 const& vec) const {
			h_float a = -rad();
			h_float cs = std::cos(a);
			h_float sn = std::sin(a);
			return { cs * vec.x + sn * vec.y, sn * -vec.x + cs * vec.y };
		}

		static constexpr angle from_degrees(h_float deg) { return angle(deg / DEG); }

		static constexpr angle from_radians(h_float rad) { return angle(rad / RAD); }

		template <typename T>
		static constexpr angle from_vector(i_tuple2<T> vec) {
			h_float tau = std::atan2(vec.y, vec.x) / RAD + 0.75_hf;
			return angle(-tau);
		}

		static constexpr angle HALF() { return angle(0.5_hf); }

		static constexpr angle UP() { return angle(0._hf); }

		static constexpr angle DOWN() { return angle(0.5_hf); }

		static constexpr angle RIGHT() { return angle(0.25_hf); }

		static constexpr angle LEFT() { return angle(0.75_hf); }

	}; // struct angle


	/// <summary>
	/// Get the smallest rotation between two angles.
	/// </summary>
	inline angle difference(angle a, angle b) {
		/*h_float phi = std::fmod(std::abs(a.tau - b.tau), 1._hf);
		a.tau = phi > 0.5_hf ? 1._hf - phi : phi;
		return a;*/
		h_float phi = a.tau - b.tau + 0.5_hf;
		return { std::abs(phi - std::floor(phi) - 0.5_hf) };
	}

	/// <summary>
	/// Returns true if the shortest angle for A to reach B is clockwise (positive angle).
	/// </summary>
	inline bool direction(angle from, angle to) {
		return std::fmod(to.tau - from.tau + 1.5_hf, 1._hf) > 0.5_hf;
	}

	/// <summary>
	/// Compare equality of two angles using the library defined epsilon value.
	/// </summary>
	inline bool compare(angle a, angle b) {
		return difference(a, b).tau < EPSILON;
	}

	/// <summary>
	/// Compare equality of two angles using a user defined epsilon value.
	/// </summary>
	inline bool compare(angle a, angle b, h_float epsilon) {
		return difference(a, b).tau < epsilon;
	}

	inline bool operator == (const angle& a, const angle& b) { return a.tau == b.tau; }
	inline bool operator < (const angle& a, const angle& b) { return a.tau < b.tau; }

	inline angle operator ~ (const angle& a) { return a.invert(); }
	inline angle operator - (const angle& a) { return a.flip(); }

	inline angle operator + (const angle& a, const angle& b) { return { a.tau + b.tau }; }
	inline angle operator + (const angle& a, const h_float& val) { return { a.tau + val }; }

	inline angle operator - (const angle& a, const angle& b) { return { a.tau - b.tau }; }
	inline angle operator - (const angle& a, const h_float& val) { return { a.tau - val }; }

	inline angle operator * (const angle& a, const angle& b) { return { a.tau * b.tau }; }
	inline angle operator * (const angle& a, const h_float& val) { return { a.tau * val }; }

	inline angle operator / (const angle& a, const angle& b) { return { a.tau / b.tau }; }
	inline angle operator / (const angle& a, const h_float& val) { return { a.tau / val }; }


} // namespace hrzn

#undef THROW_NOT_IMPLEMENTED
