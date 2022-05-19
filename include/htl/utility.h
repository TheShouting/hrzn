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

#include <vector>

namespace hrzn {

	/******************************************************************************************************************
		Transform Operations
	******************************************************************************************************************/

	inline hPoint clampPoint(const hPoint& p, const hArea& area) {
		hType_i x = std::min(std::max(p.x, area.x1), area.x2 - 1_hi);
		hType_i y = std::min(std::max(p.y, area.y1), area.y2 - 1_hi);
		return { x, y };
	}

	inline hPoint wrapPoint(const hPoint& p, const hArea& area) {
		hType_i x = (p.x % area.width() + area.width()) % area.width();
		hType_i y = (p.y % area.height() + area.height()) % area.height();
		return { x, y };
	}

	inline hType_f dotProduct(const hVector& a, const hVector& b) {
		return a.x * b.x + a.y * b.y;
	}

	inline hType_f distance(const hVector& a, const hVector& b) {
		return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	inline hType_f distanceSqr(const hVector& a, const hVector& b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}

	inline hType_f distanceManhattan(const hVector& a, const hVector& b) {
		return std::abs(a.x - b.x) + std::abs(a.y - b.y);
	}

	inline hVector getNormalized(hVector const& vec) {
		double l = vec.length();
		if (l < H_EPSILON) {
			return hVector();
		}
		double il = 1.0f / l;
		return hVector(vec.x * il, vec.y * il);
	}

	inline hRotation findRotation(hVector v) {
		return hRotation::Radians(std::atan2(v.y, v.x));
	}

	inline hPoint roundToPoint(const hVector& vec) {
		return hPoint(std::round(vec.x), std::round(vec.y));
	}


	/******************************************************************************************************************
		hArea Operations
	******************************************************************************************************************/

	inline bool overlapping(const hArea& a, const hArea& b) {
		return !(a.x1 > b.x2 || a.y1 > b.y2 || b.x1 > a.x2 || b.y1 > a.y2);
	}

	inline bool contains(const hArea& a, const hArea& b) {
		return b.x1 >= a.x1 && b.y1 >= a.y1 && b.x2 <= a.x2 && b.y2 <= a.x2;
	}

	inline bool contains(const hArea& a, const hPoint& b) {
		return b.x >= a.x1 && b.y >= a.y1 && b.x < a.x2&& b.y < a.x2;
	}

	inline bool isEdgePoint(const hArea& rec, const hPoint& pos) {
		return pos.x == rec.x1 || pos.x == rec.x2 - 1_hi || pos.y == rec.y1 || pos.y == rec.y2 - 1_hi;
	}

	/// Create an Area object from the extreme points of two areas.
	inline hArea makeAreaBoundary(const hArea& a, const hArea& b) {
		return hArea(std::min(a.x1, b.x1), std::min(a.y1, b.y1), std::max(a.x2, b.x2), std::max(a.y2, b.y2));
	}

	/// Create an hArea object based on the boundary from a list of points.
	inline hArea makeAreaBoundary(const std::initializer_list<hPoint> pts) {
		hArea r (pts.begin()->x, pts.begin()->y);
		for (auto p = pts.begin() + 1; p != pts.end(); ++p) {
			r.x1 = std::min(r.x1, p->x);
			r.y1 = std::min(r.y1, p->y);
			r.x2 = std::max(r.x2, p->x);
			r.y2 = std::max(r.y2, p->y);
		}
		return r;
	}

	/// Create an hArea object ussing a position and radius.
	inline hArea makeAreaRadius(const hPoint pos, const hType_i radius) {
		return hArea(pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius);
	}

	/// Create an hArea object using offset, size.
	inline hArea makeAreaOffsetAndSize(const hPoint offset, const hPoint size) {
		return hArea(offset.x, offset.y, offset.x + size.x, offset.y + size.y);
	}



	/// <summary>
	/// Split the area of a <type>URect</type> along its longest axis into two smaller ones.
	/// </summary>
	/// <param name="rec">The <type>Urect</type> area to be split</param>
	/// <returns>A container with the new URect objects. If <paramref name="rec"/> is only a single cell (width and height are equal to 1), then both are simply a copy of the orignal parameter. </returns>
	inline std::pair<hArea, hArea> split(const hArea& area) {
		hArea a1 = area;
		hArea a2 = area;
		hPoint cp = area.center();
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
		Matrix Operations
	******************************************************************************************************************/

	/// <summary>
	/// Compare the intersecting area of two maps. Returns true of all equivelantly positioned cells area the same.
	/// </summary>
	template <typename T>
	inline bool compare(const IMap<T>& a, const IMap<T>& b) {
		hArea area = hrzn::intersect(a, b);
		HRZN_FOREACH_POINT(area, x, y) {
			if (a.at(x, y) != b.at(x, y))
				return false;
		}
		return true;
	}

	/// <summary>
	/// Copy all cells from the intersection area of both maps. Can optionally specify a conversion function.
	/// </summary>
	template <typename Ta, typename Tb>
	inline void copy(const IMap<Ta>& from, IMap<Tb>& to, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		hArea area = hrzn::intersect(from, to);
		HRZN_FOREACH_POINT(area, x, y) {
			to.set(x, y, cast(from.at(x, y)));
		}
	}

	
	template <typename Ta, typename Tb>
	inline HMap<Ta> duplicate(const IMap<Tb>& map, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		HMap<Ta> dup(map);
		HRZN_FOREACH_POINT(map, x, y) {
			dup.set(x, y, cast(map.at(x, y)));
		}
		return dup;
	}

	template <typename T>
	inline void fill(IMap<T>& map, const T& fill_obj) {
		HRZN_FOREACH_POINT(map, x, y) {
			map.set(x, y, fill_obj);
		}
	}

	template <typename T>
	inline void fill(IMap<T>& map, const hArea area, const T& fill_obj) {
		hArea fill_area = hrzn::intersect(map, area);
		HRZN_FOREACH_POINT(fill_area, x, y) {
			map.set(x, y, fill_obj);
		}
	}

	template <typename T, typename Tf>
	inline void fill_each(IMap<T>& map, Tf& fill_func) {
		HRZN_FOREACH_POINT(map, x, y) {
			map.set(x, y, fill_func());
		}
	}

	template <typename T>
	inline void maskfill(IMap<T>& map, const T& fill_obj, const IMap<bool>& mask) {
		hArea fill_area = hrzn::intersect(map, mask);
		HRZN_FOREACH_POINT(fill_area, x, y) {
			if (mask.at(x, y))
				map.set(x, y, fill_obj);
		}
	}

	template <typename T>
	inline HMap<bool> select(const IMap<T>& map, const T& i) {
		HMap<bool> mask((hArea)map, false);
		HRZN_FOREACH_POINT(map, x, y) {
			mask.set(x, y, map.at(x, y) == i);
		}
		return mask;
	}

	template <typename T>
	inline std::vector<T> projectFromPoint(const IMap<T>& map, hPoint first, int length, hPoint direction) {
		std::vector<T> list;
		for (int i = 0; i < length; ++i) {
			hPoint pt = first + (direction * i);
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
	inline HMap<T> transposeListToMap(hArea area, T* list) {
		HMap<T> map(area);

		for (int x = map.x1; x < map.x2; ++x) {
			for (int y = map.y1; y < map.y2; ++y)
				map.set(x, y, *list);
			list++;
		}
		return map;
	}

	template <typename T>
	inline std::vector<T> transposeMapToList(const IMap<T>& map) {
		std::vector<T> list;
		list.reserve(map.area());
		int i = 0;
		for (int y = map.y1; y < map.y2; ++y)
			for (int x = map.x1; x < map.x2; ++x)
				list.emplace_back(map.at(x, y));
		return list;
	}

	template <typename T>
	inline HMap<T> transposeListToMap(hType_u width, hType_u height, T* list) {
		return transposeListToMap(hArea(width, height), list);
	}



	/******************************************************************************************************************
		Map generation algorithms
	******************************************************************************************************************/

	template <typename T>
	inline void scatter(IMap<T>& map, T val, double threshold) {
		HRZN_FOREACH_POINT(map, x, y) {
			if (((double)std::rand() / (double)RAND_MAX) > threshold)
				map.set(x, y, val);
		}
	}

	template <typename T>
	inline void floodFill(hPoint first, IMap<T>& region, IMap<bool>& result, bool edge = false, bool use8 = false) {
		hArea area = hrzn::intersect(region, result);
		result.set(first, true);

		for (int i = 0; i < (use8 ? 8 : 4); ++i) {
			hPoint pos = first + (use8 ? h_neighborhood8 : h_neighborhood4)[i];
			if (area.contains(pos) && !result.at(pos)) {
				if (region.at(pos) == region.at(first))
					hrzn::floodFill(pos, region, result, edge, use8);
				else if (edge)
					result.set(pos, true);
			}
		}

	}

	inline void cellularAutomata(IMap<bool>* mask, int birth_rate, bool wrap_position) {
		HMap<int> neighbor_counts((hArea)(*mask), 0);
		for (hType_i y = mask->y1; y < mask->y2; ++y)
			for (hType_i x = mask->x1; x < mask->x2; ++x) {
				hPoint cell = { x, y };
				int n_count = 0;
				for (const auto& dir : h_neighborhood8) {
					hPoint pos;
					if (wrap_position)
						pos = hrzn::wrapPoint(cell + dir, *mask);
					else
						pos = hrzn::clampPoint(cell + dir, *mask);

					if (mask->at(pos))
						++n_count;
				}
				neighbor_counts.set((hPoint)cell, n_count);
			}
		for (hType_i y = mask->y1; y < mask->y2; ++y)
			for (hType_i x = mask->x1; x < mask->x2; ++x)
				mask->set(x, y, neighbor_counts.at(x, y) >= birth_rate);
	}



	/******************************************************************************************************************
		Transformation and Polygon Methods
	******************************************************************************************************************/


	inline hBox boundingBox(const IPolygon& polygon) {
		hVector min = polygon.get(0);
		hVector max = polygon.get(0);

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
		inline hType_f lerp(const hType_f& a, const hType_f& b, const hType_f& f) {
			return (a * (1._hf - f)) + (b * f);
		}

		inline hVector lerp(const hVector& a, const hVector& b, const hType_f& f) {
			return { lerp(a.x, b.x, f), lerp(a.y, b.y, f) };
		}

		inline hRotation lerp(const hRotation& a, const hRotation& b, const hType_f& f) {
			hType_f t_a = a.tau;
			hType_f t_b = b.tau;
			if (std::abs(t_a - t_b) >= 1._hf) {
				if (t_a > t_b)
					t_a = t_a - 1._hf;
				else
					t_b = t_b - 1._hf;
			}
			return {lerp(t_a, t_b, f)};
		}

		inline hTransform lerp(const hTransform& a, const hTransform& b, const hType_f& f) {
			return { lerp(a.position, b.position, f), lerp(a.rotation, b.rotation, f), lerp(a.scale, b.scale, f) };
		}

		template <typename T>
		inline T smoothstep(const T& a, const T& b, const hType_f& f) {
			return lerp(a, b, f * f * (3._hf - 2._hf * f));
		}

		template <typename T>
		inline T smootherstep(const T& a, const T& b, const hType_f& f) {
			return lerp(a, b, f * f * f * (f * (6._hf * f - 15._hf) + 10._hf));
		}

	} // namespace lerp

} // namespace hrzn
