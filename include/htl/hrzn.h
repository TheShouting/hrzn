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
#include <stdexcept>
#include <iterator>
 
#define _TO_STRING_DEFERRED(n) #n
#define _TO_STRING(n) _TO_STRING_DEFERRED(n)
#define THROW_NOT_IMPLEMENTED(m) throw std::runtime_error(m " not implemented at line: " _TO_STRING(__LINE__) ", " _TO_STRING(__FILE__))

#define _GENERATE_MATH_AND(F) \
	template <typename T> inline bool F(const ITuple<T>& a) { \
		return std::F(a.x) && std::F(a.y); }

#define _GENERATE_MATH_B(F) \
	template <typename T> inline ITuple<bool> F##_b(const ITuple<T>& a) { \
		return {std::F(a.x), std::F(a.y)}; }

#define _GENERATE_MATHF(F) \
	template <typename T> inline ITuple<T> F(const ITuple<T>& a) { \
		return { std::F(a.x), std::F(a.y) }; }


//#define H_DOUBLE_TYPE
//#define H_LONG_TYPE

// Set floating precision Size
#ifdef H_DOUBLE_TYPE
	typedef double hType_f;
#else
	typedef float hType_f;
#endif // H_DOUBLE_TYPE

// Set integral size
#ifdef H_LONG_TYPE
	typedef long hType_i;
#else
	typedef int hType_i;
#endif // H_LONG_TYPE

// Aliased UNSIGNED INT type
typedef unsigned long int hType_u;

constexpr hType_f operator "" _hf(long double val) { return static_cast<::hType_f>(val); }
constexpr hType_i operator "" _hi(unsigned long long int val) { return static_cast<::hType_i>(val); }

namespace hrzn {

	constexpr auto PI = 3.1415926535897932384626433832795_hf;
	constexpr auto DEGTORAD = 0.017453_hf;
	constexpr auto RADTODEG = 57.29578_hf;
	constexpr auto DEG = 360.0_hf;
	constexpr auto RAD = 6.283185307179586476925286766559_hf;
	constexpr auto EPSILON = 1.192092896e-07_hf;

	constexpr auto TOPLEFT = 0;
	constexpr auto TOPRIGHT = 1;
	constexpr auto LOWERRIGHT = 2;
	constexpr auto LOWERLEFT = 3;

	constexpr auto DIR_N = 0;
	constexpr auto DIR_NE = 1;
	constexpr auto DIR_E = 2;
	constexpr auto DIR_SE = 3;
	constexpr auto DIR_S = 4;
	constexpr auto DIR_SW = 5;
	constexpr auto DIR_W = 6;
	constexpr auto DIR_NW = 7;

	template<typename T, bool = std::is_floating_point_v<T>>
	struct vEpsilon {};

	template<typename T>
	struct vEpsilon<T, false> {
		inline static T value = 1_hi;
	};

	template<typename T>
	struct vEpsilon<T, true> {
		inline static T value = EPSILON;
	};


	/******************************************************************************************************************
		Transform data types (Vectors/Tuples and Rotations)
	******************************************************************************************************************/

	/// <summary>
	/// A templated vector class for managing any 2D numeric data.
	/// </summary>
	/// <typeparam name="T">Template of a numeric type.</typeparam>
	template <typename T>
	struct ITuple {

		T x, y;

		constexpr ITuple() : x(), y() {}

		template <typename TCast>
		constexpr ITuple(TCast xval, TCast yval) : x(static_cast<T>(xval)), y(static_cast<T>(yval)) {}

		explicit constexpr ITuple(T val) : x(val), y(val) {}

		constexpr ITuple(const ITuple<T>& t) = default;

		template <typename TCast>
		explicit constexpr ITuple(const ITuple<TCast>& t) : x(static_cast<T>(t.x)), y(static_cast<T>(t.y)) {}

		T operator [] (int i) const { return (&x)[i]; }

		T& operator [] (int i) { return (&x)[i]; }

		operator bool() const { return x != 0 || y != 0; }

