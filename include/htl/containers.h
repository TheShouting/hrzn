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
	/// An abstract container class which holds data that is tied to an XY position.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class Cell : public point2 {
	public:


		Cell() : point2() {}
		Cell(point2 _position) : point2(_position) {}
		Cell(h_int _x, h_int _y) : point2(_x, _y) {}

		point2 position() const { return { x, y }; }

		Cell<T> & operator=(const Cell<T>& other) {
			x = other.x;
			y = other.y;
			get() = other.get();
			return *this;
		}

		virtual T& get() = 0;
		virtual T get() const = 0;
	}; // class Cell<T>


	/// <summary>
	/// A cell object that holds a copy of a veriable.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class CellObject : public Cell<T> {
	private:
		T m_contents;
	public:
		using Cell<T>::get;

		CellObject() : Cell<T>(), m_contents() {}
		CellObject(h_int _x, h_int _y) : Cell<T>(_x, _y), m_contents() {}
		CellObject(point2 _position) : Cell<T>(_position), m_contents() {}
		CellObject(h_int _x, h_int _y, const T & _contents) : Cell<T>(_x, _y), m_contents(_contents) {}
		CellObject(point2 _position, const T & _contents) : Cell<T>(_position), m_contents(_contents) {}
		CellObject(const Cell<T> & _other) : Cell<T>(_other), m_contents(_other.get()) {}

		T& get() override { return m_contents; }
		T get() const override { return m_contents; }
	}; // class CellObject<T>


	/// <summary>
	/// A cell object that holds a reference to a Map element.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class CellReference : public Cell<T> {
	private:
		T & m_contents;
	public:
		using Cell<T>::get;

		CellReference(h_int _x, h_int _y, const T& _contents) : Cell<T>(_x, _y), m_contents(_contents) {}
		CellReference(point2 _position, const T _contents) : Cell<T>(_position), m_contents(_contents) {}
		CellReference(Cell<T>& _other) : Cell<T>(_other), m_contents(_other.get()) {}

		T& get() override { return m_contents; }
		T get() const override { return m_contents; }
	}; // CellRefereence<T>


	/// <summary>
	/// Abstract class for all Matrix-like containers and accessors.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class Map : public point_area {
	public:

		using fill_func = T(*)();
		//using fill_func = std::function<T()>; // TODO add ifdef block to check for cpp compiler version

		bool repeat_boundary = false;

		Map(const point_area& area) : point_area(area) {}

		virtual ~Map() {}

		// Common inherited methods
		T& operator[](point2 pt) { return at(pt.x, pt.y); }
		T operator[](point2 pt) const { return at(pt.x, pt.y); }

		T& at(point2 p) { return at(p.x, p.y); }
		T at(point2 p) const { return at(p.x, p.y); }
		void set(point2 p, const T& val) { set(p.x, p.y, val); }

		CellObject<T> getCell(const point2 & _p) const { return CellObject<T>(_p, at(_p)); }

		/// <summary>
		/// Fill entire map with a single value.
		/// </summary>
		/// <param name="obj">The value with which to fill the map.</param>
		virtual void fill(const T& obj) {
			for (h_int y = y1; y < y2; ++y)
				for (h_int x = x1; x < x2; ++x)
					set(x, y, obj);
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
		void flipX() {
			for (h_int x, y = y1; y < y2; ++y)
				for (x = 0; x < std::floor(width() / 2); ++x)
					this->swap({ this->x1 + x, y }, { this->x2 - x - 1, y });
		}

		/// <summary>
		/// Flip the map in place along the Y axis.
		/// </summary>
		void flipY() {
			for (h_int y, x = x1; x < x2; ++x)
				for (y = 0; y < std::floor(height() / 2); ++y)
					this->swap({ x, this->y1 + y }, { x, this->y2 - y - 1 });
		}

		/// <summary>
		/// Rotate the map 180 deg (equivalent to running both flipX() and flipY() operations).
		/// </summary>
		void reverse() {
			//for (h_int x, y =  0; y < std::floor(height() / 2); ++y)
			//	for (x = 0; x < std::floor(width() / 2); ++x)
			//		this->swap({ this->x1 + x, this->y1 + y }, { this->x2 - x - 1, this->y2 - y - 1 });
			std::reverse(this->begin(), this->end());
		}

		// Abstract methods
		virtual operator bool() const = 0;
		virtual T& at(h_int x, h_int y) = 0;
		virtual T at(h_int x, h_int y) const = 0;
		virtual void set(h_int x, h_int y, const T& val) = 0;

	protected:
		std::size_t f_index(h_int x, h_int y) const {
#ifndef H_NOEXCEPTIONS
			if (!contains(x, y)) throw std::out_of_range("Point not located in Map.");
#endif // !H_NOEXCEPTIONS
			return (x - x1) + (y - y1) * width();
		}

	public:
		class Iterator : public Cell<T> {
		public:
			using Cell<T>::get;

			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			Map& map;

			Iterator(Map& m, point2 p) : Cell<T>(p), map(m) {}

			value_type operator *() const { return get(); } // TODO move dereference and pointer operators to base Cell class
			reference operator *() { return get(); }
			pointer operator ->() { return &(get()); }

			reference get() override { return map.at(this->x, this->y); }
			value_type get() const override { return map.at(this->x, this->y); }

			operator bool() { return *this != map.end(); }

			// Prefix increment
			Iterator& operator++() {
				this->y = this->y + (this->x == (map.x2 - 1_hi));
				this->x = (this->x - map.x1 + 1) % map.width() + map.x1;
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
				this->y = this->y - (this->x == (map.x1));
				this->x = (this->x - map.x1 - 1 + map.width()) % map.width() + map.x1;
				return *this;
			}

			// Postfix decrement
			Iterator operator--(int) {
				Iterator tmp = *this;
				--(*this);
				return tmp;
			}


		}; // struct Map>T>::Iterator

		Iterator begin() { return Iterator(*this, { x1, y1 }); }
		Iterator end() { return Iterator(*this, { x1, y2 }); }

	}; // class Map<T>


	/// <summary>
	/// A helper class used for operating on the sub region of an abstract Matrix parent class.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MapReference : public Map<T> {
	private:
		Map<T>* m_source;
	public:

		using Map<T>::operator[];
		using Map<T>::at;
		using Map<T>::set;
		using base = Map<T>;

		MapReference(const point_area& area, base& mat) : base(area), m_source(&mat) {}

		operator bool() const override { return m_source->operator bool(); }

		T& at(h_int x, h_int y) override { return m_source->at(x, y); }

		T at(h_int x, h_int y) const override { return m_source->at(x, y); }

		void set(h_int x, h_int y, const T& val) override { m_source->set(x, y, val); }

		base* source() { return m_source; }

	}; // class MapReference<T>


	/// <summary>
	/// Create a referance to a map sub-area.
	/// </summary>
	/// <param name="area">The sub area of the base map to reference</param>
	/// <param name="map">The base map object from which to to create a reference area</param>
	template<typename T>
	MapReference<T> GetReferenceArea(point_area area, Map<T>& map) {
		point_area i_area = hrzn::intersect(area, map);
		return MapReference<T>(i_area, map);
	}

	/// <summary>
	/// A map object that hides the referenced map data type and only exposes a member variable for access. 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="TRef"></typeparam>
	template <typename T, typename TRef>
	class MapReader : public Map<T> {
	public:
		using Map<T>::operator[];
		using Map<T>::at;
		using Map<T>::set;
		using base = Map<T>;
	private:

		T TRef::* m_member_ptr;

		Map<TRef>* m_reference;

	public:

		MapReader(Map<TRef>& reference, T TRef::* m_ptr) : base((point_area)reference), m_reference(&reference), m_member_ptr(m_ptr) {}

		operator bool() const override { return m_reference->operator bool(); }

		T& at(h_int x, h_int y) override { return m_reference->at(x, y).*m_member_ptr; }

		T at(h_int x, h_int y) const override { return m_reference->at(x, y).*m_member_ptr; }

		void set(h_int x, h_int y, const T& val) override { m_reference->at(x, y).*m_member_ptr = val; }

		//void resize(h_int xa, h_int ya, h_int xb, h_int yb) override {
		//	point_area new_rect = hrzn::intersect(*this, { xa, ya, xb, yb });
		//	point_area::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
		//}
	};



	/// <summary>
	/// Map container for storing any variables by position in a 2 dimensional grid.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MapContainer : public Map<T> {
	public:

		using Map<T>::operator[];
		using Map<T>::at;
		using Map<T>::set;
		using base = Map<T>;

	private:

		T* m_contents = nullptr;
		// TODO keep a member variable for contents size. Used independently from parent point_area class.

	public:

		MapContainer() : base(point_area()) {}
		MapContainer(std::size_t w, std::size_t h) : base(point_area(w, h)), m_contents(new T[w * h]) {}
		MapContainer(std::size_t w, std::size_t h, const T& obj) : base(point_area(w, h)), m_contents(new T[w * h]) { for (int i = 0; i < (w * h); ++i) m_contents[i] = obj; }
		MapContainer(const point_area& rect) : base(rect), m_contents(new T[rect.area()]) {}
		MapContainer(const point_area& rect, const T& obj) : base(rect), m_contents(new T[rect.area()]) { for (int i = 0; i < rect.area(); ++i) m_contents[i] = obj; }

		MapContainer(const MapContainer<T>& other) : base((point_area)other), m_contents(new T[other.area()]) {
			std::copy(other.m_contents, other.m_contents + other.area(), m_contents);
		}

		MapContainer(MapContainer<T>&& other) noexcept : base((point_area)other), m_contents(other.m_contents) {
			other.m_contents = nullptr;
			other.x1 = other.x2;
			other.y1 = other.y2;
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
				point_area::resize(other.x1, other.y1, other.x2, other.y2);
			}
			return *this;
		}

		operator bool() const override { return m_contents; }

		T& operator[](std::size_t i) { return m_contents[i]; }

		const T& operator[](std::size_t i) const { return m_contents[i]; }

		T& at(h_int x, h_int y) override {
			return m_contents[this->f_index(x, y)];
		}

		T at(h_int x, h_int y) const override {
			return m_contents[this->f_index(x, y)];
		}

		void set(h_int x, h_int y, const T& val) override {
			m_contents[this->f_index(x, y)] = val;
		}

		bool valid() const {
			return m_contents;
		}

		//void resize(h_int xa, h_int ya, h_int xb, h_int yb) override {
		//	resize(xa, ya, xb, yb, T());
		//}

		//void resize(h_int xa, h_int ya, h_int xb, h_int yb, const T& fill_obj) {
		//	assert(m_contents != nullptr);
		//	point_area new_rect(xa, ya, xb, yb);
		//	T* new_block = new T[new_rect.area()];
		//	for (h_int y = new_rect.y1; y < new_rect.y2; ++y)
		//		for (h_int x = new_rect.x1; x < new_rect.x2; ++x) {
		//			h_int i = (x - new_rect.x1) + (y - new_rect.y1) * (h_int)new_rect.width();
		//			new_block[i] = point_area::contains(x, y) ? std::move(m_contents[base::f_index(x, y)]) : fill_obj;
		//		}
		//	delete[]m_contents;
		//	m_contents = new_block;
		//	point_area::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
		//}

	}; // class MapContainer<T>


	/// <summary>
	/// A map container which returns a single reference for all positions. 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class MapSingleton : public Map<T> {
	public:

		using Map<T>::operator[];
		using Map<T>::at;
		using Map<T>::set;
		using base = Map<T>;

	private:

		T m_contents;

	public:

		MapSingleton() : base(point_area()) {}

		T& at(h_int x, h_int y) override {
			return m_contents;
		}

		T at(h_int x, h_int y) const override {
			return m_contents;
		}

		void set(h_int x, h_int y, const T& val) override {
			m_contents = val;
		}
	};


	/// Bitwise AND operation between two boolean Matrices
	inline MapContainer<bool> operator & (const Map<bool>& a, const Map<bool>& b) {
		MapContainer<bool> result(intersect(a, b));
		for (int y = result.y1; y < result.y2; ++y)
			for (int x = result.x1; x < result.x2; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	/// Bitwise OR operation between two boolean Matrices
	inline MapContainer<bool> operator | (const Map<bool>& a, const Map<bool>& b) {
		MapContainer<bool> result(intersect(a, b));
		for (int y = result.y1; y < result.y2; ++y)
			for (int x = result.x1; x < result.x2; ++x)
				result.set(x, y, a.at(x, y) || b.at(x, y));
		return result;
	}

	/// Bitwise XOR operation between two boolean Matrices
	inline MapContainer<bool> operator ^ (const Map<bool>& a, const Map<bool>& b) {
		MapContainer<bool> result(intersect(a, b));
		for (int y = result.y1; y < result.y2; ++y)
			for (int x = result.x1; x < result.x2; ++x)
				result.set(x, y, a.at(x, y) && b.at(x, y));
		return result;
	}

	/// Bitwise Invert operation between on a boolean Matrix
	inline MapContainer<bool> operator ~ (const Map<bool>& a) {
		MapContainer<bool> result((point_area)a);
		for (int y = a.y1; y < a.y2; ++y)
			for (int x = a.x1; x < a.x2; ++x)
				result.set(x, y, !a.at(x, y));
		return result;
	}

	/// <summary>
	/// Explicit copy from an abstract map into a map container
	/// </summary>
	template <typename T>
	MapContainer<T> copy(const Map<T>& map) {
		MapContainer<T> result((point_area)map);
		for (h_int x, y = map.y1; y < map.y2; ++y)
			for (x = map.x1; x < map.x2; ++x)
				result.set(x, y, map.at(x, y));
		return result;
	}

	/// <summary>
	/// Compare the intersecting area of two maps. Returns true if all cells in the intersection are the same.
	/// </summary>
	template <typename T>
	inline bool equal(const Map<T>& a, const Map<T>& b) {
		point_area area = hrzn::intersect(a, b);
		for (h_int x, y = area.y1; y < area.y2; ++y)
			for (x = area.x1; x < area.x2; ++x)
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
	inline void replace(Map<T>& map, const T& find, const T& replace) {
		for (h_int x, y = map.y1; y < map.y2; ++y)
			for (x = map.x1; x < map.x2; ++x)
				map.at(x, y) = replace;
	}

} // namespace hrzn 