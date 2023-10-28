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

#include <iterator>

namespace hrzn {

	/******************************************************************************************************************
		Maps and Cells
	******************************************************************************************************************/


	/// <summary>
	/// An immutable object meant to hold a reference to data stored at a position.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	struct cell_pointer {
		const point2 position;
		T& contents;

		cell_pointer(point2 p, T& o) noexcept : position(p), contents(o) {}

		T& operator*() { return contents; }
		T* operator->() { return &contents; };
	}; // struct cell_pointer<T>


	/// <summary>
	/// Abstract class for all Matrix-like containers and accessors.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class I_Map : public rectangle {
	public:

		using fill_func = T(*)();
		//using fill_func = std::function<T()>; // TODO add ifdef block to check for cpp compiler version

		bool repeat_boundary = false;

		I_Map(const rectangle& area) : rectangle(area) {}

		virtual ~I_Map() {}

		// Common inherited methods
		T& operator[](point2 pt) { return at(pt.x, pt.y); }
		T operator[](point2 pt) const { return at(pt.x, pt.y); }

		T& at(point2 p) { return at(p.x, p.y); }
		T at(point2 p) const { return at(p.x, p.y); }
		void set(point2 p, const T& val) { set(p.x, p.y, val); }

		cell_pointer<T> get_cell(const point2& _p) { return { _p, at(_p) }; }

		/// <summary>
		/// Fill entire map with a single value.
		/// </summary>
		/// <param name="obj">The value with which to fill the map.</param>
		virtual void fill(const T& obj) {
			for (h_int iy = this->y; iy < last().y; ++iy)
				for (h_int ix = this->x; ix < last().x; ++ix)
					set(ix, iy, obj);
		}

		/// <summary>
		/// Swap two cell values in place.
		/// </summary>
		virtual void swap(point2 a, point2 b) {
			T value = this->at(a);
			this->set(a, this->at(b));
			this->set(b, value);
		}

		/// <summary>
		/// Flip the map in place along the X axis.
		/// </summary>
		void flip_x() {
			for (h_int ix, iy = this->y; iy < this->last().y; ++iy)
				for (ix = 0; ix < std::floor(this->w / 2); ++ix)
					this->swap({ this->x + ix, iy}, {this->last().x - ix - 1, iy});
		}

		/// <summary>
		/// Flip the map in place along the Y axis.
		/// </summary>
		void flip_y() {
			for (h_int iy, ix = this->x; ix < this->last().x; ++ix)
				for (iy = 0; iy < std::floor(height() / 2); ++iy)
					this->swap({ ix, this->y + iy}, {ix, this->last().y - iy - 1});
		}

		/// <summary>
		/// Rotate the map 180 deg (equivalent to running both flip_x() and flip_y() operations).
		/// </summary>
		void reverse() {
			//for (h_int x, y =  0; y < std::floor(height() / 2); ++y)
			//	for (x = 0; x < std::floor(width() / 2); ++x)
			//		this->swap({ this->x1 + x, this->y1 + y }, { this->x2 - x - 1, this->y2 - y - 1 });
			std::reverse(this->begin(), this->end());
		}

		// Abstract methods
		virtual operator bool() const = 0;
		virtual T& at(h_int px, h_int py) = 0;
		virtual T at(h_int px, h_int py) const = 0;
		virtual void set(h_int px, h_int py, const T& val) = 0;

	protected:
		std::size_t f_index(h_int px, h_int py) const {
#ifndef HRZN_NOEXCEPTIONS
			if (!contains(px, py)) throw std::out_of_range("Point not located in I_Map.");
#endif // !HRZN_NOEXCEPTIONS
			return (px - this->x) + (py - this->y) * this->w;
		}

	public:
		class Iterator : public point2 {
		public:

			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			I_Map& map;

			Iterator(I_Map& m, point2 p) : point2(p), map(m) {}

			value_type operator *() const { return get(); }
			reference operator *() { return get(); }
			pointer operator ->() { return &(get()); }

			reference get() { return map.at(this->x, this->y); }
			value_type get() const { return map.at(this->x, this->y); }

			operator bool() { return *this != map.end(); }

			// Prefix increment
			Iterator& operator++() {
				this->y = this->y + (this->x == (map.x + map.w - 1_hi));
				this->x = (this->x - map.x + 1) % map.w + map.x;
				return *this;
			}

			// Postfix increment
			Iterator operator++(int) {
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			// Prefix decrement
			Iterator& operator--() {
				this->y = this->y - (this->x == map.x);
				this->x = (this->x - map.x - 1 + map.w) % map.w + map.x;
				return *this;
			}

			// Postfix decrement
			Iterator operator--(int) {
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}


		}; // struct I_Map>T>::Iterator

		Iterator begin() { return Iterator(*this, { this->x, this->y }); }
		Iterator end() { return Iterator(*this, { this->x, this->y + this->h }); }

	}; // class I_Map<T>


	/// <summary>
	/// A helper class used for operating on the sub region of an abstract Matrix parent class.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MapReference : public I_Map<T> {
	private:
		I_Map<T>* m_source;
	public:

		using I_Map<T>::operator[];
		using I_Map<T>::at;
		using I_Map<T>::set;
		using base = I_Map<T>;

		MapReference(const rectangle& area, base& mat) : base(area), m_source(&mat) {}

		operator bool() const override { return m_source->operator bool(); }

		T& at(h_int _x, h_int _y) override { return m_source->at(_x, _y); }

		T at(h_int _x, h_int _y) const override { return m_source->at(_x, _y); }

		void set(h_int _x, h_int _y, const T& val) override { m_source->set(_x, _y, val); }

		base* source() { return m_source; }

	}; // class MapReference<T>


	/// <summary>
	/// Create a referance to a map sub-area.
	/// </summary>
	/// <param name="area">The sub area of the base map to reference</param>
	/// <param name="map">The base map object from which to to create a reference area</param>
	template<typename T>
	MapReference<T> GetReferenceArea(rectangle area, I_Map<T>& map) {
		rectangle i_area = hrzn::intersect(area, map);
		return MapReference<T>(i_area, map);
	}

	/// <summary>
	/// A map object that hides the referenced map data type but provides direct access to contained object type member. 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="TRef"></typeparam>
	template <typename T, typename TRef>
	class MapReader : public I_Map<T> {
	public:
		using I_Map<T>::operator[];
		using I_Map<T>::at;
		using I_Map<T>::set;
		using base = I_Map<T>;
	private:

		T TRef::* m_member_ptr;

		I_Map<TRef>* m_reference;

	public:

		MapReader(I_Map<TRef>& reference, T TRef::* m_ptr) : base((rectangle)reference), m_reference(&reference), m_member_ptr(m_ptr) {}

		operator bool() const override { return m_reference->operator bool(); }

		T& at(h_int px, h_int py) override { return m_reference->at(px, py).*m_member_ptr; }

		T at(h_int px, h_int py) const override { return m_reference->at(px, py).*m_member_ptr; }

		void set(h_int px, h_int py, const T& val) override { m_reference->at(px, py).*m_member_ptr = val; }

	}; // class MapReader<T>


	/// <summary>
	/// I_Map container for storing any variables by position in a 2 dimensional grid.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MapContainer : public I_Map<T> {
	public:

		using I_Map<T>::operator[];
		using I_Map<T>::at;
		using I_Map<T>::set;
		using base = I_Map<T>;

	private:

		T* m_contents = nullptr;
		// TODO keep a member variable for contents size. Used independently from parent rectangle class.

	public:

		MapContainer() : base(rectangle()) {}
		MapContainer(std::size_t _w, std::size_t _h) : base(rectangle(_w, _h)), m_contents(new T[_w * _h]) {}
		MapContainer(std::size_t _w, std::size_t _h, const T& obj) : base(rectangle(_w, _h)), m_contents(new T[_w * _h]) { for (int i = 0; i < (_w * _h); ++i) m_contents[i] = obj; }
		MapContainer(const rectangle& rect) : base(rect), m_contents(new T[rect.area()]) {}
		MapContainer(const rectangle& rect, const T& obj) : base(rect), m_contents(new T[rect.area()]) { for (int i = 0; i < rect.area(); ++i) m_contents[i] = obj; }

		MapContainer(const MapContainer<T>& other) : base((rectangle)other), m_contents(new T[other.area()]) {
			std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
		}

		MapContainer(MapContainer<T>&& other) noexcept : base((rectangle)other), m_contents(other.m_contents) {
			other.m_contents = nullptr;
			other.w = 0;
			other.h = 0;
		}

		~MapContainer() {
			if (m_contents) delete[] m_contents;
			m_contents = nullptr;
		}

		MapContainer<T>& operator =(const MapContainer<T>& other) {
			if (this != &other) {
				delete[] m_contents;
				m_contents = new T[other.area()];
				std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
				//rectangle::resize(other.x1, other.y1, other.x2, other.y2);
				this->x = other.x;
				this->y = other.y;
				this->w = other.w;
				this->h = other.h;
			}
			return *this;
		}

		operator bool() const override { return m_contents; }

		T& operator[](std::size_t i) { return m_contents[i]; }

		const T& operator[](std::size_t i) const { return m_contents[i]; }

		T& at(h_int px, h_int py) override {
			return m_contents[this->f_index(px, py)];
		}

		T at(h_int px, h_int py) const override {
			return m_contents[this->f_index(px, py)];
		}

		void set(h_int px, h_int py, const T& val) override {
			m_contents[this->f_index(px, py)] = val;
		}

		bool valid() const {
			return m_contents;
		}

	}; // class MapContainer<T>


	/// <summary>
	/// A map container which returns a single reference for all positions. 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MapSingleton : public I_Map<T> {
	public:

		using I_Map<T>::operator[];
		using I_Map<T>::at;
		using I_Map<T>::set;
		using base = I_Map<T>;

	private:

		T m_contents;

	public:

		MapSingleton() : base(rectangle()) {}

		T& at(h_int px, h_int py) override {
			return m_contents;
		}

		T at(h_int px, h_int py) const override {
			return m_contents;
		}

		void set(h_int px, h_int py, const T& val) override {
			m_contents = val;
		}

	}; // class MapSingleton<T>


	/// Bitwise AND operation between two boolean Matrices
	inline MapContainer<bool> operator & (const I_Map<bool>& a, const I_Map<bool>& b) {
		MapContainer<bool> result(intersect(a, b));
		for (int y = result.y; y < result.last().y; ++y)
			for (int x = result.x; x < result.last().x; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	/// Bitwise OR operation between two boolean Matrices
	inline MapContainer<bool> operator | (const I_Map<bool>& a, const I_Map<bool>& b) {
		MapContainer<bool> result(intersect(a, b));
		for (int y = result.y; y < result.last().y; ++y)
			for (int x = result.x; x < result.last().x; ++x)
				result.set(x, y, a.at(x, y) || b.at(x, y));
		return result;
	}

	/// Bitwise XOR operation between two boolean Matrices
	inline MapContainer<bool> operator ^ (const I_Map<bool>& a, const I_Map<bool>& b) {
		MapContainer<bool> result(intersect(a, b));
		for (int y = result.y; y < result.last().y; ++y)
			for (int x = result.x; x < result.last().x; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	/// Bitwise Invert operation between on a boolean Matrix
	inline MapContainer<bool> operator ~ (const I_Map<bool>& a) {
		MapContainer<bool> result((rectangle)a);
		for (int y = a.y; y < a.last().y; ++y)
			for (int x = a.x; x < a.last().x; ++x)
				result.set(x, y, !a.at(x, y));
		return result;
	}

	/// <summary>
	/// Explicit copy from an abstract map into a map container
	/// </summary>
	template <typename T>
	MapContainer<T> copy(const I_Map<T>& map) {
		MapContainer<T> result((rectangle)map);
		for (int x, y = map.y; y < map.last().y; ++y)
			for (x = map.x; x < map.last().x; ++x)
				result.set(x, y, map.at(x, y));
		return result;
	}

	/// <summary>
	/// Compare the intersecting area of two maps. Returns true if all cells in the intersection are the same.
	/// </summary>
	template <typename T>
	inline bool equal(const I_Map<T>& a, const I_Map<T>& b) {
		rectangle area = hrzn::intersect(a, b);
		for (h_int x, y = area.y; y < area.last().y; ++y)
			for (x = area.x; x < area.last().x; ++x)
				if (a.at(x, y) != b.at(x, y))
					return false;
		return true;
	}

	/// <summary>
	/// Find and replaces all values in map that match the proveded value and replace it with another.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="map">The map on which the operation is performed in-place.</param>
	/// <param name="find">The value to search for in the map.</param>
	/// <param name="replace">The value with which to replace all found values.</param>
	template <typename T>
	inline void replace(I_Map<T>& map, const T& find, const T& replace) {
		for (h_int x, y = map.y; y < map.last().y; ++y)
			for (x = map.x; x < map.last().x; ++x)
				map.at(x, y) = replace;
	}

} // namespace hrzn 