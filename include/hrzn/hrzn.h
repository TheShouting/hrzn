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
#include <cstddef>

#define U_PI 3.1415926535897932384626433832795_hf
#define U_DEGTORAD 0.017453_hf
#define U_RADTODEG 57.29578_hf
#define U_DEG 360.0_hf
#define U_RAD 6.283185307179586476925286766559_hf
#define U_EPSILON 1.192092896e-07_hf

namespace hrzn {

	typedef float hType_f;
	typedef int hType_i;
	typedef unsigned int hType_u;

	constexpr hType_f operator "" _hf(long double val) { return static_cast<hType_f>(val); }
	constexpr hType_i operator "" _hi(unsigned long long int val) { return static_cast<hType_i>(val); }

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

		T lengthSqr() const { return this->x * this->x + this->y * this->y; }

		T lengthManhattan() const { return std::abs(this->x) + std::abs(this->y); }

		double normalize() {
			double l = length();
			if (l < U_EPSILON)
			{
				return 0.0;
			}
			double il = 1.0 / l;
			this->x = static_cast<T>(this->x * il);
			this->y = static_cast<T>(this->y * il);
			return l;
		}

	}; // struct ITuple<T>


	// Operator overloads for ITuple objects
	template <typename T>
	ITuple<T> operator - (const ITuple<T>& a) {
		return ITuple<T>(-a.x, -a.y);
	}

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

	// Type aliases for float-like tuples and int-like tuples
	using hVector = ITuple<hType_f>;
	using hPoint = ITuple<hType_i>;

	/// <summary>
	/// A class for managing rotations and angles.
	/// </summary>
	struct hRotation {

		hType_f tau;

		hRotation() : tau(0.f) {}

		hRotation(hType_f a) : tau(a - std::floor(a)) {}

		hRotation operator-() const { return hRotation(tau + U_DEG * 0.5f); }

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
			hType_f val = tau + (deg / U_DEG);
			tau = val - std::floor(val);
			return tau * U_DEG;
		}

		auto addRad(hType_f rad) {
			hType_f val = tau + (rad / U_RAD);
			tau = val - std::floor(val);
			return tau * U_RAD;
		}

		void setDeg(hType_f deg) {
			hType_f a = deg / U_DEG;
			tau = a - std::floor(a);
		}

		void setRad(hType_f rad) {
			hType_f a = rad / U_RAD;
			tau = a - std::floor(a);
		}

		inline hType_f deg() const { return tau * U_DEG; }
		inline hType_f rad() const { return tau * U_RAD; }

		hRotation flipped() const {
			return hRotation(tau + 0.5_hf);
		}

		hRotation inverse() const {
			return hRotation(1.0_hf - tau);
		}

		void setWithVector(hVector vec) {
			tau = std::atan2(vec.y, vec.x) / U_RAD;
			tau = tau - std::floor(tau);
		}

		hVector getForwardVector(hType_f length = 1._hf) const {
			return hVector(std::cos(rad()) * length, std::sin(rad()) * length);
		}

		hVector getRightVector(hType_f length = 1._hf) const {
			return hVector(std::cos(rad() + U_PI) * length, std::sin(rad() + U_PI) * length);
		}

		hVector rotate(hVector const& vec) const {
			hType_f x = std::cos(rad()) * vec.x - std::sin(rad()) * vec.y;
			hType_f y = std::sin(rad()) * vec.x + std::cos(rad()) * vec.y;
			return hVector(x, y);
		}

		hVector unrotate(hVector const& vec) const {
			hType_f x = std::cos(-rad()) * vec.x - std::sin(-rad()) * vec.y;
			hType_f y = std::sin(-rad()) * vec.x + std::cos(-rad()) * vec.y;
			return hVector(x, y);
		}

		static hRotation difference(hRotation a, hRotation b) {
			hType_f diff = a.tau - b.tau + 0.5f;
			return hRotation(diff - std::floor(diff) - 0.5f);
		}

		static hRotation makeDeg(hType_f deg) { return hRotation(deg / U_DEG); }

		static hRotation makeRad(hType_f rad) { return hRotation(rad / U_RAD); }

	}; // hRotation


	struct IterableRect; // Forward declaration

	struct Rect {

		hType_i x1, y1, x2, y2;

		Rect() : x1(0), y1(0), x2(0), y2(0) {}

		Rect(std::size_t w, std::size_t h) {
			x1 = 0;
			y1 = 0;
			x2 = (hType_i)w - 1;
			y2 = (hType_i)h - 1;
		}

		Rect(hType_i x_1, hType_i y_1, hType_i x_2, hType_i y_2)
		{
			x1 = std::min(x_1, x_2);
			x2 = std::max(x_1, x_2);
			y1 = std::min(y_1, y_2);
			y2 = std::max(y_1, y_2);
		}

		explicit Rect(hPoint pos, hPoint sz) {
			x1 = pos.x;
			y1 = pos.y;
			x2 = pos.x + sz.x - 1;
			y2 = pos.y + sz.y - 1;
		}

		IterableRect iterable();

		virtual void set(hType_i x_1, hType_i y_1, hType_i x_2, hType_i y_2) {
			x1 = std::min(x_1, x_2);
			x2 = std::max(x_1, x_2);
			y1 = std::min(y_1, y_2);
			y2 = std::max(y_1, y_2);
		}

		Rect& operator +=(hPoint dist) {
			move(dist.x, dist.y);
			return *this;
		}

		Rect& operator -=(hPoint dist) {
			move(-dist.x, -dist.y);
			return *this;
		}

		void move(hType_i move_x, hType_i move_y) {
			x1 += move_x;
			y1 += move_y;
			x2 += move_x;
			y2 += move_y;
		}

		void resizeFromCenter(hPoint size) {
			resizeFromCenter(size.x, size.y);
		}

		void resizeFromCenter(unsigned size) {
			resizeFromCenter(size, size);
		}

		void resizeFromCenter(unsigned w, unsigned h) {
			hPoint ctr = center();
			set(ctr.x - w / 2, ctr.y - h / 2, x1 + w - 1, y1 + h - 1);
		}

		void resize(const hPoint& _s) {
			if (_s) {
				set(std::min(x1, x1 + _s.x), std::min(y1, y1 + _s.y), std::max(x1, x1 + _s.x) - 1, std::max(y1, y1 + _s.y) - 1);
			}
		}

		Rect normalize() {
			return Rect(0, 0, x2 - x1, y2 - y1);
		}

		hPoint getClampedValue(const hPoint& pt) const {
			return { std::min(x2, std::max(x1, pt.x)), std::min(y2, std::max(y1, pt.y)) };
		}

		hPoint getWrapValue(const hPoint& pt) const {
			hType_i x = ((hType_i)width() + ((pt.x - x1) % (hType_i)width())) % (hType_i)width();
			hType_i y = ((hType_i)height() + ((pt.y - y1) % (hType_i)height())) % (hType_i)height();
			return { x, y };
		}

		std::size_t width() const {
			return x2 - x1 + 1;
		}

		std::size_t height() const {
			return y2 - y1 + 1;
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
			return { x2 + 1, y2 + 1 };
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

		/// Create a Rect object using position, width, and height.
		static Rect build(hType_i x, hType_i y, hType_i width, hType_i height) {
			return Rect(x, y, x + width - 1, y + height - 1);
		}

		static Rect buildBoundary(std::initializer_list<hPoint> pts) {
			Rect r (pts.begin()->x, pts.begin()->y, pts.begin()->x, pts.begin()->y);
			for (auto p : pts) {
				r.x1 = std::min(r.x1, p.x);
				r.y1 = std::min(r.y1, p.y);
				r.x2 = std::max(r.x2, p.x);
				r.y2 = std::max(r.y2, p.y);
			}
			return r;
		}

		static Rect buildRadius(hType_i x, hType_i y, hType_i radius) {
			return Rect(x - radius, y - radius, x + radius, y + radius);
		}

		static Rect intersect(const Rect& a, const Rect& b) {
			return Rect(std::max(a.x1, b.x1), std::max(a.y1, b.y1), std::min(a.x2, b.x2), std::min(a.y2, b.y2));
		}

	}; // struct Rect

	struct IterableRect : public Rect {

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

		IterableRect(const Rect& _rect) : Rect(_rect) {}

		iterator begin() {
			return iterator(0, width(), x1, y1);
		}

		iterator end() {
			return iterator(area(), width(), x1, y1);
		}

	}; // struct IterableRect

	IterableRect Rect::iterable() {
		return IterableRect(*this);
	}


	template <typename T>
	class Region;


	/// <summary>
	/// Abstract class for all Matrix-like containers and accessors.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class IMatrix : public Rect {
	public:

		using fill_func = T(*)();
		//using fill_func = std::function<T()>; // TODO add ifdef block to check for cpp compiler version

		IMatrix(const Rect& rect) : Rect(rect) {}

		virtual ~IMatrix() {}

		// Common inherited methods
		Region<T> region();
		Region<T> region(const Rect& area);

		T& operator[](hPoint pt) { return at(pt.x, pt.y); }
		const T& operator[](hPoint pt) const { return at(pt.x, pt.y); }

		T& at(hPoint p) { return at(p.x, p.y); }
		const T& at(hPoint p) const { return at(p.x, p.y); }

		void fill(const T& obj);
		void fill(fill_func f);

		// Abstract methods
		virtual T& at(hType_i x, hType_i y) = 0;
		virtual const T& at(hType_i x, hType_i y) const = 0;
		virtual operator bool() const = 0;

	protected:
		std::size_t m_Index(hType_i x, hType_i y) const {
			if (contains(x, y))
				return (x - x1) + (y - y1) * width();
			throw std::out_of_range("invalid matrix index");
		}
	}; // class IMatrix<T>


	/// <summary>
	/// Helper class for iterating through a specified region of a Matrix class
	/// </summary>
	/// <typeparam name="T">Element Type of the reference Matrix</typeparam>
	template<typename T>
	class Region : public Rect {

	public:
		struct RegionPoint : public hPoint {
			T* data = nullptr;
			T* operator ->() { return data; }
			T& operator *() { return *data; }
		};

		/// <summary>
		/// Iterator object for iterating through a Region object.
		/// </summary>
		class RegionIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;

		private:
			IMatrix<T>* m_source;
			Rect m_area;
			std::size_t m_index;
			RegionPoint m_container;

		public:
			RegionIterator(IMatrix<T>* source, Rect area, std::size_t i = 0) : m_source(source), m_area(area), m_index(i) {
				m_container.x = m_area.x1 + (hType_i)m_index % m_area.width();
				m_container.y = m_area.y1 + (hType_i)(m_index / m_area.width());
				if (source && i != area.area())
					m_container.data = &(m_source->at(m_container.x, m_container.y));
			}

			RegionPoint* operator ->() {
				return &m_container;
			}
			RegionPoint& operator *() {
				return m_container;
			}

			// Prefix increment
			RegionIterator& operator++() {
				m_index++;
				m_container.x = m_area.x1 + (hType_i)(m_index % m_area.width());
				m_container.y = m_area.y1 + (hType_i)(m_index / m_area.width());

				if (m_index != m_area.area())
					m_container.data = &(m_source->at(m_container.x, m_container.y));
				else
					m_container.data = nullptr;

				return *this;
			}

			// Postfix increment
			RegionIterator operator++(int) {
				RegionIterator tmp = *this;
				++(*this);
				return tmp;
			}

			friend bool operator== (const RegionIterator& a, const RegionIterator& b) {
				return (a.m_source == b.m_source) && (a.m_index == b.m_index);
			};
			friend bool operator!= (const RegionIterator& a, const RegionIterator& b) {
				return !(a == b);
			};

		}; // class RegionIterator

	private:
		IMatrix<T>* m_source;

	public:
		Region(Rect area, IMatrix<T>* source) :
			Rect(area), m_source(source) {}

		RegionIterator begin() { return RegionIterator(m_source, *this, 0); }
		RegionIterator end() { return RegionIterator(m_source, *this, this->area()); }

	}; // class Region<T>


	/// <summary>
	/// Matrix container for storing any variables by position in a 2 dimensional grid.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MatrixContainer : public IMatrix<T> {
	public:

		using IMatrix<T>::operator[];
		using IMatrix<T>::at;
		using base = IMatrix<T>;

	private:

		T* m_contents = nullptr;

	public:

		MatrixContainer() : base(Rect()) {}
		MatrixContainer(std::size_t w, std::size_t h) : base(Rect(w, h)), m_contents(new T[w * h]) {}
		MatrixContainer(std::size_t w, std::size_t h, const T& obj) : base(Rect(w, h)), m_contents(new T[w * h]) { for (auto& i : (*this)) i = obj; }
		MatrixContainer(const Rect& rect) : base(rect), m_contents(new T[rect.area()]) {}
		MatrixContainer(const Rect& rect, const T& obj) : base(rect), m_contents(new T[rect.area()]) { for (auto& i : (*this)) i = obj; }

		MatrixContainer(const MatrixContainer<T>& other) : base(other), m_contents(new T[other.area()]) {
			std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
		}

		~MatrixContainer() {
			delete[] m_contents;
			m_contents = nullptr;
		}

		T* begin() { return m_contents; }
		T* end() { return &m_contents[this->area()]; }

		MatrixContainer<T>& operator =(const MatrixContainer<T>& other) {
			if (this != &other) {
				delete[] m_contents;
				m_contents = new T[other.area()];
				std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
				Rect::set(other.x1, other.y1, other.x2, other.y2);
			}
			return *this;
		}

		operator bool() const override { return m_contents; }

		T& operator[](std::size_t i) { return m_contents[i]; }

		const T& operator[](std::size_t i) const { return m_contents[i]; }

		T& at(hType_i x, hType_i y) override {
			return m_contents[this->m_Index(x, y)];
		}

		const T& at(hType_i x, hType_i y) const override {
			return m_contents[this->m_Index(x, y)];
		}

		void set(hType_i xa, hType_i ya, hType_i xb, hType_i yb) override {
			set(xa, ya, xb, yb, T());
		}

		void set(hType_i xa, hType_i ya, hType_i xb, hType_i yb, const T& fill_obj) {
			assert(m_contents != nullptr);
			Rect new_rect(xa, ya, xb, yb);
				T* new_block = new T[new_rect.area()];
				for (hType_i y = new_rect.y1; y <= new_rect.y2; ++y)
					for (hType_i x = new_rect.x1; x <= new_rect.x2; ++x) {
						hType_i i = (x - new_rect.x1) + (y - new_rect.y1) * (hType_i)new_rect.width();
						new_block[i] = Rect::contains(x, y) ? std::move(m_contents[base::m_Index(x, y)]) : fill_obj;
					}
				delete[]m_contents;
				m_contents = new_block;
			Rect::set(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
		}

	public:
		template <typename A, typename B>
		friend class MatrixAccessor;

	}; // class MatrixContainer<T>


	/// <summary>
	/// A helper class used for operating on the sub region of an abstract Matrix parent class.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class SubMatrix : public IMatrix<T> {
	private:
		IMatrix<T>* m_source;
	public:

		using IMatrix<T>::operator[];
		using IMatrix<T>::at;
		using base = IMatrix<T>;

		SubMatrix(const Rect& area, base& mat) : base(area), m_source(&mat) {}

		operator bool() const override { return m_source->operator bool(); }

		T& at(hType_i x, hType_i y) { return m_source->at(x, y); }

		const T& at(hType_i x, hType_i y) const { return m_source->at(x, y); }

		base* source() { return m_source; }

	}; // class SubMatrix<T>


	// Type alias for MatrixContainer<bool>
	using Mask = MatrixContainer<bool>;


	template<typename T>
	inline Region<T> IMatrix<T>::region() {
		return Region<T>(Rect(*this), this);
	}

	template<typename T>
	inline Region<T> IMatrix<T>::region(const Rect& area) {
		return Region<T>(Rect::intersect(*this, area), this);
	}

	template<typename T>
	inline void IMatrix<T>::fill(const T& obj) {
		for (auto& i : this->region())
			*i = obj;
	}

	template<typename T>
	inline void IMatrix<T>::fill(fill_func f) {
		for (auto& i : this->region())
			*i = f();
	}

} // hrzn