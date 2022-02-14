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

#define H_PI 3.1415926535897932384626433832795_hf
#define H_DEGTORAD 0.017453_hf
#define H_RADTODEG 57.29578_hf
#define H_DEG 360.0_hf
#define H_RAD 6.283185307179586476925286766559_hf
#define H_EPSILON 1.192092896e-07_hf

#define _TO_STRING_DEFERRED(n) #n
#define _TO_STRING(n) _TO_STRING_DEFERRED(n)
#define THROW_NOT_IMPLEMENTED(m) throw std::runtime_error(m " not implemented at line: " _TO_STRING(__LINE__) ", " _TO_STRING(__FILE__))

namespace hrzn {

	// Aliased FLOAT type
	typedef float hType_f;
	// Aliased INT type
	typedef int hType_i;
	// Aliased UNSIGNED INT type
	typedef unsigned int hType_u;

	constexpr hType_f operator "" _hf(long double val) { return static_cast<hType_f>(val); }
	constexpr hType_i operator "" _hi(unsigned long long int val) { return static_cast<hType_i>(val); }


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

		ITuple() : x(), y() {}

		template <typename TCast>
		ITuple(TCast xval, TCast yval) : x(static_cast<T>(xval)), y(static_cast<T>(yval)) {}

		explicit ITuple(T val) : x(val), y(val) {}

		ITuple(const ITuple<T>& t) = default;

		template <typename TCast>
		explicit ITuple(const ITuple<TCast>& t) : x(static_cast<T>(t.x)), y(static_cast<T>(t.y)) {}

		virtual ~ITuple() {}

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

		ITuple<T> normal() const {
			double l = length();
			if (l < H_EPSILON)
				return { 0._hf, 0._hf };
			double il = 1.0 / l;
			return { static_cast<T>(this->x * il), static_cast<T>(this->y * il) };
		}

		double normalize() {
			double l = length();
			if (l < H_EPSILON)
				return 0.0;
			double il = 1.0 / l;
			this->x = static_cast<T>(this->x * il);
			this->y = static_cast<T>(this->y * il);
			return l;
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

	// Type aliases for Tuples of FLOAT types.
	using hVector = ITuple<hType_f>;
	// Type alias for Tuples of INT types.
	using hPoint = ITuple<hType_i>;

	/// <summary>
	/// A class for managing rotations and angles.
	/// </summary>
	struct hRotation {

		hType_f tau;

		hRotation() : tau(0.f) {}

		hRotation(hType_f a) : tau(a) {}

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

	struct hArea_iterable; // Forward declaration
	
	/// <summary>
	/// A class for managing coordinates of a 2D area.
	/// </summary>
	struct hArea {

		hType_i x1, y1, x2, y2;

		hArea() : x1(INT_MAX), y1(INT_MAX), x2(INT_MIN), y2(INT_MIN) {}

		hArea(hType_u w, hType_u h) : x1(0), y1(0), x2((hType_i)w - 1), y2((hType_i)h - 1) {}

		hArea(hType_i x_1, hType_i y_1, hType_i x_2, hType_i y_2) : x1(x_1), y1(y_1), x2(x_2), y2(y_2) {}

		explicit hArea(hPoint p1, hPoint p2) : x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y) {}

		hArea_iterable iterable() const;
		hArea_iterable iterate_x(int y) const;
		hArea_iterable iterate_y(int x) const;

		hArea& operator +=(hPoint dist) {
			move(dist.x, dist.y);
			return *this;
		}

		hArea& operator -=(hPoint dist) {
			move(-dist.x, -dist.y);
			return *this;
		}

		operator bool() const {
			return x1 <= x2 && y1 <= y2;
		}

		bool valid() const {
			return x1 <= x2 && y1 <= y2;
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
				resize(std::min(x1, x1 + size.x), std::min(y1, y1 + size.y), std::max(x1, x1 + size.x) - 1, std::max(y1, y1 + size.y) - 1);
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
				resize(ctr.x - w / 2, ctr.y - h / 2, x1 + w - 1, y1 + h - 1);
			}
		}

		hArea normalize() const {
			if (valid())
				return hArea(0, 0, x2 - x1, y2 - y1);
			else
				return hArea();
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
			return std::max(x2 - x1 + 1_hi, 0_hi);
		}

		std::size_t height() const {
			return std::max(y2 - y1 + 1_hi, 0_hi);
		}

		hPoint dimensions() const {
			return { width(), height() };
		}

		std::size_t area() const {
			return width() * height();
		}

		hPoint start() const {
			return { x1, y1 };
		}

		hPoint end() const {
			return { x2 + 1_hi, y2 + 1_hi };
		}

		hPoint center() const {
			return { (x2 - x1) / 2 + x1, (y2 - y1) / 2 + y1 };
		}

		hPoint upperLeft() const {
			return { x1, y1 };
		}

		hPoint upperRight() const {
			return { x2, y1 };
		}

		hPoint bottomLeft() const {
			return { x1, y2 };
		}

		hPoint bottomRight() const {
			return { x2, y2 };
		}

		bool contains(const hPoint& pt) const {
			return this->contains(pt.x, pt.y);
		}

		bool contains(hType_i x, hType_i y) const {
			return x >= x1 && x <= x2 && y >= y1 && y <= y2;
		}

		/// Create an hArea object using position, width, and height.
		static hArea build(hType_i x, hType_i y, hType_i width, hType_i height) {
			return hArea(x, y, x + width - 1, y + height - 1);
		}

		// Create an hArea object that contains all listed points.
		static hArea buildBoundary(std::initializer_list<hPoint> pts) {
			hArea r; // (pts.begin()->x, pts.begin()->y, pts.begin()->x, pts.begin()->y);
			for (auto p : pts) {
				r.x1 = std::min(r.x1, p.x);
				r.y1 = std::min(r.y1, p.y);
				r.x2 = std::max(r.x2, p.x);
				r.y2 = std::max(r.y2, p.y);
			}
			return r;
		}

		static hArea buildRadius(hType_i x, hType_i y, hType_i radius) {
			return hArea(x - radius, y - radius, x + radius, y + radius);
		}

		static hArea intersect(const hArea& a, const hArea& b) {
			return hArea(std::max(a.x1, b.x1), std::max(a.y1, b.y1), std::min(a.x2, b.x2), std::min(a.y2, b.y2));
		}

	}; // struct hArea