		template <typename TCast>
		void operator =(const ITuple<TCast>& t) {
			x = static_cast<T>(t.x);
			y = static_cast<T>(t.y);
		}

		ITuple<T> signumAxis() const { // TODO test signum axis
			return { (T(0) < this->x) - (this->x < T(0)), (T(0) < this->y) - (this->y < T(0)) };
		}

		void set(T xval, T yval) { this->x = xval; this->y = yval; }

		void set(ITuple<T> other) { this->x = other.x; this->y = other.y; }

		void shift(T xshift, T yshift) { this->x += xshift; this->y += yshift; }

		void shift(ITuple<T> const& amt) { this->shift(amt.x, amt.y); }

		void scale(T mag) { this->x *= mag; this->y *= mag; }

		double length() const { return std::sqrt(this->x * this->x + this->y * this->y); }

		ITuple<T> swizzle() const { return { this->y, this->x }; }

		T lengthSqr() const { return this->x * this->x + this->y * this->y; }

		T lengthManhattan() const { return std::abs(this->x) + std::abs(this->y); }

		/// <summary>
		/// Find the unit vector of the tuple.
		/// </summary>
		/// <returns>A normailized tuple or vector.</returns>
		ITuple<T> normal() const {
			double l = length();
			if (l < EPSILON)
				return { 0._hf, 0._hf };
			double il = 1.0 / l;
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
		ITuple<T> epsilon() const {
			return { vEpsilon<T>::value, vEpsilon<T>::value };
		}

		ITuple<T> epsilonSigned() const {
			return { vEpsilon<T>::value * (T)std::signbit((double)this->x) * -2 + 1, vEpsilon<T>::value * (T)std::signbit((double)this->y) * -2 + 1 };
		}

	}; // struct ITuple<T>


	// Operator overloads for ITuple objects

	template <typename T>
	ITuple<T> operator - (const ITuple<T>& a) { return ITuple<T>(-a.x, -a.y); }

	template <typename T, typename TCast>
	bool operator ==(const ITuple<T>& a, const ITuple<TCast>& b) { return a.x == static_cast<T>(b.x) && a.y == static_cast<T>(b.y); }

	template <typename T, typename TCast>
	bool operator !=(const ITuple<T>& a, const ITuple<TCast>& b) { return !(a == b); }

	template <typename T, typename TCast>
	ITuple<T>& operator += (ITuple<T>& a, const ITuple<TCast>& b) {
		a.x += static_cast<T>(b.x); a.y += static_cast<T>(b.y);
		return a;
	}

	template <typename T, typename TCast>
	ITuple<T>& operator -= (ITuple<T>& a, const ITuple<TCast>& b) {
		a.x -= static_cast<T>(b.x); a.y -= static_cast<T>(b.y);
		return a;
	}

