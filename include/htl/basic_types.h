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

#ifndef H_NOEXCEPTIONS
#include <stdexcept>
#endif // !H_NOEXCEPTIONS

 
#define _TO_STRING_DEFERRED(n) #n
#define _TO_STRING(n) _TO_STRING_DEFERRED(n)
#define THROW_NOT_IMPLEMENTED(m) throw std::runtime_error(m " not implemented at line: " _TO_STRING(__LINE__) ", " _TO_STRING(__FILE__))

//#define H_DOUBLE_TYPE
//#define H_LONG_TYPE

// Set floating precision Size
#ifdef H_DOUBLE_TYPE
	typedef double h_float;
#else
	typedef float h_float;
#endif // H_DOUBLE_TYPE

// Set integral size
#ifdef H_LONG_TYPE
	typedef long h_int;
#else
	typedef int h_int;
#endif // H_LONG_TYPE

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
	struct tuple2 {

		T x, y;

		constexpr tuple2() : x(), y() {}

		template <typename TCast>
		constexpr tuple2(TCast xval, TCast yval) : x(static_cast<T>(xval)), y(static_cast<T>(yval)) {}

		explicit constexpr tuple2(T val) : x(val), y(val) {}

		constexpr tuple2(const tuple2<T>& t) = default;

		template <typename TCast>
		explicit constexpr tuple2(const tuple2<TCast>& t) : x(static_cast<T>(t.x)), y(static_cast<T>(t.y)) {}

		T operator [] (int i) const { 
#ifndef H_NOEXCEPTIONS
			if (i != 0 && i != 1) throw std::out_of_range("Index is not 0 or 1.");
#endif // !H_NOEXCEPTIONS
			return (&x)[i]; 
		}

		T& operator [] (int i) {
#ifndef H_NOEXCEPTIONS
			if (i != 0 && i != 1) throw std::out_of_range("Index is not 0 or 1.");
#endif // !H_NOEXCEPTIONS
				return (&x)[i];
		}

		operator bool() const { return x != 0 || y != 0; }

		template <typename TCast>
		void operator =(const tuple2<TCast>& t) {
			x = static_cast<T>(t.x);
			y = static_cast<T>(t.y);
		}

		tuple2<T> signumAxis() const { // TODO test signum axis
			return { (T(0) < this->x) - (this->x < T(0)), (T(0) < this->y) - (this->y < T(0)) };
		}

		void set(T xval, T yval) { this->x = xval; this->y = yval; }

		void set(tuple2<T> other) { this->x = other.x; this->y = other.y; }

		void shift(T xshift, T yshift) { this->x += xshift; this->y += yshift; }

		void shift(tuple2<T> const& amt) { this->shift(amt.x, amt.y); }

		void scale(T mag) { this->x *= mag; this->y *= mag; }

		tuple2<T> swizzle() const { return { this->y, this->x }; }

		auto length() const { return std::sqrt(this->x * this->x + this->y * this->y); }

		T lengthSqr() const { return this->x * this->x + this->y * this->y; }

		T lengthManhattan() const { return std::abs(this->x) + std::abs(this->y); }

		/// <summary>
		/// Find the unit vector of the tuple.
		/// </summary>
		/// <returns>A normailized tuple or vector.</returns>
		tuple2<T> normal() const {
			h_float l = length();
			if (l < EPSILON)
				return { 0._hf, 0._hf };
			h_float il = 1._hf / l;
			return { static_cast<T>(this->x * il), static_cast<T>(this->y * il) };
		}

		/// <summary>
		/// Normailize the Tuple in place and return the former length.
		/// </summary>
		/// <returns>Original length before normalizing.</returns>
		double normalize() {
			double l = length();
			if (l < EPSILON)
				return 0.0;
			double il = 1.0 / l;
			this->x = static_cast<T>(this->x * il);
			this->y = static_cast<T>(this->y * il);
			return l;
		}

		/// <summary>
		/// Create a new tuple contining the smallest values of x and y where the they are non-zero.
		/// </summary>
		/// <returns></returns>
		tuple2<T> epsilonSigned() const {
			return { _lib::vEpsilon<T>::value * (T)std::signbit((double)this->x) * -2 + 1, _lib::vEpsilon<T>::value * (T)std::signbit((double)this->y) * -2 + 1 };
		}

		///// <summary>
		///// Create a new tuple contining the smallest values of x and y where the they are non-zero.
		///// </summary>
		static constexpr tuple2<T> EPSILON2() { return { _lib::vEpsilon<T>::value, _lib::vEpsilon<T>::value }; }

		static constexpr tuple2<T> UP() { return { 0, 1 }; }

		static constexpr tuple2<T> DOWN() { return { 0, -1 }; }

		static constexpr tuple2<T> RIGHT() { return { 1, 0 }; }

		static constexpr tuple2<T> LEFT() { return { -1, 0 }; }

	}; // struct tuple2<T>


	// Operator overloads for tuple2 objects

	template <typename T>
	tuple2<T> operator - (const tuple2<T>& a) { return tuple2<T>(-a.x, -a.y); }

	template <typename T, typename TCast>
	bool operator ==(const tuple2<T>& a, const tuple2<TCast>& b) { return a.x == static_cast<T>(b.x) && a.y == static_cast<T>(b.y); }

	template <typename T, typename TCast>
	bool operator !=(const tuple2<T>& a, const tuple2<TCast>& b) { return !(a == b); }

	template <typename T, typename TCast>
	tuple2<T>& operator += (tuple2<T>& a, const tuple2<TCast>& b) {
		a.x += static_cast<T>(b.x); a.y += static_cast<T>(b.y);
		return a;
	}

	template <typename T, typename TCast>
	tuple2<T>& operator -= (tuple2<T>& a, const tuple2<TCast>& b) {
		a.x -= static_cast<T>(b.x); a.y -= static_cast<T>(b.y);
		return a;
	}

	template <typename T, typename TCast>
	tuple2<T> operator + (const tuple2<T>& a, const tuple2<TCast>& b) {
		return tuple2<T>(a.x + static_cast<T>(b.x), a.y + static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	tuple2<T> operator - (const tuple2<T>& a, const tuple2<TCast>& b) {
		return tuple2<T>(a.x - static_cast<T>(b.x), a.y - static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	tuple2<T> operator * (const tuple2<T>& a, const tuple2<TCast>& b) {
		return tuple2<T>(a.x * static_cast<T>(b.x), a.y * static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	tuple2<T> operator / (const tuple2<T>& a, const tuple2<TCast>& b) {
		return tuple2<T>(a.x / static_cast<T>(b.x), a.y / static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	tuple2<T> operator * (const tuple2<T>& a, TCast val) {
		static_assert(std::is_arithmetic_v<TCast>, "An arithmetic type is required for Tuple operations");
		return tuple2<T>(a.x * static_cast<T>(val), a.y * static_cast<T>(val));
	}

	template <typename T, typename TCast>
	tuple2<T> operator / (const tuple2<T>& a, TCast val) {
		static_assert(std::is_arithmetic_v<TCast>, "An arithmetic type is required for Tuple operations");
		return tuple2<T>(a.x / static_cast<T>(val), a.y / static_cast<T>(val));
	}

	template <typename T>
	tuple2<T> operator ~ (const tuple2<T>& a) { return tuple2<T>(a.y, a.x); }


#define MACRO_GENERATE_MATH_AND(F) \
	template <typename T> inline bool F(const tuple2<T>& a) { \
		return std::F(a.x) && std::F(a.y); }

#define MACRO_GENERATE_MATH_B(F) \
	template <typename T> inline tuple2<bool> F##_b(const tuple2<T>& a) { \
		return {std::F(a.x), std::F(a.y)}; }

#define MACRO_GENERATE_MATH_F(F) \
	template <typename T> inline tuple2<T> F(const tuple2<T>& a) { \
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
	using vector2 = tuple2<h_float>;
	/// Type alias for Tuples of INT types.
	using point2 = tuple2<h_int>;

	/// <summary>
	/// An array of unit vectors containinf the vertices of a quadrilateral.
	/// </summary>
	static constexpr tuple2<h_float> h_quad[] = {
		{0._hf, 0._hf},
		{1._hf, 0._hf},
		{1._hf, 1._hf},
		{0._hf, 1._hf}
	};

	/// <summary>
	/// An array contained the point offsets for each corner in a box or area.
	/// </summary>
	static constexpr tuple2<h_int> h_corner[4] = {
		{0_hi, 0_hi},
		{1_hi, 0_hi},
		{1_hi, 1_hi},
		{0_hi, 1_hi}
	};

	/// <summary>
	/// An array containing all offsets for a 4-way neighborhood.
	/// </summary>
	static constexpr tuple2<h_int> h_neighborhood4[] = {
		{ 0_hi, -1_hi},
		{ 1_hi,  0_hi},
		{ 0_hi,  1_hi},
		{-1_hi,  0_hi},
		{ 0_hi,  0_hi}
	};

	/// <summary>
	/// An array containing all offsets for a 4-way neighborhood.
	/// </summary>
	static constexpr tuple2<h_int> h_neighborhood8[] = {
		{ 0_hi, -1_hi},
		{ 1_hi, -1_hi},
		{ 1_hi,  0_hi},
		{ 1_hi,  1_hi},
		{ 0_hi,  1_hi},
		{-1_hi,  1_hi},
		{-1_hi,  0_hi},
		{-1_hi, -1_hi},
		{ 0_hi,  0_hi}
	};

	/// <summary>
	/// Normalized angles.
	/// </summary>
	struct angle {

		h_float tau;

		constexpr angle() : tau(0.f) {}

		constexpr angle(h_float a) : tau(_lib::normalizef(a)) {}

		constexpr angle(const angle & a) = default;

		inline auto deg() const { return tau * DEG; }

		inline auto rad() const { return tau * RAD; }

		auto rotateByDeg(h_float deg) {
			tau = _lib::normalizef(deg / DEG + tau);
			return tau * DEG;
		}

		auto rotateByRad(h_float rad) {
			tau = _lib::normalizef(rad / RAD + tau);
			return tau * RAD;
		}

		void set(h_float t) {
			tau = _lib::normalizef(t);
		}

		void setByDeg(h_float deg) {
			tau = _lib::normalizef(deg / DEG);
		}

		void setByRad(h_float rad) {
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

		vector2 getForwardVector(h_float length = 1._hf) const {
			return vector2(std::sin(rad()) * length, std::cos(rad()) * length);
		}

		vector2 getRightVector(h_float length = 1._hf) const {
			return vector2(std::sin(rad() + PI * 0.5_hf) * length, std::cos(rad() + PI * 0.5_hf) * length);
		}

		vector2 rotate(vector2 const& vec) const {
			h_float a = rad();
			h_float cs = std::cos(a);
			h_float sn = std::sin(a);
			return { cs * vec.x + sn * vec.y, sn * -vec.x + cs * vec.y };
		}

		vector2 unrotate(vector2 const& vec) const {
			h_float a = -rad();
			h_float cs = std::cos(a);
			h_float sn = std::sin(a);
			return { cs * vec.x + sn * vec.y, sn * -vec.x + cs * vec.y };
		}

		static constexpr angle fromDegrees(h_float deg) { return angle(deg / DEG); }

		static constexpr angle fromRadians(h_float rad) { return angle(rad / RAD); }

		template <typename T>
		static constexpr angle fromVector(tuple2<T> vec) {
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

	//angle& operator += (angle& a, const angle& b) {
	//	a.tau = _lib::normalizef(a.tau + b.tau);
	//	return a;
	//}

	//angle& operator -= (angle& a, const angle& b) {
	//	a.tau = _lib::normalizef(a.tau - b.tau);
	//	return a;
	//}


	/******************************************************************************************************************
		point_area data types
	******************************************************************************************************************/

	/// <summary>
	/// A class for managing coordinates of a 2D area.
	/// </summary>
	struct point_area {

		h_int x1, y1, x2, y2;

		constexpr point_area() : x1(INT_MAX), y1(INT_MAX), x2(INT_MIN), y2(INT_MIN) {}

		constexpr point_area(h_unsigned w, h_unsigned h) : x1(0), y1(0), x2((h_int)w), y2((h_int)h) {}

		constexpr point_area(h_int x_1, h_int y_1, h_int x_2, h_int y_2) : x1(x_1), y1(y_1), x2(x_2), y2(y_2) {}

		explicit constexpr point_area(point2 p1, point2 p2) : x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y) {}

		point_area& operator +=(point2 dist) {
			move(dist.x, dist.y);
			return *this;
		}

		point_area& operator -=(point2 dist) {
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

		void resize(const point2& size) {
			if (size) {
				resize(std::min(x1, x1 + size.x), std::min(y1, y1 + size.y), std::max(x1, x1 + size.x), std::max(y1, y1 + size.y));
			}
		}

		void resizeFromCenter(point2 size) {
			resizeFromCenter(size.x, size.y);
		}

		void resizeFromCenter(h_unsigned size) {
			resizeFromCenter(size, size);
		}

		void resizeFromCenter(h_unsigned w, h_unsigned h) {
			if (valid()) {
				point2 ctr = center();
				resize(ctr.x - w / 2, ctr.y - h / 2, x1 + w, y1 + h);
			}
		}

		point_area normalized() const {
			return point_area(0, 0, x2 - x1, y2 - y1);
		}

		point2 clamp(const point2& pt) const {
			return { std::min(x2, std::max(x1, pt.x)), std::min(y2, std::max(y1, pt.y)) };
		}

		point2 wrap(const point2& pt) const {
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
#ifndef H_NOEXCEPTIONS
			if (i < 0 || i > 3) throw std::out_of_range("Index is not a valid corner.");
#endif // !H_NOEXCEPTIONS
			return { x1 + h_corner[i].x * (width() - 1), y1 + h_corner[i].y * (height() - 1) };
		}

		point2 center() const {
			return { (x2 - x1) / 2 + x1, (y2 - y1) / 2 + y1 };
		}

		bool contains(const point2& pt) const {
			return this->contains(pt.x, pt.y);
		}

		bool contains(h_int x, h_int y) const {
			return x >= x1 && x < x2 && y >= y1 && y < y2;
		}

		bool contains(const point_area& other) {
			return false;
		}

	}; // struct point_area

	inline bool operator==(const point_area& a, const point_area& b) {
		return a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2;
	}

	inline point_area operator+(const point_area& a, const point2& b) {
		return point_area(a.first() + b, a.last() + b);
	}

	inline point_area operator-(const point_area& a, const point2& b) {
		return point_area(a.first() - b, a.last() - b);
	}

	/// <summary>
	/// Create a new Area based on the overlap of two other Area objects.
	/// </summary>
	inline point_area intersect(point_area a, point_area b) {
		return point_area(std::max(a.x1, b.x1), std::max(a.y1, b.y1), std::min(a.x2, b.x2), std::min(a.y2, b.y2));
	}

	/// <summary>
	/// Return true if both <type>point_area</type> objects overlap.
	/// </summary>
	inline bool overlap(point_area a, point_area b) {
		return !(a.x1 > b.x2 || a.y1 > b.y2 || b.x1 > a.x2 || b.y1 > a.y2);
	}

	/// <summary>
	/// Is true if <type>point_area</type> b is completely contained within <type>point_area</type> a.
	/// </summary>
	inline bool contains(point_area a, point_area b) {
		return b.x1 >= a.x1 && b.y1 >= a.y1 && b.x2 <= a.x2 && b.y2 <= a.x2;
	}

	/// <summary>
	/// Is true if <type>poin2</type> b is completely contained within <type>point_area</type> a.
	/// </summary>
	inline bool contains(point_area a, point2 b) {
		return b.x >= a.x1 && b.y >= a.y1 && b.x < a.x2 && b.y < a.x2;
	}

	/// <summary>
	/// Is true if <type>point2</type> is equal to any edge position of <type>point_area</type>.
	/// </summary>
	inline bool is_edge(point_area area, point2 pos) {
		return pos.x == area.x1 || pos.x == area.x2 - 1_hi || pos.y == area.y1 || pos.y == area.y2 - 1_hi;
	}

	/// <summary>
	/// Swap x and y values.
	/// </summary>
	template<typename T>
	inline tuple2<T> swizzle(const tuple2<T>& t) {
		return { t.y, t.x };
	}

	/// <summary>
	/// Swap width and height values (same as rotating 90 deg)
	/// </summary>
	inline point_area swizzle(const point_area& a) {
		return point_area(a.y1, a.x1, a.y2, a.x2);
	}

	/// <summary>
	/// Snaps a point to an area edge if it is out of bounds.
	/// </summary>
	inline point2 clamp_point(const point2& p, const point_area& area) {
		h_int x = std::min(std::max(p.x, area.x1), area.x2 - 1_hi);
		h_int y = std::min(std::max(p.y, area.y1), area.y2 - 1_hi);
		return { x, y };
	}

	/// <summary>
	/// Wraps a point to the oppposite edge of an area if it is out of bounds.
	/// </summary>
	inline point2 wrap_point(const point2& p, const point_area& area) {
		h_int x = (p.x % area.width() + area.width()) % area.width();
		h_int y = (p.y % area.height() + area.height()) % area.height();
		return { x, y };
	}

	/******************************************************************************************************************
		Transformation types
	******************************************************************************************************************/


	/// <summary>
	/// A class holding position, angle, and scale values for use in transformation of various other coordinates in 2D space.
	/// </summary>
	struct transform {
		hrzn::vector2 position;
		hrzn::angle rotation;
		hrzn::vector2 scale;

		constexpr transform() : position(0._hf, 0._hf), rotation(0._hf), scale(1._hf, 1._hf) {}

		constexpr transform(hrzn::vector2 p, hrzn::angle r, hrzn::vector2 s) : position(p), rotation(r), scale(s) {}

		vector2 getForwardVector() const {
			return rotation.getForwardVector(scale.y);
		}

		vector2 getRightVector() const {
			return rotation.getRightVector(scale.x);
		}

		vector2 childPositon(vector2 pos) const {
			return position + rotation.rotate(pos * scale);
		}

		hrzn::angle childRotation(hrzn::angle r) const {
			return rotation + r;
		}

		vector2 childScale(vector2 s) const {
			return scale * s;
		}

		//vector2 inversePosition(vector2 pos) {
		//	//return rotation.unrotate(pos / scale.epsilon()) - pos;
		//	return rotation.unrotate(pos / vector2::EPSILON2()) - pos;
		//}

		transform childTransform(const transform & child) const {
			return { childPositon(child.position), childRotation(child.rotation), childScale(child.scale)};
		}

	}; // struct transform

} // namespace hrzn

#undef THROW_NOT_IMPLEMENTED
