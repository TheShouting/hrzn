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


	const ITuple<hType_i> neighborhood4[] = {
		{0, -1},
		{1, 0},
		{0, 1},
		{-1, 0},
		{0, 0}
	};

	const ITuple<hType_i> neighborhood8[] = {
		{0, -1},
		{1, -1},
		{1, 0},
		{1, 1},
		{0, 1},
		{-1, 1},
		{-1, 0},
		{-1, -1},
		{0, 0}
	};

	/******************************************************************************************************************
		Transform Operations
	******************************************************************************************************************/

	inline hPoint wrapPoint(const hPoint& p, const hArea& area) {
		hType_i x = std::min(std::max(p.x, area.x1), area.x2);
		hType_i y = std::min(std::max(p.y, area.y1), area.y2);
		return { x, y };
	}

	inline hPoint clampPoint(const hPoint& p, const hArea& area) {
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

	inline hVector round(const hVector& vec) {
		return { std::round(vec.x), std::round(vec.y) };
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
		return b.x >= a.x1 && b.y >= a.y1 && b.x <= a.x2 && b.y <= a.x2;
	}

	inline bool isEdgePoint(const hArea& rec, const hPoint& pos) {
		return pos.x == rec.x1 || pos.x == rec.x2 || pos.y == rec.y1 || pos.y == rec.y2;
	}

	inline hArea intersect(const hArea& a, const hArea& b) {
		return hArea(std::max(a.x1, b.x1), std::max(a.y1, b.y1), std::min(a.x2, b.x2), std::min(a.y2, b.y2));
	}

	inline hArea makeBoundary(const hArea& a, const hArea& b) {
		return hArea(std::min(a.x1, b.x1), std::min(a.y1, b.y1), std::max(a.x2, b.x2), std::max(a.y2, b.y2));
	}

	inline hArea makeBoundary(const std::initializer_list<hPoint> list) {
		hArea a;
		return a;
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
			a2.y1 = cp.y + 1;
		}
		else if (area.width() > 1) {
			a1.x2 = cp.x;
			a2.x1 = cp.x + 1;
		}
		return std::make_pair(a1, a2);
	}

	/******************************************************************************************************************
		Matrix Operations
	******************************************************************************************************************/
	
	template <typename T>
	inline bool compare(const IMap<T> & a, const IMap<T> & b) {
		hArea area = hrzn::intersect(a, b);
		for (int y = area.y1; y <= area.y2; ++y)
			for (int x = area.x1; x <= area.x2; ++x)
				if (a.at(x, y) != b.at(x, y))
					return false;
		return true;
	}

	template <typename T>
	inline void transfer(IMap<T>* to, const IMap<T>& from) {
		hArea area = hrzn::intersect(*to, from);
		for (int y = area.y1; y <= area.y2; ++y)
			for (int x = area.x1; x <= area.x2; ++x)
				to->set(x, y, from.at(x, y));
	}

	template <typename Ta, typename Tb>
	inline HMap<Ta> copy(const IMap<Tb>& mat, Ta(*cast)(Tb) = [](Tb val)->Ta {return static_cast<Ta>(val); }) {
		HMap<Ta> dup(mat);
		for (int y = mat.y1; y <= mat.y2; ++y)
			for (int x = mat.x1; x <= mat.x2; ++x)
				dup.set(x, y, cast(mat.at(x, y)));
		return dup;
	}

	template <typename T>
	inline void fill(IMap<T>* mat, const hArea area, const T& fill_obj) {
		hArea fill_area = hrzn::intersect(*mat, area);
		for (int y = fill_area.y1; y <= fill_area.y2; ++y)
			for (int x = fill_area.x1; x <= fill_area.x2; ++x)
				mat->set(x, y, fill_obj);
	}

	template <typename T, typename Tf>
	inline void fill_each(IMap<T>* mat, Tf& fill_func) {
		for (int y = mat->y1; y <= mat->y2; ++y)
			for (int x = mat->x1; x <= mat->x2; ++x)
				mat->set(x, y, fill_func());
	}

	template <typename T>
	inline void maskfill(IMap<T>* mat, const IMap<bool>& mask, const T& fill_obj) {
		hArea fill_area = hrzn::intersect(*mat, mask);
		for (int y = fill_area.y1; y <= fill_area.y2; ++y)
			for (int x = fill_area.x1; x <= fill_area.x2; ++x)
				if (mask.at(x, y)) 
					mat.set(x, y, fill_obj);
	}

	template <typename T>
	inline void floodFillArea(hPoint start, IMap<T>* region, IMap<bool>* result, bool edge = false, bool use8 = false) {
		hArea area = hrzn::intersect(*region, *result);
		result->set(start, true);

		for (int i = 0; i < (use8 ? 8 : 4); ++i) {
			hPoint pos = start + (use8 ? neighborhood8 : neighborhood4)[i];
			if (area.contains(pos) && !result->at(pos)) {
				if (region->at(pos) == region->at(start))
					hrzn::floodFillArea(pos, region, result, edge, use8);
				else if (edge)
					result->set(pos.x, true);
			}
		}

	}

	inline void cellularAutomata(IMap<bool>* mask, int birth_rate, bool wrap_position) {
		HMap<int> neighbor_counts((hArea)(*mask), 0);
		for (auto cell : mask->iterable()) {
			int n_count = 0;
			for (const auto& dir : neighborhood8) {
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
		for (auto cell : mask->iterable())
			mask->set((hPoint)cell, neighbor_counts.at(cell) >= birth_rate);
	}

	template <typename T>
	inline void scatter(IMap<T>* mat, T val, double threshold) {
		for (auto i : mat->iterable()) {
			if (((double)std::rand() / (double)RAND_MAX) > threshold)
				mat->set((hPoint)i, val);
		}
	}

	template <typename T>
	inline HMask select(const IMap<T>& map, const T& i) {
		HMask mask((hArea)map);
		for (auto p : map.iterable())
			mask.set(p, map.at(p) == i);
		return mask;
	}

	//HMask select(const IMap<T>& map, bool(*f)(const T &)) {
	template <typename T, typename Tf>
	inline HMask select(const IMap<T>& map, Tf & f) {
		HMask mask((hArea)map);
		for (auto p : map.iterable())
			mask.set(p, f(map.at(p)));
		return mask;
	}

	template <typename T>
	inline std::vector<T> projectFromPoint(const IMap<T>& map, hPoint start, int length, hPoint direction) {
		std::vector<T> list;
		for (int i = 0; i < length; ++i) {
			hPoint pt = start + (direction * i);
			if (map.contains(pt)) {
				list.push_back(map.at(pt));
			}
			else {
				return list;
			}
		}
		return list;
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
