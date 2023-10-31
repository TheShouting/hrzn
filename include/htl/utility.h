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
#include "geometry.h"
#include "containers.h"


namespace hrzn {

	/******************************************************************************************************************
		Basic Type Operations
	******************************************************************************************************************/

	inline h_float dot_product(vector2 a, vector2 b) {
		return a.x * b.x + a.y * b.y;
	}

	inline h_float distance(vector2 a, vector2 b) {
		return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	inline h_float distance_sqr(vector2 a, vector2 b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}

	inline h_float distance_manhattan(vector2 a, vector2 b) {
		return std::abs(a.x - b.x) + std::abs(a.y - b.y);
	}

	inline vector2 get_normalized(vector2 vec) {
		double l = vec.length();
		if (l < EPSILON) {
			return vector2();
		}
		double il = 1.0f / l;
		return vector2(vec.x * il, vec.y * il);
	}

	inline angle find_rotation(vector2 v) {
		return angle::from_radians(std::atan2(v.y, v.x));
	}

	inline point2 round_to_point(vector2 vec) {
		return point2(std::round(vec.x), std::round(vec.y));
	}

	/// Create a contiguous rect_i based on the extreme corners of a rect_i list.
	template <typename T>
	inline i_rectangle<T> make_boundary(std::initializer_list<i_rectangle<T>> rects) {
		i_rectangle<T> area = *rects.begin();
		for (auto a = rects.begin() + 1; a != rects.end(); ++a) {
			area.x = std::min(area.x, a->x);
			area.y = std::min(area.y, a->y);
			area.w = std::max(area.x + area.w, a->x + a->w) - area.x;
			area.h = std::max(area.y + area.h, a->y + a->h) - area.y;
		}
		return area;
	}

	/// Create an rect_i object which contains all points in a list.
	template <typename T>
	inline i_rectangle<T> make_boundary(std::initializer_list<i_tuple2<T>> pts) {
		i_rectangle<T> r (pts.begin()->x, pts.begin()->y);
		for (auto p = pts.begin() + 1; p != pts.end(); ++p) {
			r.x = std::min(r.x, p->x);
			r.y = std::min(r.y, p->y);
			r.w = std::max(r.x + r.w, p->x) - r.x;
			r.h = std::max(r.y + r.h, p->y) - r.y;
		}
		return r;
	}

	/// Create an rect_i object using a position and radius.
	template <typename T>
	inline i_rectangle<T> rect_from_radius(i_tuple2<T> pos, T radius) { // TODO Create unit test
		return i_rectangle<T>{ pos.x - radius, pos.y - radius, radius * 2, radius * 2 };
	}


	/******************************************************************************************************************
		I_Map Operations
	******************************************************************************************************************/

	/// <summary>
	/// Copy all cells from the intersection area of one map into another. Can optionally specify a conversion function (defaults to static cast).
	/// </summary>
	template <typename Ta, typename Tb>
	inline void copy_into(const I_Map<Ta>& from, I_Map<Tb>& to, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		rect_i area = hrzn::intersect(from, to);
		H_FOREACH_POINT(area, x, y) {
			to.set(x, y, cast(from.at(x, y)));
		}
	}

	/// <summary>
	/// Create a duplicate map from an existing map with an optional conversion method.
	/// </summary>
	template <typename Ta, typename Tb>
	inline MapContainer<Ta> copy_each(const I_Map<Tb>& map, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		MapContainer<Ta> dup((rect_i)map);
		H_FOREACH_POINT(map, x, y) {
			dup.set(x, y, cast(map.at(x, y)));
		}
		return dup;
	}

	/// <summary>
	/// Fill a map in place with specified value.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="map">The map which is modified in-place.</param>
	/// <param name="fill_obj">The value with which to fill the map.</param>
	template <typename T>
	inline void fill(I_Map<T>& map, const T& fill_obj) {
		H_FOREACH_POINT(map, x, y) {
			map.set(x, y, fill_obj);
		}
	}

	/// <summary>
	/// Fill a map in place but only in the provided area.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="map">The map which is modified in-place.</param>
	/// <param name="area">The area within which the operation is performed.</param>
	/// <param name="fill_obj">The value with which to fill the map.</param>
	template <typename T>
	inline void fill(I_Map<T>& map, rect_i area, const T& fill_obj) {
		rect_i fill_area = hrzn::intersect(map, area);
		H_FOREACH_POINT(fill_area, x, y) {
			map.set(x, y, fill_obj);
		}
	}

	/// <summary>
	/// Fill the entire map in place utilizing a pure function.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="Tf"></typeparam>
	/// <param name="map"></param>
	/// <param name="fill_func"></param>
	template <typename T, typename Tf>
	inline void fill_each(I_Map<T>& map, Tf& fill_func) { // TODO fill function should be in the format of [T (*)(h_int, h_int)]
		H_FOREACH_POINT(map, x, y) {
			map.set(x, y, fill_func());
		}
	}

	/// <summary>
	/// Fill a map in place using another map of booleans as a mask. If the value for the corresponding mask position is <c>true</c>, the value at that position in the map is replaced with the provided value.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="map">The map which is modified in-place.</param>
	/// <param name="fill_obj">The value used to fill the map.</param>
	/// <param name="mask">A boolean map to be used as a mask.</param>
	template <typename T>
	inline void fill_mask(I_Map<T>& map, const T& fill_obj, const I_Map<bool>& mask) {
		rect_i fill_area = hrzn::intersect(map, mask);
		H_FOREACH_POINT(fill_area, x, y) {
			if (mask.at(x, y))
				map.set(x, y, fill_obj);
		}
	}

	/// <summary>
	/// Generate a boolean map based on whether each value in the map is equivelent to the provided value.
	/// </summary>
	/// <param name="map">The map reference to search.</param>
	/// <param name="i">The value with which to equal each cell in the map.</param>
	template <typename T>
	inline MapContainer<bool> select(const I_Map<T>& map, const T& i) {
		MapContainer<bool> mask((rect_i)map);
		H_FOREACH_POINT(map, x, y) {
			mask.set(x, y, map.at(x, y) == i);
		}
		return mask;
	}

	/// <summary>
	/// Starting from a point and moving in a driection, create a list of cell references of each point along the path.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="map"></param>
	/// <param name="first"></param>
	/// <param name="length"></param>
	/// <param name="direction"></param>
	/// <returns></returns>
	template <typename T>
	inline std::vector<cell_pointer<T>> project_from_point(I_Map<T>& map, point2 first, point2 direction, int length) { //TODO decide on a clearer definition and what to do for copy.
		std::vector<cell_pointer<T>> list;
		for (int i = 0; i < length; ++i) {
			point2 pt = first + (direction * i);
			if (map.contains(pt)) {
				cell_pointer<T> ptr(pt, map.at(pt));
				list.push_back(ptr);
			}
			else {
				return list;
			}
		}
		return list;
	}

	template <typename T>
	inline std::vector<T> transpose_map_to_list(const I_Map<T>& map) {
		std::vector<T> list;
		list.reserve(map.area());
		for (const auto& cell : map)
			list.emplace_back(cell);
		return list;
	}

	/// <summary>
	/// Create a map of the specified area and fill it from a list.
	/// </summary>
	template <typename T, typename TForwardIterator>
	inline MapContainer<T> transpose_list_to_map(rect_i area, TForwardIterator first, TForwardIterator last) {
		MapContainer<T> map(area);
		auto itr_map = map.begin();

		while (itr_map != map.end() && first != last) {
			*itr_map = *first;
			++itr_map;
			++first;
		}

		return map;
	}

	/// <summary>
	/// Create a map of the specified area and fill it from a list.
	/// </summary>
	template <typename T, typename TForwardIterator>
	inline MapContainer<T> transpose_list_to_map(h_unsigned width, h_unsigned height, TForwardIterator first, TForwardIterator last) {
		return transpose_list_to_map<T, TForwardIterator>(rect_i(width, height), first, last);
	}

	/// <summary>
	/// Return a new map that is transformed where each column is now a row and each row is now a column.
	/// </summary>
	/// <returns>A transformed copy of the map parameter.</returns>
	template<typename T>
	MapContainer<T> swizzle_map(const I_Map<T>& map) {
		MapContainer<T> rotated(hrzn::swizzle((rect_i)map));

		H_FOREACH_POINT(map, x, y) {
			rotated.set(y, x, map.at(x, y));
		}

		return rotated;
	}


	template<typename T>
	MapContainer<T> rotate_map(const I_Map<T>& map, int turns) {
		turns = ((turns % 4) + 4) % 4;

		MapContainer<T> rotated;
		switch (turns) {
		case(1):
			rotated = hrzn::swizzle_map(map);
			rotated.flip_x();
			return rotated;
		case(3):
			rotated = hrzn::swizzle_map(map);
			rotated.flip_y();
			return rotated;
		case(2):
			rotated = hrzn::copy(map);
			rotated.reverse();
			return rotated;
		default:
			return hrzn::copy(map);
		}
	}

	/******************************************************************************************************************
		I_Map generation algorithms
	******************************************************************************************************************/

	template <typename T>
	inline void scatter(I_Map<T>& map, T val, double threshold) {
		H_FOREACH_POINT(map, x, y) {
			if (((double)std::rand() / (double)RAND_MAX) > threshold)
				map.set(x, y, val);
		}
	}

	template <typename T>
	inline void flood_fill(point2 first, I_Map<T>& region, I_Map<bool>& result, bool edge = false, bool use8 = false) {
		rect_i area = hrzn::intersect(region, result);
		result.set(first, true);

		for (int i = 0; i < (use8 ? 8 : 4); ++i) {
			point2 pos = first + (use8 ? h_neighborhood8[i] : h_neighborhood4[i]);
			if (area.contains(pos) && !result.at(pos)) {
				if (region.at(pos) == region.at(first))
					hrzn::flood_fill(pos, region, result, edge, use8);
				else if (edge)
					result.set(pos, true);
			}
		}

	}

	inline void cellular_automata(I_Map<bool>* mask, int birth_rate, bool wrap_position) {
		MapContainer<int> neighbor_counts((rect_i)(*mask), 0);
		for (h_int y = mask->y; y < mask->last().y; ++y)
			for (h_int x = mask->x; x < mask->last().x; ++x) {
				point2 cell = { x, y };
				int n_count = 0;
				for (const auto& dir : h_neighborhood8) {
					point2 pos;
					if (wrap_position)
						pos = hrzn::wrap_point(cell + dir, *mask);
					else
						pos = hrzn::clamp_point(cell + dir, *mask);

					if (mask->at(pos))
						++n_count;
				}
				neighbor_counts.set((point2)cell, n_count);
			}
		for (h_int y = mask->y; y < mask->y + mask->h; ++y)
			for (h_int x = mask->x; x < mask->x + mask->w; ++x)
				mask->set(x, y, neighbor_counts.at(x, y) >= birth_rate);
	}



	/******************************************************************************************************************
		Transformation and Polygon Methods
	******************************************************************************************************************/


	/*inline hBox bounding_box(const i_polygon& polygon) {
		vector2 min = polygon.get(0);
		vector2 max = polygon.get(0);

		for (int i = 1; i < polygon.count(); ++i) {
			min.x = std::min(min.x, polygon.get(i).x);
			min.y = std::min(min.y, polygon.get(i).y);
			max.x = std::max(max.x, polygon.get(i).x);
			max.y = std::max(max.y, polygon.get(i).y);
		}

		return { min, max };
	}*/


	/******************************************************************************************************************
		Lerping Methods
	******************************************************************************************************************/

	namespace lerp {
		inline h_float lerp(h_float a, h_float b, h_float f) {
			return (a * (1._hf - f)) + (b * f);
		}

		inline vector2 lerp(vector2 a, vector2 b, h_float f) {
			return { lerp(a.x, b.x, f), lerp(a.y, b.y, f) };
		}

		inline angle lerp(angle a, angle b, h_float f) {
			h_float t_a = a.tau;
			h_float t_b = b.tau;
			if (std::abs(t_a - t_b) >= 1._hf) {
				if (t_a > t_b)
					t_a = t_a - 1._hf;
				else
					t_b = t_b - 1._hf;
			}
			return {lerp(t_a, t_b, f)};
		}

		inline transform lerp(transform a, transform b, h_float f) {
			return { lerp(a.position, b.position, f), lerp(a.rotation, b.rotation, f), lerp(a.scale, b.scale, f) };
		}

		inline float linear_clamp(h_float t) { // TODO linear_clamp(): is this redundant?
			return std::clamp(t, 0._hf, 1._hf);
		}

		inline float repeat(h_float t) {
			return std::fmod(t, 1._hf);
		}

		inline float pingpong(h_float t) {
			return 1._hf - std::abs(2._hf * std::fmod(t * 0.5_hf, 1._hf) - 1._hf);
		}

		inline float bounce(h_float t) {
			return 1._hf - std::abs(2._hf * t - 1._hf);
		}

		inline float smoothstep(h_float t) {
			return t * t * (3._hf - 2._hf * t);
		}

		inline float smootherstep(h_float t) {
			return t * t * t * (t * (6._hf * t - 15._hf) + 10._hf);
		}

	} // namespace lerp

} // namespace hrzn
