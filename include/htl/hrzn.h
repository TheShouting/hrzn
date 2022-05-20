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
#include <vector>

#define H_PI 3.1415926535897932384626433832795_hf
#define H_DEGTORAD 0.017453_hf
#define H_RADTODEG 57.29578_hf
#define H_DEG 360.0_hf
#define H_RAD 6.283185307179586476925286766559_hf
#define H_EPSILON 1.192092896e-07_hf

#define H_CORNER_TOPLEFT 0
#define H_CORNER_TOPRIGHT 1
#define H_CORNER_LOWERRIGHT 2
#define H_CORNER_LOWERLEFT 3


#define _TO_STRING_DEFERRED(n) #n
#define _TO_STRING(n) _TO_STRING_DEFERRED(n)
#define THROW_NOT_IMPLEMENTED(m) throw std::runtime_error(m " not implemented at line: " _TO_STRING(__LINE__) ", " _TO_STRING(__FILE__))

#define _GENERATE_MATH_AND(F) \
	template <typename T> inline bool F(const ITuple<T>& a) { \
		return std::F(a.x) && std::F(a.y); }

#define _GENERATE_MATH_OR(F) \
	template <typename T> inline bool F(const ITuple<T>& a) { \
		return std::F(a.x) || std::F(a.y); }

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

#define HRZN_FOREACH_POINT(A, X, Y) for (hType_i X, Y = A.y1; Y < A.y2; ++Y) for (X = A.x1; X < A.x2; ++X) 

namespace hrzn {

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
			if (l < H_EPSILON)
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
			if (l < H_EPSILON)
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
			double small_v = 1.0;
			if (std::is_floating_point<T>::value)
				small_v = H_EPSILON;
			double vx = static_cast<double>(x);
			double x_sign = std::signbit(vx) * -2.0 + 1.0;
			double vy = static_cast<double>(y);
			double y_sign = std::signbit(vy) * -2.0 + 1.0;
			return { std::max(small_v, std::abs(vx)) * x_sign, std::max(small_v, std::abs(vy)) * y_sign};
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

