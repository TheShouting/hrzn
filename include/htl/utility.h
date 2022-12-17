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

#include "hrzn.h"
#include "geometry.h"
#include "containers.h"

#define HRZN_FOREACH_POINT(A, X, Y) for (h_int X, Y = A.y1; Y < A.y2; ++Y) for (X = A.x1; X < A.x2; ++X) 

namespace hrzn {

	/******************************************************************************************************************
		Basic Type Operations
	******************************************************************************************************************/

	inline h_float dotProduct(const vector2& a, const vector2& b) {
		return a.x * b.x + a.y * b.y;
	}

	inline h_float distance(const vector2& a, const vector2& b) {
		return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	inline h_float distanceSqr(const vector2& a, const vector2& b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}

	inline h_float distanceManhattan(const vector2& a, const vector2& b) {
		return std::abs(a.x - b.x) + std::abs(a.y - b.y);
	}

	inline vector2 getNormalized(vector2 const& vec) {
		double l = vec.length();
		if (l < EPSILON) {
			return vector2();
		}
		double il = 1.0f / l;
		return vector2(vec.x * il, vec.y * il);
	}

	inline angle findRotation(vector2 v) {
		return angle::Radians(std::atan2(v.y, v.x));
	}

	inline point2 roundToPoint(const vector2& vec) {
		return point2(std::round(vec.x), std::round(vec.y));
	}


	/// Create a contiguous point_area based on the extreme corners of a point_area list.
	inline point_area make_boundary(const std::initializer_list<point_area>& areas) {
		point_area area = *areas.begin();
		for (auto a = areas.begin() + 1; a != areas.end(); ++a) {
			area.x1 = std::min(area.x1, a->x1);
			area.y1 = std::min(area.y1, a->y1);
			area.x2 = std::max(area.x2, a->x2);
			area.y2 = std::max(area.y2, a->y2);
		}
		return area;
	}


	/// Create an point_area object which contains all points in a list.
	inline point_area make_boundary(const std::initializer_list<point2> & pts) {
		point_area r (pts.begin()->x, pts.begin()->y);
		for (auto p = pts.begin() + 1; p != pts.end(); ++p) {
			r.x1 = std::min(r.x1, p->x);
			r.y1 = std::min(r.y1, p->y);
			r.x2 = std::max(r.x2, p->x);
			r.y2 = std::max(r.y2, p->y);
		}
		return r;
	}

	/// Create an point_area object ussing a position and radius.
	inline point_area makeAreaRadius(const point2 pos, const h_int radius) {
		return point_area(pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius);
	}


	/// <summary>
	/// Split the area of a <type>point_area</type> along its longest axis into two smaller ones.
	/// </summary>
	/// <param name="area">The <type>point_area</type> to be split</param>
	/// <returns>A container with the new URect objects. If <paramref name="rec"/> is only a single cell (width and height are equal to 1), then both are simply a copy of the orignal parameter. </returns>
	inline std::pair<point_area, point_area> split(const point_area& area) {
		point_area a1 = area;
		point_area a2 = area;
		point2 cp = area.center();
		if (area.height() > 1 && area.height() > area.width()) {
			a1.y2 = cp.y;
			a2.y1 = cp.y;
		}
		else if (area.width() > 1) {
			a1.x2 = cp.x;
			a2.x1 = cp.x;
		}
		return std::make_pair(a1, a2);
	}

	/******************************************************************************************************************
		Map Operations
	******************************************************************************************************************/


	/// <summary>
	/// Copy all cells from the intersection area of one map into another. Can optionally specify a conversion function (defaults to static cast).
	/// </summary>
	template <typename Ta, typename Tb>
	inline void copy_into(const Map<Ta>& from, Map<Tb>& to, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		point_area area = hrzn::intersect(from, to);
		HRZN_FOREACH_POINT(area, x, y) {
			to.set(x, y, cast(from.at(x, y)));
		}
	}

	/// <summary>
	/// Create a duplicate map from an existing map with an optional conversion method.
	/// </summary>
	template <typename Ta, typename Tb>
	inline MapContainer<Ta> copy_each(const Map<Tb>& map, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		MapContainer<Ta> dup((point_area)map);
		HRZN_FOREACH_POINT(map, x, y) {
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
	inline void fill(Map<T>& map, const T& fill_obj) {
		HRZN_FOREACH_POINT(map, x, y) {
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
	inline void fill(Map<T>& map, const point_area area, const T& fill_obj) {
		point_area fill_area = hrzn::intersect(map, area);
		HRZN_FOREACH_POINT(fill_area, x, y) {
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
	inline void fill_each(Map<T>& map, Tf& fill_func) {
		HRZN_FOREACH_POINT(map, x, y) {
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
	inline void fill_mask(Map<T>& map, const T& fill_obj, const Map<bool>& mask) {
		point_area fill_area = hrzn::intersect(map, mask);
		HRZN_FOREACH_POINT(fill_area, x, y) {
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
	inline MapContainer<bool> select(const Map<T>& map, const T& i) {
		MapContainer<bool> mask((point_area)map);
		HRZN_FOREACH_POINT(map, x, y) {
			mask.set(x, y, map.at(x, y) == i);
		}
		return mask;
	}

	template <typename T>
	inline std::vector<T> projectFromPoint(const Map<T>& map, point2 first, int length, point2 direction) {
		std::vector<T> list;
		for (int i = 0; i < length; ++i) {
			point2 pt = first + (direction * i);
			if (map.contains(pt)) {
				list.push_back(map.at(pt));
			}
			else {
				return list;
			}
		}
		return list;
	}

	template <typename T>
	inline std::vector<T> transposeMapToList(const Map<T>& map) {
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
	inline MapContainer<T> transposeListToMap(point_area area, TForwardIterator first, TForwardIterator last) {
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
	inline MapContainer<T> transposeListToMap(h_unsigned width, h_unsigned height, TForwardIterator first, TForwardIterator last) {
		return transposeListToMap<T, TForwardIterator>(point_area(width, height), first, last);
	}


	template<typename T>
	MapContainer<T> swizzle_map(const Map<T>& map) {
		MapContainer<T> rotated(hrzn::swizzle((point_area)map));

		HRZN_FOREACH_POINT(map, x, y) {
			rotated.set(y, x, map.at(x, y));
		}

		return rotated;
	}


	template<typename T>
	MapContainer<T> rotate_map(const Map<T>& map, int turns) {
		turns = ((turns % 4) + 4) % 4;

		MapContainer<T> rotated;
		switch (turns) {
		case(1):
			rotated = hrzn::swizzle_map(map);
			rotated.flipX();
			return rotated;
		case(3):
			rotated = hrzn::swizzle_map(map);
			rotated.flipY();
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
		Map generation algorithms
	******************************************************************************************************************/

	template <typename T>
	inline void scatter(Map<T>& map, T val, double threshold) {
		HRZN_FOREACH_POINT(map, x, y) {
			if (((double)std::rand() / (double)RAND_MAX) > threshold)
				map.set(x, y, val);
		}
	}

	template <typename T>
	inline void floodFill(point2 first, Map<T>& region, Map<bool>& result, bool edge = false, bool use8 = false) {
		point_area area = hrzn::intersect(region, result);
		result.set(first, true);

		for (int i = 0; i < (use8 ? 8 : 4); ++i) {
			point2 pos = first + (use8 ? h_neighborhood8 : h_neighborhood4)[i];
			if (area.contains(pos) && !result.at(pos)) {
				if (region.at(pos) == region.at(first))
					hrzn::floodFill(pos, region, result, edge, use8);
				else if (edge)
					result.set(pos, true);
			}
		}

	}

	inline void cellularAutomata(Map<bool>* mask, int birth_rate, bool wrap_position) {
		MapContainer<int> neighbor_counts((point_area)(*mask), 0);
		for (h_int y = mask->y1; y < mask->y2; ++y)
			for (h_int x = mask->x1; x < mask->x2; ++x) {
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
		for (h_int y = mask->y1; y < mask->y2; ++y)
			for (h_int x = mask->x1; x < mask->x2; ++x)
				mask->set(x, y, neighbor_counts.at(x, y) >= birth_rate);
	}



	/******************************************************************************************************************
		Transformation and Polygon Methods
	******************************************************************************************************************/


	inline hBox boundingBox(const IPolygon& polygon) {
		vector2 min = polygon.get(0);
		vector2 max = polygon.get(0);

		for (int i = 1; i < polygon.count(); ++i) {
			min.x = std::min(min.x, polygon.get(i).x);
			min.y = std::min(min.y, polygon.get(i).y);
			max.x = std::max(max.x, polygon.get(i).x);
			max.y = std::max(max.y, polygon.get(i).y);
		}

		return { min, max };
	}


	/******************************************************************************************************************
		Lerping Methods
	******************************************************************************************************************/

	namespace lerp {
		inline h_float lerp(const h_float& a, const h_float& b, const h_float& f) {
			return (a * (1._hf - f)) + (b * f);
		}

		inline vector2 lerp(const vector2& a, const vector2& b, const h_float& f) {
			return { lerp(a.x, b.x, f), lerp(a.y, b.y, f) };
		}

		inline angle lerp(const angle& a, const angle& b, const h_float& f) {
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

		inline transform lerp(const transform& a, const transform& b, const h_float& f) {
			return { lerp(a.position, b.position, f), lerp(a.rotation, b.rotation, f), lerp(a.scale, b.scale, f) };
		}

		template <typename T>
		inline T smoothstep(const T& a, const T& b, const h_float& f) {
			return lerp(a, b, f * f * (3._hf - 2._hf * f));
		}

		template <typename T>
		inline T smootherstep(const T& a, const T& b, const h_float& f) {
			return lerp(a, b, f * f * f * (f * (6._hf * f - 15._hf) + 10._hf));
		}

	} // namespace lerp

} // namespace hrzn
