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


		inline constexpr h_float normalizef(const h_float f) {
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

	/// <summary>
	/// Normalized angles.
	/// </summary>
	struct angle {

		h_float tau;

		constexpr angle() : tau(0._hf) {}

		constexpr angle(h_float _tau) : tau(_lib::normalizef(_tau)) {}

		constexpr angle(const angle & a) = default;

		inline auto deg() const { return tau * DEG; }

		inline auto rad() const { return tau * RAD; }

		auto rotate_deg(h_float deg) {
			tau = _lib::normalizef(deg / DEG + tau);
			return tau * DEG;
		}

		auto rotate_rad(h_float rad) {
			tau = _lib::normalizef(rad / RAD + tau);
			return tau * RAD;
		}

		void set(h_float t) {
			tau = _lib::normalizef(t);
		}

		void set_deg(h_float deg) {
			tau = _lib::normalizef(deg / DEG);
		}

		void set_rad(h_float rad) {
			tau = _lib::normalizef(rad / RAD);
		}

		void spin(h_float rotations) {
			tau = _lib::normalizef(tau + rotations);
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

	inline angle operator ~ (const angle & a) { return a.invert(); }
	inline angle operator - (const angle & a) { return a.flip(); }

	inline angle operator + (const angle & a, const angle & b) { return { a.tau + b.tau }; }
	inline angle operator + (const angle & a, const h_float & val) { return { a.tau + val }; }

	inline angle operator - (const angle& a, const angle& b) { return { a.tau - b.tau }; }
	inline angle operator - (const angle& a, const h_float& val) { return { a.tau - val }; }
	
	inline angle operator * (const angle& a, const angle& b) { return { a.tau * b.tau }; }
	inline angle operator * (const angle& a, const h_float& val) { return { a.tau * val }; }
	
	inline angle operator / (const angle& a, const angle& b) { return { a.tau / b.tau }; }
	inline angle operator / (const angle& a, const h_float& val) { return { a.tau / val }; }


	/******************************************************************************************************************
		rectangle data types
	******************************************************************************************************************/

	/// <summary>
	/// A class for managing coordinates of a 2D area.
	/// </summary>
	struct rectangle { // TODO make abstract

		h_int x1, y1, x2, y2; // TODO refactor to use x, y, w, h

		constexpr rectangle() : x1(INT_MAX), y1(INT_MAX), x2(INT_MIN), y2(INT_MIN) {}

		constexpr rectangle(h_unsigned w, h_unsigned h) : x1(0), y1(0), x2((h_int)w), y2((h_int)h) {}

		constexpr rectangle(h_int _x1, h_int _y1, h_int _x2, h_int _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

		explicit constexpr rectangle(point2 p1, point2 p2) : x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y) {}

		rectangle& operator +=(point2 dist) {
			move(dist.x, dist.y);
			return *this;
		}

		rectangle& operator -=(point2 dist) {
			move(-dist.x, -dist.y);
			return *this;
		}

		operator bool() const {
			return x1 < x2 && y1 < y2;
		}

		bool valid() const {
			return x1 < x2 && y1 < y2;
		}

		void move(h_int move_x, h_int move_y) {
			x1 += move_x;
			y1 += move_y;
			x2 += move_x;
			y2 += move_y;
		}

		virtual void resize(h_int x_1, h_int y_1, h_int x_2, h_int y_2) {
			x1 = std::min(x_1, x_2);
			x2 = std::max(x_1, x_2);
			y1 = std::min(y_1, y_2);
			y2 = std::max(y_1, y_2);
		}

		void resize(point2 size) {
			if (size) {
				resize(std::min(x1, x1 + size.x), std::min(y1, y1 + size.y), std::max(x1, x1 + size.x), std::max(y1, y1 + size.y));
			}
		}

		void resize_from_center(point2 size) {
			resize_from_center(size.x, size.y);
		}

		void resize_from_center(h_unsigned size) {
			resize_from_center(size, size);
		}

		void resize_from_center(h_unsigned w, h_unsigned h) {
			if (valid()) {
				point2 ctr = center();
				resize(ctr.x - w / 2, ctr.y - h / 2, x1 + w, y1 + h);
			}
		}

		rectangle normalized() const {
			return rectangle(0, 0, x2 - x1, y2 - y1);
		}

		point2 clamp(point2 pt) const {
			return { std::min(x2, std::max(x1, pt.x)), std::min(y2, std::max(y1, pt.y)) };
		}

		point2 wrap(point2 pt) const {
			h_int x = ((h_int)width() + ((pt.x - x1) % (h_int)width())) % (h_int)width() + x1;
			h_int y = ((h_int)height() + ((pt.y - y1) % (h_int)height())) % (h_int)height() + y1;
			return { x, y };
		}

		std::size_t width() const {
			return std::abs(x2 - x1);
		}

		std::size_t height() const {
			return std::abs(y2 - y1);
		}

		point2 dimensions() const {
			return { width(), height() };
		}

		point2 first() const {
			return { x1, x1 };
		}

		point2 last() const {
			return { x2, y2 };
		}

		std::size_t area() const {
			return width() * height();
		}

		point2 corner(int i) const {
#ifndef HRZN_NOEXCEPTIONS
			if (i < 0 || i > 3) throw std::out_of_range("Index is not a valid corner.");
#endif // !HRZN_NOEXCEPTIONS
			return { x1 + h_corner[i].x * (width() - 1), y1 + h_corner[i].y * (height() - 1) };
		}

		point2 center() const {
			return { (x2 - x1) / 2 + x1, (y2 - y1) / 2 + y1 };
		}

		bool contains(point2 pt) const {
			return this->contains(pt.x, pt.y);
		}

		bool contains(h_int x, h_int y) const {
			return x >= x1 && x < x2 && y >= y1 && y < y2;
		}

		bool contains(rectangle other) {
			return false; // TODO implement bool contains(rect)
		}

	}; // struct rectangle

	inline bool operator==(rectangle a, rectangle b) {
		return a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2;
	}

	inline rectangle operator+(rectangle a, point2 b) {
		return rectangle(a.first() + b, a.last() + b);
	}

	inline rectangle operator-(rectangle a, point2 b) {
		return rectangle(a.first() - b, a.last() - b);
	}

	/// <summary>
	/// Create a new Area based on the overlap of two other Area objects.
	/// </summary>
	inline rectangle intersect(rectangle a, rectangle b) {
		return rectangle(std::max(a.x1, b.x1), std::max(a.y1, b.y1), std::min(a.x2, b.x2), std::min(a.y2, b.y2));
	}

	/// <summary>
	/// Return true if both <type>rectangle</type> objects overlap.
	/// </summary>
	inline bool overlap(rectangle a, rectangle b) {
		return !(a.x1 > b.x2 || a.y1 > b.y2 || b.x1 > a.x2 || b.y1 > a.y2);
	}

	/// <summary>
	/// Is true if <type>rectangle</type> b is completely contained within <type>rectangle</type> a.
	/// </summary>
	inline bool contains(rectangle a, rectangle b) {
		return b.x1 >= a.x1 && b.y1 >= a.y1 && b.x2 <= a.x2 && b.y2 <= a.x2;
	}

	/// <summary>
	/// Is true if <type>poin2</type> b is completely contained within <type>rectangle</type> a.
	/// </summary>
	inline bool contains(rectangle a, point2 b) {
		return b.x >= a.x1 && b.y >= a.y1 && b.x < a.x2 && b.y < a.x2;
	}

	/// <summary>
	/// Is true if <type>point2</type> is equal to any edge position of <type>rectangle</type>.
	/// </summary>
	inline bool is_edge(rectangle area, point2 pos) {
		return pos.x == area.x1 || pos.x == area.x2 - 1_hi || pos.y == area.y1 || pos.y == area.y2 - 1_hi;
	}

	/// <summary>
	/// Swap x and y values.
	/// </summary>
	template<typename T>
	inline i_tuple2<T> swizzle(const i_tuple2<T>& t) {
		return { t.y, t.x };
	}

	/// <summary>
	/// Swap width and height values (same as rotating 90 deg)
	/// </summary>
	inline rectangle swizzle(const rectangle& a) {
		return rectangle(a.y1, a.x1, a.y2, a.x2);
	}

	/// <summary>
	/// Snaps a point to an area edge if it is out of bounds.
	/// </summary>
	inline point2 clamp_point(const point2& p, const rectangle& area) {
		h_int x = std::min(std::max(p.x, area.x1), area.x2 - 1_hi);
		h_int y = std::min(std::max(p.y, area.y1), area.y2 - 1_hi);
		return { x, y };
	}

	/// <summary>
	/// Wraps a point to the oppposite edge of an area if it is out of bounds.
	/// </summary>
	inline point2 wrap_point(const point2& p, const rectangle& area) {
		h_int x = (p.x % area.width() + area.width()) % area.width();
		h_int y = (p.y % area.height() + area.height()) % area.height();
		return { x, y };
	}


	/// <summary>
	/// Split a rectangle into 4 seperate reperate rectangles based on the defined center point.
	/// </summary>
	/// <param name="area">The <type>rectangle</type> to be split.</param>
	/// <param name="center">The center point were the split occurs.</param>
	/// <returns></returns>
	inline constexpr std::array<rectangle, 4> quad_split(rectangle area, point2 center) {
		return std::array<rectangle, 4> {
			rectangle{ area.x1, area.y1, center.x, center.y },
			rectangle{ center.x, area.y1, area.x2, center.y },
			rectangle{ area.x1, center.y, center.x, area.y2 },
			rectangle{ center.x, center.y, area.x2, area.y2 }
		};
	}


	/// <summary>
	/// Split the area of a <type>rectangle</type> along its longest axis into two smaller ones.
	/// </summary>
	/// <param name="area">The <type>rectangle</type> to be split</param>
	/// <returns>A container with the new URect objects. If <paramref name="rec"/> is only a single cell (width and height are equal to 1), then both are simply a copy of the orignal parameter. </returns>
	inline std::array<rectangle, 2> split(rectangle area) {
		rectangle a1 = area;
		rectangle a2 = area;
		point2 cp = area.center();
		if (area.height() > 1 && area.height() > area.width()) {
			a1.y2 = cp.y;
			a2.y1 = cp.y;
		} else if (area.width() > 1) {
			a1.x2 = cp.x;
			a2.x1 = cp.x;
		}
		return { a1, a2 };
	}

} // namespace hrzn

#undef THROW_NOT_IMPLEMENTED