	inline bool operator==(const hArea& a, const hArea& b) {
		return a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2;
	}


	/// <summary>
	/// A helper class for iterating through all points in an area.
	/// </summary>
	struct hArea_iterable : public hArea {

		struct iterator : public hPoint {
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;

			iterator(hType_i i, hType_i w, hType_i x, hType_i y) : m_i(i), m_w(w), m_x(x), m_y(y) {
				hPoint::x = m_x + (hType_i)(m_i % m_w);
				hPoint::y = m_y + (hType_i)(m_i / m_w);
			}

			hPoint& operator*() { return *this; }
			hPoint* operator->() { return this; }

			// Prefix increment
			iterator& operator++() {
				m_i++;
				hPoint::x = m_x + (hType_i)(m_i % m_w);
				hPoint::y = m_y + (hType_i)(m_i / m_w);
				return *this;
			}

			// Postfix increment
			iterator operator++(int) {
				iterator tmp = *this;
				++(*this);
				return tmp;
			}

			friend bool operator== (const iterator& a, const iterator& b) { return a.m_i == b.m_i; };
			friend bool operator!= (const iterator& a, const iterator& b) { return a.m_i != b.m_i; };

			std::size_t index() { return m_i; }

		private:
			std::size_t m_i;
			const hType_i m_w, m_x, m_y;
		};

		hArea_iterable(const hArea& _area) : hArea(_area) {}

		iterator begin() {
			return iterator(0, width(), x1, y1);
		}

		iterator end() {
			return iterator(area(), width(), x1, y1);
		}

	}; // struct IterableRect

	inline hArea_iterable hArea::iterable() const {
		return hArea_iterable(*this);
	}

	inline hArea_iterable hArea::iterate_x(int y) const {
		return hArea_iterable({this->x1, y, this->x2, y});
	}

	inline hArea_iterable hArea::iterate_y(int x) const {
		return hArea_iterable({ x, this->y1, x, this->y2 });
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
			for (hType_i y = y1; y <= y2; ++y)
				for (hType_i x = x1; x <= x2; ++x)
					set(x, y, obj);
		}

		void fill(fill_func f) {
			for (hType_i y = y1; y <= y2; ++y)
				for (hType_i x = x1; x <= x2; ++x)
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
	}; // class IMap<T>


	/// <summary>
	/// A helper class used for operating on the sub region of an abstract Matrix parent class.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class ISubMap : public IMap<T> {
	private:
		IMap<T>* m_source;
	public:

		using IMap<T>::operator[];
		using IMap<T>::at;
		using IMap<T>::set;
		using base = IMap<T>;

		ISubMap(const hArea& area, base& mat) : base(area), m_source(&mat) {}

