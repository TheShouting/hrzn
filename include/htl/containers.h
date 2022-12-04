#pragma once

#include "hrzn.h"


namespace hrzn {

	/******************************************************************************************************************
		Maps and Cells
	******************************************************************************************************************/


	/// <summary>
	/// Base Cell class which holds a reference to data along with a XY position.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class ICell {
	protected:
		hType_i x, y;
	public:


		ICell() : hPoint() {}
		ICell(hPoint _position) : x(_position.x), y(_position.y) {}
		ICell(hType_i _x, hType_i _y) : x(_x), y(_y) {}

		hPoint position() const { return { x, y }; }

		T& operator *() { return get(); }
		T* operator ->() { return &(get()); }

		void operator=(const ICell<T>& other) {
			x = other.x;
			y = other.y;
			get() = other.get();
		}

		operator bool() { return x != 0 && y != 0; }

		operator hPoint() const { return { x, y }; }

		bool operator ==(const hPoint & other) { return x == other.x && y == other.y; }
		bool operator ==(const ICell<T> & other) { return x == other.x && y == other.y; }
		bool operator !=(const ICell<T> & other) { return x != other.x || y != other.y; }

		void setPosition(hPoint _position) { setPosition(_position.x, _position.y); }
		void setPosition(hType_i _x, hType_i _y) { x = _x; y = _y; }

		virtual T& get() = 0;
		virtual T get() const = 0;

	};



	template <typename T>
	class Cell : public ICell<T> {
	private:
		T m_contents;
	public:
		using ICell<T>::get;

		Cell() : hPoint(), m_contents() {}
		Cell(hType_i _x, hType_i _y) : ICell<T>(_x, _y), m_contents() {}
		Cell(hPoint _position) : ICell<T>(_position), m_contents() {}
		Cell(hType_i _x, hType_i _y, const T & _contents) : ICell<T>(_x, _y), m_contents(_contents) {}
		Cell(hPoint _position, const T & _contents) : ICell<T>(_position), m_contents(_contents) {}
		//Cell(const ICell<T> _other) : ICell<T>(_other), m_contents(_other.get()) {}

		T& get() override { return m_contents; }
		T get() const override { return m_contents; }
	};


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

		Cell<T> getCell(const hPoint & _p) const { return Cell<T>(_p, at(_p)); }

		/// <summary>
		/// Fill entire map with a single value.
		/// </summary>
		/// <param name="obj">The value with which to fill the map.</param>
		virtual void fill(const T& obj) {
			for (hType_i y = y1; y < y2; ++y)
				for (hType_i x = x1; x < x2; ++x)
					set(x, y, obj);
		}

		/// <summary>
		/// Fill map utilizing a value returned by a fill function for each cell.
		/// </summary>
		/// <param name="f"></param>
		void fill(fill_func f) {
			for (hType_i x, y = y1; y < y2; ++y)
				for (x = x1; x < x2; ++x)
					set(x, y, f());
		}

		/// <summary>
		/// Swap two cell values in place.
		/// </summary>
		virtual void swap(hPoint a, hPoint b) {
			T value = this->at(a);
			this->set(a, this->at(b));
			this->set(b, value);
		}

		/// <summary>
		/// Flip the map in place along the X axis.
		/// </summary>
		void flipX() {
			for (hType_i x, y = y1; y < y2; ++y)
				for (x = 0; x < std::floor(width() / 2); ++x)
					this->swap({ this->x1 + x, y }, { this->x2 - x - 1, y });
		}

		/// <summary>
		/// Flip the map in place along the Y axis.
		/// </summary>
		void flipY() {
			for (hType_i y, x = x1; x < x2; ++x)
				for (y = 0; y < std::floor(height() / 2); ++y)
					this->swap({ x, this->y1 + y }, { x, this->y2 - y - 1 });
		}

		/// <summary>
		/// Rotate the map 180 deg (equivalent to running both flipX() and flipY() operations).
		/// </summary>
		void reverse() {
			for (hType_i x, y =  0; y < std::floor(height() / 2); ++y)
				for (x = 0; x < std::floor(width() / 2); ++x)
					this->swap({ this->x1 + x, this->y1 + y }, { this->x2 - x - 1, this->y2 - y - 1 });
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
		class Iterator : public ICell<T> {
		public:
			using ICell<T>::get;

			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;

			IMap& map;

			Iterator(IMap& m, hPoint p) : ICell<T>(p), map(m) {}

			//T& operator *() { return map.at(x, y); }
			//T* operator ->() { return &map.at(x, y); }
			
			T& get() override { return map.at(this->x, this->y); }
			T get() const override { return map.at(this->x, this->y); }

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
			hArea new_rect = hrzn::intersect(*this, { xa, ya, xb, yb });
			hArea::resize(new_rect.x1, new_rect.y1, new_rect.x2, new_rect.y2);
		}

	}; // class HMapRef<T>


	/// <summary>
	/// Create a referance to a map sub-area.
	/// </summary>
	/// <param name="area">The sub area of the base map to reference</param>
	/// <param name="map">The base map object from which to to create a reference area</param>
	template<typename T>
	HMapRef<T> GetReferenceArea(hArea area, IMap<T>& map) {
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

	// Explicit copy from an abstract map
	template <typename T>
	HMap<T> copy(const IMap<T>& map) {
		HMap<T> result((hArea)map);
		for (hType_i x, y = map.y1; y < map.y2; ++y)
			for (x = map.x1; x < map.x2; ++x)
				result.set(x, y, map.at(x, y));
		return result;
	}

} // namespace hrzn