	template <typename T, typename TCast>
	ITuple<T> operator + (const ITuple<T>& a, const ITuple<TCast>& b) {
		return ITuple<T>(a.x + static_cast<T>(b.x), a.y + static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	ITuple<T> operator - (const ITuple<T>& a, const ITuple<TCast>& b) {
		return ITuple<T>(a.x - static_cast<T>(b.x), a.y - static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	ITuple<T> operator * (const ITuple<T>& a, const ITuple<TCast>& b) {
		return ITuple<T>(a.x * static_cast<T>(b.x), a.y * static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	ITuple<T> operator / (const ITuple<T>& a, const ITuple<TCast>& b) {
		return ITuple<T>(a.x / static_cast<T>(b.x), a.y / static_cast<T>(b.y));
	}

	template <typename T, typename TCast>
	ITuple<T> operator * (const ITuple<T>& a, TCast val) {
		static_assert(std::is_arithmetic_v<TCast>, "An arithmetic type is required for Tuple operations");
		return ITuple<T>(a.x * static_cast<T>(val), a.y * static_cast<T>(val));
	}

	template <typename T, typename TCast>
	ITuple<T> operator / (const ITuple<T>& a, TCast val) {
		static_assert(std::is_arithmetic_v<TCast>, "An arithmetic type is required for Tuple operations");
		return ITuple<T>(a.x / static_cast<T>(val), a.y / static_cast<T>(val));
	}

	template <typename T>
	ITuple<T> operator ~ (const ITuple<T>& a) { return ITuple<T>(a.y, a.x); }

	_GENERATE_MATHF(sqrt);
	_GENERATE_MATHF(abs);
	_GENERATE_MATHF(round);
	_GENERATE_MATHF(floor);
	_GENERATE_MATHF(ceil);
	_GENERATE_MATHF(trunc);

	_GENERATE_MATH_AND(isnormal);
	_GENERATE_MATH_AND(isfinite);

	_GENERATE_MATH_B(isnormal);
	_GENERATE_MATH_B(isfinite);
	_GENERATE_MATH_B(isnan);
	_GENERATE_MATH_B(isinf);
	_GENERATE_MATH_B(signbit);

	/// Type aliases for Tuples of FLOAT types.
	using hVector = ITuple<hType_f>;
	/// Type alias for Tuples of INT types.
	using hPoint = ITuple<hType_i>;

	/// <summary>
	/// An array of unit vectors containinf the vertices of a quadrilateral.
	/// </summary>
	constexpr ITuple<hType_f> h_quad[] = {
		{0._hf, 0._hf},
		{1._hf, 0._hf},
		{1._hf, 1._hf},
		{0._hf, 1._hf}
	};

	/// <summary>
	/// An array contained the point offsets for each corner in a box or area.
	/// </summary>
	constexpr ITuple<hType_i> h_corner[4] = {
		{0_hi, 0_hi},
		{1_hi, 0_hi},
		{1_hi, 1_hi},
		{0_hi, 1_hi}
	};

	/// <summary>
	/// An array containing all offsets for a 4-way neighborhood.
	/// </summary>
	constexpr ITuple<hType_i> h_neighborhood4[] = {
		{ 0_hi, -1_hi},
		{ 1_hi,  0_hi},
		{ 0_hi,  1_hi},
		{-1_hi,  0_hi},
		{ 0_hi,  0_hi}
	};

	/// <summary>
	/// An array containing all offsets for a 4-way neighborhood.
	/// </summary>
	constexpr ITuple<hType_i> h_neighborhood8[] = {
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
	/// A class for managing rotations and angles.
	/// </summary>
	struct hRotation {

		hType_f tau;

		constexpr hRotation() : tau(0.f) {}

		constexpr hRotation(hType_f a) : tau(a) {}

		hRotation operator-() const { return hRotation(-tau); }

		hRotation operator + (hRotation const& other) const { return hRotation(tau + other.tau); }
		hRotation operator - (hRotation const& other) const { return hRotation(tau - other.tau); }
		hRotation operator * (hRotation const& other) const { return hRotation(tau * other.tau); }
		hRotation operator / (hRotation const& other) const { return hRotation(tau / other.tau); }

		template <typename T>
		hRotation operator + (T const& val) const { return hRotation(tau + static_cast<hType_f>(val)); }

		template <typename T>
		hRotation operator - (T const& val) const { return hRotation(tau - static_cast<hType_f>(val)); }

		template <typename T>
		hRotation operator * (T const& val) const { return hRotation(tau * static_cast<hType_f>(val)); }

		template <typename T>
		hRotation operator / (T const& val) const { return hRotation(tau / static_cast<hType_f>(val)); }

		auto addDeg(hType_f deg) {
			tau += (deg / DEG);
			return tau * DEG;
		}

		auto addRad(hType_f rad) {
			tau += (rad / RAD);
			return tau * RAD;
		}

		void setDeg(hType_f deg) {
			tau = deg / DEG;
		}

		void setRad(hType_f rad) {
			tau = rad / RAD;
		}

		inline auto deg() const { return tau * DEG; }

		inline auto rad() const { return tau * RAD; }

		inline void spin(hType_f t) {
			tau += t;
		}

		hRotation angle() const {
			return std::fmod(tau - std::trunc(tau) + 1._hf, 1._hf);
		}

		hRotation revolutions() const {
			return std::trunc(tau);
		}

		hRotation flip() const {
			return hRotation(tau + 0.5_hf).angle();
		}

		hRotation inverse() const {
			return hRotation(1.0_hf - angle().tau);
		}

		void setWithVector(hVector vec) {
			tau = std::atan2(vec.y, vec.x) / RAD;
			tau = tau - std::floor(tau);
		}

		hVector getForwardVector(hType_f length = 1._hf) const {
			return hVector(std::cos(rad()) * length, std::sin(rad()) * length);
		}

		hVector getRightVector(hType_f length = 1._hf) const {
			return hVector(std::cos(rad() + PI * 0.5_hf) * length, std::sin(rad() + PI * 0.5_hf) * length);
		}

		hVector rotate(hVector const& vec) const {
			hType_f a = angle().rad();
			hType_f x = std::cos(a) * vec.x - std::sin(a) * vec.y;
			hType_f y = std::sin(a) * vec.x + std::cos(a) * vec.y;
			return hVector(x, y);
		}

		hVector unrotate(hVector const& vec) const {
			hType_f a = angle().rad();
			hType_f x = std::cos(-a) * vec.x - std::sin(-a) * vec.y;
			hType_f y = std::sin(-a) * vec.x + std::cos(-a) * vec.y;
			return hVector(x, y);
		}

		static hRotation difference(hRotation a, hRotation b) {
			hType_f diff = a.angle().tau - b.angle().tau + 0.5_hf;
			return hRotation(diff - std::floor(diff) - 0.5_hf);
		}

		static hRotation Degrees(hType_f deg) { return hRotation(deg / DEG); }

		static hRotation Radians(hType_f rad) { return hRotation(rad / RAD); }

	}; // struct hRotation


	/******************************************************************************************************************
		hArea data types
	******************************************************************************************************************/

	/// <summary>
	/// A class for managing coordinates of a 2D area.
	/// </summary>
	struct hArea {

		hType_i x1, y1, x2, y2;

		constexpr hArea() : x1(INT_MAX), y1(INT_MAX), x2(INT_MIN), y2(INT_MIN) {}

		constexpr hArea(hType_u w, hType_u h) : x1(0), y1(0), x2((hType_i)w), y2((hType_i)h) {}

		constexpr hArea(hType_i x_1, hType_i y_1, hType_i x_2, hType_i y_2) : x1(x_1), y1(y_1), x2(x_2), y2(y_2) {}

		explicit constexpr hArea(hPoint p1, hPoint p2) : x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y) {}

		hArea& operator +=(hPoint dist) {
			move(dist.x, dist.y);
			return *this;
		}

		hArea& operator -=(hPoint dist) {
			move(-dist.x, -dist.y);
			return *this;
		}

		operator bool() const {
			return x1 < x2 && y1 < y2;
		}

		bool valid() const {
			return x1 < x2 && y1 < y2;
		}

		void move(hType_i move_x, hType_i move_y) {
			x1 += move_x;
			y1 += move_y;
			x2 += move_x;
			y2 += move_y;
		}

		virtual void resize(hType_i x_1, hType_i y_1, hType_i x_2, hType_i y_2) {
			x1 = std::min(x_1, x_2);
			x2 = std::max(x_1, x_2);
			y1 = std::min(y_1, y_2);
			y2 = std::max(y_1, y_2);
		}

		void resize(const hPoint& size) {
			if (size) {
				resize(std::min(x1, x1 + size.x), std::min(y1, y1 + size.y), std::max(x1, x1 + size.x), std::max(y1, y1 + size.y));
			}
		}

		void resizeFromCenter(hPoint size) {
			resizeFromCenter(size.x, size.y);
		}

		void resizeFromCenter(hType_u size) {
			resizeFromCenter(size, size);
		}

		void resizeFromCenter(hType_u w, hType_u h) {
			if (valid()) {
				hPoint ctr = center();
				resize(ctr.x - w / 2, ctr.y - h / 2, x1 + w, y1 + h);
			}
		}

		hArea normalized() const {
			return hArea(0, 0, x2 - x1, y2 - y1);
		}

		hPoint clamp(const hPoint& pt) const {
			return { std::min(x2, std::max(x1, pt.x)), std::min(y2, std::max(y1, pt.y)) };
		}

		hPoint wrap(const hPoint& pt) const {
			hType_i x = ((hType_i)width() + ((pt.x - x1) % (hType_i)width())) % (hType_i)width() + x1;
			hType_i y = ((hType_i)height() + ((pt.y - y1) % (hType_i)height())) % (hType_i)height() + y1;
			return { x, y };
		}

		std::size_t width() const {
			return std::max(x2 - x1, 0_hi);
		}

		std::size_t height() const {
			return std::max(y2 - y1, 0_hi);
		}

		hPoint dimensions() const {
			return { width(), height() };
		}

		hPoint first() const {
			return { x1, x1 };
		}

		hPoint last() const {
			return { x2, y2 };
		}

		std::size_t area() const {
			return width() * height();
		}

		hPoint corner(int i) const {
			switch (i) {
			case(TOPLEFT):
				return { x1, y1 };
			case(TOPRIGHT):
				return { x2 - 1, y1 };
			case(LOWERRIGHT):
				return { x2 - 1, y2 - 1 };
			case(LOWERLEFT):
				return { x1, y2 - 1 };
			default:
				throw std::out_of_range("Index is not a valid corner.");
			}
		}

		hPoint center() const {
			return { (x2 - x1) / 2 + x1, (y2 - y1) / 2 + y1 };
		}

		bool contains(const hPoint& pt) const {
			return this->contains(pt.x, pt.y);
		}

		bool contains(hType_i x, hType_i y) const {
			return x >= x1 && x < x2 && y >= y1 && y < y2;
		}

		bool contains(const hArea& other) {
			return false;
		}

	}; // struct hArea

	inline bool operator==(const hArea& a, const hArea& b) {
		return a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2;
	}

	inline hArea operator+(const hArea& a, const hPoint& b) {
		return hArea(a.first() + b, a.last() + b);
	}

	inline hArea intersect(const hArea& a, const hArea& b) {
		return hArea(std::max(a.x1, b.x1), std::max(a.y1, b.y1), std::min(a.x2, b.x2), std::min(a.y2, b.y2));
	}


	/******************************************************************************************************************
		Transformation types
	******************************************************************************************************************/


	/// <summary>
	/// A class holding position, rotation, and scale values for use in transformation of various other coordinates in 2D space.
	/// </summary>
	struct hTransform {
		hVector position;
		hRotation rotation;
		hVector scale;

		constexpr hTransform() : position(0._hf, 0._hf), rotation(0._hf), scale(1._hf, 1._hf) {}

		constexpr hTransform(hVector p, hRotation r, hVector s) : position(p), rotation(r), scale(s) {}

		hVector getForwardVector() const {
			return rotation.getForwardVector(scale.y);
		}

		hVector getRightVector() const {
			return rotation.getRightVector(scale.x);
		}

		hVector childPositon(hVector pos) const {
			return position + rotation.rotate(pos * scale);
		}

		hRotation childRotation(hRotation r) const {
			return rotation + r;
		}

		hVector childScale(hVector s) const {
			return scale * s;
		}

		hVector inversePosition(hVector pos) {
			return rotation.unrotate(pos / scale.epsilon()) - pos;
		}

		hTransform childTransform(const hTransform & child) const {
			return { childPositon(child.position), childRotation(child.rotation), childScale(child.scale)};
		}

	}; // struct hTransform

} // namespace hrzn

#undef THROW_NOT_IMPLEMENTED

#undef _GENERATE_MATHF
#undef _GENERATE_MATH_AND
#undef _GENERATE_MATH_OR