		operator bool() const override { return m_source->operator bool(); }

		T& at(hType_i x, hType_i y) override { return m_source->at(x, y); }

		T at(hType_i x, hType_i y) const override { return m_source->at(x, y); }

		void set(hType_i x, hType_i y, const T& val) override { m_source->set(x, y, val); }

		base* source() { return m_source; }

		void resize(hType_i xa, hType_i ya, hType_i xb, hType_i yb) override {
			//hArea new_rect = hArea::intersect(*this, { xa, ya, xb, yb });
			//hArea::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
			hArea::resize(xa, ya, xb, yb);
		}

	}; // class ISubMap<T>


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
		HMap(std::size_t w, std::size_t h, const T& obj) : base(hArea(w, h)), m_contents(new T[w * h]) { for (auto& i : (*this)) i = obj; }
		HMap(const hArea& rect) : base(rect), m_contents(new T[rect.area()]) {}
		HMap(const hArea& rect, const T& obj) : base(rect), m_contents(new T[rect.area()]) { for (auto& i : (*this)) i = obj; }

		HMap(const HMap<T>& other) : base(other), m_contents(new T[other.area()]) {
			std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
		}

		~HMap() {
			delete[] m_contents;
			m_contents = nullptr;
		}

		T* begin() { return m_contents; }
		T* end() { return &m_contents[this->area()]; }

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
				for (hType_i y = new_rect.y1; y <= new_rect.y2; ++y)
					for (hType_i x = new_rect.x1; x <= new_rect.x2; ++x) {
						hType_i i = (x - new_rect.x1) + (y - new_rect.y1) * (hType_i)new_rect.width();
						new_block[i] = hArea::contains(x, y) ? std::move(m_contents[base::f_index(x, y)]) : fill_obj;
					}
				delete[]m_contents;
				m_contents = new_block;
			hArea::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
		}

	}; // class HMap<T>

	/// <summary>
	/// A extension of a boolean Matrix class that provides additional methods and increased performance for boolean masks.
	/// </summary>
	class HMask : public IMap<bool> {

	public:
		using block_type = std::uint64_t;

		using IMap<bool>::operator[];
		using IMap<bool>::at;
		using IMap<bool>::set;
		using base = IMap<bool>;

	private:

		static const hType_u s_bit_interval = sizeof(block_type) * CHAR_BIT;

		std::size_t m_size = 0;
		block_type* m_blocks = nullptr;
		bool m_dummy = false;

	public:

		HMask(int w, int h) : base(hArea(w, h)), m_size((this->area() / s_bit_interval) + 1), m_blocks(new block_type[m_size]) {
			for (int i = 0; i < m_size; ++i)
				m_blocks[i] = 0;
		}

		HMask(hArea area) : base(hArea(area)), m_size((this->area() / s_bit_interval) + 1), m_blocks(new block_type[m_size]) {
			for (int i = 0; i < m_size; ++i)
				m_blocks[i] = 0;
		}

		HMask(const IMap<bool>& obj) : base(obj), m_size((obj.area() / s_bit_interval) + 1), m_blocks(new block_type[m_size]) {
			for (int y = obj.y1; y <= obj.y2; ++y) {
				for (int x = obj.x1; x <= obj.x2; ++x) {
					this->set(x, y, obj.at(x, y));
				}
			}
		}

		HMask(const HMask& obj) : base(obj), m_size((obj.area() / s_bit_interval) + 1), m_blocks(new block_type[m_size]) {
			std::copy(obj.m_blocks, obj.m_blocks + m_size, m_blocks);
		}

		~HMask() {
			delete[] m_blocks;
			m_blocks = nullptr;
		}

		HMask& operator =(const IMap<bool>& other) {
			if (this != &other) {
				delete[] m_blocks;
				hArea::resize(other.x1, other.y1, other.x2, other.y2);
				m_size = (other.area() / s_bit_interval) + 1;
				m_blocks = new block_type[m_size];
				for (int y = other.y1; y <= other.y2; ++y) {
					for (int x = other.x1; x <= other.x2; ++x) {
						this->set(x, y, other.at(x, y));
					}
				}
			}
			return *this;
		}

		HMask& operator =(const HMask& other) {
			if (this != &other) {
				delete[] m_blocks;
				hArea::resize(other.x1, other.y1, other.x2, other.y2);
				m_size = (other.area() / s_bit_interval) + 1;
				m_blocks = new block_type[m_size];
				std::copy(other.m_blocks, other.m_blocks + m_size, m_blocks);
			}
			return *this;
		}

		operator bool() const override {
			return m_blocks;
		}

		void set(hType_i x, hType_i y, const bool& val) override {
			hType_u index = f_index(x, y);
			hType_u block_id = index / s_bit_interval;			
			block_type offset = 1ULL << (index % s_bit_interval);
			m_blocks[block_id] = (m_blocks[block_id] & ~offset) | (offset * val);
		}

		bool& at(int x, int y) override {
			m_dummy = f_get(x, y);
			return m_dummy;
		}

		bool at(int x, int y) const override {
			return f_get(x, y);
		}

		void fill(const bool& obj) override {
			block_type v = ~block_type() * obj;
			for (int i = 0; i < m_size; ++i)
				m_blocks[i] = v;
		}

		HMap<bool> expand() const {
			HMap<bool> obj((hArea)*this);
			for (int y = obj.y1; y <= obj.y2; ++y)
				for (int x = obj.x1; x <= obj.x2; ++x)
					obj.set(x, y, this->at(x, y));
			return obj;
		}

		void resize(hType_i xa, hType_i ya, hType_i xb, hType_i yb) override { // TODO implement
			THROW_NOT_IMPLEMENTED("HMask::resize");
			//resize(xa, ya, xb, yb, false);
		}

		void flip() {
			for (int i = 0; i < m_size; ++i)
				m_blocks[i] = ~m_blocks[i];
		}

	private:
		
		bool f_get(int x, int y) const {
			hType_u index = f_index(x, y);
			hType_u block_id = index / s_bit_interval;
			block_type offset = 1ULL << (index % s_bit_interval);
			return (m_blocks[block_id] & offset) == offset;
		}

	private:
		static block_type s_offsetBlock(const HMask & mask, hType_u x, hType_u y) {
			hType_u i = mask.f_index(x, y);
			hType_u offset_x = i % s_bit_interval;
			hType_u offset_y = i / s_bit_interval;
			block_type block = mask.m_blocks[offset_y];
			if (offset_x) {
				block = block << offset_x;
				block |= mask.m_blocks[offset_y + 1] >> (s_bit_interval - offset_x);
			}
			return block;
		}

	public:
		
		static HMask AND(const HMask& a, const HMask& b) {
			THROW_NOT_IMPLEMENTED("HMask::AND");
			HMask v(hArea::intersect(a, b));
			return v;
		}

		static HMask OR(const HMask& a, const HMask& b) {
			THROW_NOT_IMPLEMENTED("HMask::OR");
			HMask v(hArea::intersect(a, b));
			return v;
		}

		static HMask XOR(const HMask& a, const HMask& b) {
			THROW_NOT_IMPLEMENTED("HMask::XOR");
			HMask v(hArea::intersect(a, b));
			return v;
		}

		static HMask NOT(const HMask& a) {
			HMask v(a);
			v.flip();
			return v;
		}

	}; // class MatrixMask : IMap<bool>



	// Bitwise AND operation between two boolean Matrices
	inline HMask operator & (const IMap<bool>& a, const IMap<bool>& b) {
		HMask result(hArea::intersect(a, b));
		for (int y = result.y1; y <= result.y2; ++y)
			for (int x = result.x1; x <= result.x2; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	// Bitwise OR operation between two boolean Matrices
	inline HMask operator | (const IMap<bool>& a, const IMap<bool>& b) {
		HMask result(hArea::intersect(a, b));
		for (int y = result.y1; y <= result.y2; ++y)
			for (int x = result.x1; x <= result.x2; ++x)
				result.set(x, y, a.at(x, y) || b.at(x, y));
		return result;
	}

	// Bitwise XOR operation between two boolean Matrices
	inline HMask operator ^ (const IMap<bool>& a, const IMap<bool>& b) {
		HMask result(hArea::intersect(a, b));
		for (int y = result.y1; y <= result.y2; ++y)
			for (int x = result.x1; x <= result.x2; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	// Bitwise Invert operation between on a boolean Matrix
	inline HMask operator ~ (const IMap<bool>& a) {
		HMask result((hArea)a);
		for (int y = a.y1; y <= a.y2; ++y)
			for (int x = a.x1; x <= a.x2; ++x)
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

		hTransform() : position(0._hf, 0._hf), rotation(0._hf), scale(1._hf, 1._hf) {}

		hTransform(hVector p, hRotation r, hVector s) : position(p), rotation(r), scale(s) {}

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

		hTransform childTransform(const hTransform & child) const {
			return { childPositon(child.position), childRotation(child.rotation), childScale(child.scale)};
		}

	}; // struct hTransform

} // namespace hrzn