	_GENERATE_MATH_OR(isnan);
	_GENERATE_MATH_OR(isinf);
	_GENERATE_MATH_OR(signbit);


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
			tau += (deg / H_DEG);
			return tau * H_DEG;
		}

		auto addRad(hType_f rad) {
			tau += (rad / H_RAD);
			return tau * H_RAD;
		}

		void setDeg(hType_f deg) {
			tau = deg / H_DEG;
		}

		void setRad(hType_f rad) {
			tau = rad / H_RAD;
		}

		inline auto deg() const { return tau * H_DEG; }

		inline auto rad() const { return tau * H_RAD; }

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
			tau = std::atan2(vec.y, vec.x) / H_RAD;
			tau = tau - std::floor(tau);
		}

		hVector getForwardVector(hType_f length = 1._hf) const {
			return hVector(std::cos(rad()) * length, std::sin(rad()) * length);
		}

		hVector getRightVector(hType_f length = 1._hf) const {
			return hVector(std::cos(rad() + H_PI * 0.5_hf) * length, std::sin(rad() + H_PI * 0.5_hf) * length);
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

		static hRotation Degrees(hType_f deg) { return hRotation(deg / H_DEG); }

		static hRotation Radians(hType_f rad) { return hRotation(rad / H_RAD); }

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
			case(H_CORNER_TOPLEFT):
				return { x1, y1 };
			case(H_CORNER_TOPRIGHT):
				return { x2 - 1, y1 };
			case(H_CORNER_LOWERRIGHT):
				return { x2 - 1, y2 - 1 };
			case(H_CORNER_LOWERLEFT):
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
		Matrix container types
	******************************************************************************************************************/

	/// <summary>
	/// Abstract class for all Matrix-like containers and accessors.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class IMap : public hArea {
	public:

		using fill_func = T(*)();
		//using fill_func = std::function<T()>; // TODO add ifdef block to check for cpp compiler version

		bool repeat_boundary = false;

		IMap(const hArea& area) : hArea(area) {}

		virtual ~IMap() {}

		// Common inherited methods
		T& operator[](hPoint pt) { return at(pt.x, pt.y); }
		T operator[](hPoint pt) const { return at(pt.x, pt.y); }

		T& at(hPoint p) { return at(p.x, p.y); }
		T at(hPoint p) const { return at(p.x, p.y); }
		void set(hPoint p, const T& val) { set(p.x, p.y, val); }

		virtual void fill(const T& obj) {
			for (hType_i y = y1; y < y2; ++y)
				for (hType_i x = x1; x < x2; ++x)
					set(x, y, obj);
		}

		void fill(fill_func f) {
			for (hType_i y = y1; y < y2; ++y)
				for (hType_i x = x1; x < x2; ++x)
					set(x, y, f());
		}

		// Abstract methods
		virtual operator bool() const = 0;
		virtual T& at(hType_i x, hType_i y) = 0;
		virtual T at(hType_i x, hType_i y) const = 0;
		virtual void set(hType_i x, hType_i y, const T& val) = 0;

	protected:
		std::size_t f_index(hType_i x, hType_i y) const {
			if (contains(x, y))
				return (x - x1) + (y - y1) * width();
			throw std::out_of_range("Point not located in Matrix.");
		}

	public:
		struct Iterator : hPoint {
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;

			IMap& map;

			Iterator(IMap& m, hPoint p) : map(m), hPoint(p) {}

			T& operator *() { return map.at(x, y); }
			T* operator ->() { return &map.at(x, y); }

			// Prefix increment
			Iterator& operator++() {
				y = y + (x == (map.x2 - 1_hi));
				x = (x - map.x1 + 1) % map.width() + map.x1;
				return *this;
			}

			// Postfix increment
			Iterator operator++(int) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

		}; // struct IMap>T>::Iterator

		Iterator begin() { return Iterator(*this, { x1, x1 }); }
		Iterator end() { return Iterator(*this, { x1, y2 }); }

	}; // class IMap<T>


	/// <summary>
	/// A helper class used for operating on the sub region of an abstract Matrix parent class.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class HMapRef : public IMap<T> {
	private:
		IMap<T>* m_source;
	public:

		using IMap<T>::operator[];
		using IMap<T>::at;
		using IMap<T>::set;
		using base = IMap<T>;

		HMapRef(const hArea& area, base& mat) : base(area), m_source(&mat) {}

		operator bool() const override { return m_source->operator bool(); }

		T& at(hType_i x, hType_i y) override { return m_source->at(x, y); }

		T at(hType_i x, hType_i y) const override { return m_source->at(x, y); }

		void set(hType_i x, hType_i y, const T& val) override { m_source->set(x, y, val); }

		base* source() { return m_source; }

		void resize(hType_i xa, hType_i ya, hType_i xb, hType_i yb) override {
			hArea new_rect = intersect(*this, { xa, ya, xb, yb });
			hArea::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
			//hArea::resize(xa, ya, xb, yb);
		}

	}; // class HMapRef<T>

	template<typename T>
	HMapRef<T> ReferenceArea(hArea area, IMap<T> & map) {
		hArea i_area = hrzn::intersect(area, map);
		return HMapRef<T>(i_area, map);
	}


	/// <summary>
	/// Map container for storing any variables by position in a 2 dimensional grid.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class HMap : public IMap<T> {
	public:

		using IMap<T>::operator[];
		using IMap<T>::at;
		using IMap<T>::set;
		using base = IMap<T>;

	private:

		T* m_contents = nullptr;

	public:

		HMap() : base(hArea()) {}
		HMap(std::size_t w, std::size_t h) : base(hArea(w, h)), m_contents(new T[w * h]) {}
		HMap(std::size_t w, std::size_t h, const T& obj) : base(hArea(w, h)), m_contents(new T[w * h]) { for (int i = 0; i < (w * h); ++i) m_contents[i] = obj; }
		HMap(const hArea& rect) : base(rect), m_contents(new T[rect.area()]) {}
		HMap(const hArea& rect, const T& obj) : base(rect), m_contents(new T[rect.area()]) { for (int i = 0; i < rect.area(); ++i) m_contents[i] = obj; }

		HMap(const HMap<T>& other) : base(other), m_contents(new T[other.area()]) {
			std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
		}

		~HMap() {
			delete[] m_contents;
			m_contents = nullptr;
		}

		HMap<T>& operator =(const HMap<T>& other) {
			if (this != &other) {
				delete[] m_contents;
				m_contents = new T[other.area()];
				std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
				hArea::resize(other.x1, other.y1, other.x2, other.y2);
			}
			return *this;
		}

		operator bool() const override { return m_contents; }

		T& operator[](std::size_t i) { return m_contents[i]; }

		const T& operator[](std::size_t i) const { return m_contents[i]; }

		T& at(hType_i x, hType_i y) override {
			return m_contents[this->f_index(x, y)];
		}

		T at(hType_i x, hType_i y) const override {
			return m_contents[this->f_index(x, y)];
		}

		void set(hType_i x, hType_i y, const T& val) override {
			m_contents[this->f_index(x, y)] = val;
		}

		void resize(hType_i xa, hType_i ya, hType_i xb, hType_i yb) override {
			resize(xa, ya, xb, yb, T());
		}

		void resize(hType_i xa, hType_i ya, hType_i xb, hType_i yb, const T& fill_obj) {
			assert(m_contents != nullptr);
			hArea new_rect(xa, ya, xb, yb);
				T* new_block = new T[new_rect.area()];
				for (hType_i y = new_rect.y1; y < new_rect.y2; ++y)
					for (hType_i x = new_rect.x1; x < new_rect.x2; ++x) {
						hType_i i = (x - new_rect.x1) + (y - new_rect.y1) * (hType_i)new_rect.width();
						new_block[i] = hArea::contains(x, y) ? std::move(m_contents[base::f_index(x, y)]) : fill_obj;
					}
				delete[]m_contents;
				m_contents = new_block;
			hArea::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
		}

	}; // class HMap<T>


	// Bitwise AND operation between two boolean Matrices
	inline HMap<bool> operator & (const IMap<bool>& a, const IMap<bool>& b) {
		HMap<bool> result(intersect(a, b));
		for (int y = result.y1; y < result.y2; ++y)
			for (int x = result.x1; x < result.x2; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	// Bitwise OR operation between two boolean Matrices
	inline HMap<bool> operator | (const IMap<bool>& a, const IMap<bool>& b) {
		HMap<bool> result(intersect(a, b));
		for (int y = result.y1; y < result.y2; ++y)
			for (int x = result.x1; x < result.x2; ++x)
				result.set(x, y, a.at(x, y) || b.at(x, y));
		return result;
	}

	// Bitwise XOR operation between two boolean Matrices
	inline HMap<bool> operator ^ (const IMap<bool>& a, const IMap<bool>& b) {
		HMap<bool> result(intersect(a, b));
		for (int y = result.y1; y < result.y2; ++y)
			for (int x = result.x1; x < result.x2; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	// Bitwise Invert operation between on a boolean Matrix
	inline HMap<bool> operator ~ (const IMap<bool>& a) {
		HMap<bool> result((hArea)a);
		for (int y = a.y1; y < a.y2; ++y)
			for (int x = a.x1; x < a.x2; ++x)
				result.set(x, y, !a.at(x, y));
		return result;
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

	struct IPolygon {

		IPolygon() {}
		virtual ~IPolygon() {}

		virtual std::size_t count() const = 0;
		virtual hVector get(std::size_t index) const = 0;

		hVector operator[](std::size_t index) const { return get(index); }

		std::vector<hVector> list() const {
			std::vector<hVector> list;
			for (int i = 0; i < count(); ++i)
				list.emplace_back(get(i));
			return list;
		}

		hVector center() const {
			hVector avg;
			for (int i = 0; i < count(); ++i)
				avg += get(i);
			return avg / static_cast<hType_f>(count());
		}

		hType_f perimeter() const {
			hType_f length = 0._hf;
			for (int i = 0; i < count() - 1; ++i)
				length += (get(i) - get(i + 1)).length();
			return length;
		}

		hType_f perimeter_closed() const {
			hType_f length = 0._hf;
			for (int i = 0; i < count() - 1; ++i)
				length += (get(i) - get(i + 1)).length();
			length += (get(0) - get(count() - 1)).length();
			return length;
		}

	}; // struct IPolygon


	struct hPolygon : public IPolygon {
		std::vector<hVector> vertices;

		hPolygon() {}
		hPolygon(std::initializer_list<hVector> verts) : vertices(verts) {}
		hPolygon(const IPolygon & polygon) : vertices(polygon.list()) {}

		std::size_t count() const override { return vertices.size(); }
		hVector get(std::size_t index) const { return vertices[index]; }
	}; // struct hPolygon


	template <unsigned int N>
	struct IPolygonN : public IPolygon {
		std::size_t count() const override { return N; }
	}; // struct IPolygonN<int>


	struct hBox : public IPolygonN<4> {

		hVector v1;
		hVector v2;

		hBox() : v1(0._hf), v2(1._hf) {}
		hBox(hVector size) : v1(0._hf), v2(size) {}
		hBox(hVector v1, hVector v2) : v1(v1), v2(v2) {}
		hBox(hType_f w, hType_f h) : v1(0._hf), v2(w, h) {}
		hBox(hType_f ax, hType_f ay, hType_f bx, hType_f by) : v1(ax, ay), v2(bx, by) {}
		hBox(hArea area) : v1(area.x1, area.y1), v2(area.x2, area.y2) {}

		hVector get(std::size_t index) const override {
			return { (&v1)[h_corner[index].x].x, (&v1)[h_corner[index].y].y };
		}

	}; // struct hBox


	struct hQuad : public hTransform, IPolygonN<4> {

		hQuad() : hTransform() {}
		hQuad(hType_f w, hType_f h) : hTransform({ 0,0 } ,0.f, {w, h}) {}
		hQuad(hVector dim) : hTransform() {}
		hQuad(hTransform tform) : hTransform(tform) {}

		hVector get(std::size_t index) const override {
			return position + rotation.rotate(scale * hVector(h_quad[index].x - 0.5_hf, h_quad[index].y - 0.5_hf));
		}

	}; // struct hQuad


} // namespace hrzn

#undef THROW_NOT_IMPLEMENTED

#undef _GENERATE_MATHF
#undef _GENERATE_MATH_AND
#undef _GENERATE_MATH_OR
