#pragma once

#include "hrzn.h"
#include <vector>

namespace hrzn {

	struct IPolygon {

		IPolygon() {}
		virtual ~IPolygon() {}

		virtual std::size_t count() const = 0;
		virtual vector2 get(std::size_t index) const = 0;

		vector2 operator[](std::size_t index) const { return get(index); }

		std::vector<vector2> list() const {
			std::vector<vector2> list;
			for (int i = 0; i < count(); ++i)
				list.emplace_back(get(i));
			return list;
		}

		vector2 center() const {
			vector2 avg;
			for (int i = 0; i < count(); ++i)
				avg += get(i);
			return avg / static_cast<h_float>(count());
		}

		h_float perimeter() const {
			h_float length = 0._hf;
			for (int i = 0; i < count() - 1; ++i)
				length += (get(i) - get(i + 1)).length();
			return length;
		}

		h_float perimeter_closed() const {
			h_float length = 0._hf;
			for (int i = 0; i < count() - 1; ++i)
				length += (get(i) - get(i + 1)).length();
			length += (get(0) - get(count() - 1)).length();
			return length;
		}

	}; // struct IPolygon


	struct hPolygon : public IPolygon {
		std::vector<vector2> vertices;

		hPolygon() {}
		hPolygon(std::initializer_list<vector2> verts) : vertices(verts) {}
		hPolygon(const IPolygon& polygon) : vertices(polygon.list()) {}

		std::size_t count() const override { return vertices.size(); }
		vector2 get(std::size_t index) const { return vertices[index]; }
	}; // struct hPolygon


	template <unsigned int N>
	struct IPolygonN : public IPolygon {
		std::size_t count() const override { return N; }
	}; // struct IPolygonN<int>


	struct hBox : public IPolygonN<4> {

		vector2 v1;
		vector2 v2;

		hBox() : v1(0._hf), v2(1._hf) {}
		hBox(vector2 size) : v1(0._hf), v2(size) {}
		hBox(vector2 v1, vector2 v2) : v1(v1), v2(v2) {}
		hBox(h_float w, h_float h) : v1(0._hf), v2(w, h) {}
		hBox(h_float ax, h_float ay, h_float bx, h_float by) : v1(ax, ay), v2(bx, by) {}
		hBox(point_area area) : v1(area.x1, area.y1), v2(area.x2, area.y2) {}

		vector2 get(std::size_t index) const override {
			return { (&v1)[h_corner[index].x].x, (&v1)[h_corner[index].y].y };
		}

	}; // struct hBox


	struct hQuad : public transform, IPolygonN<4> {

		hQuad() : transform() {}
		hQuad(h_float w, h_float h) : transform({ 0,0 }, 0.f, { w, h }) {}
		hQuad(vector2 dim) : transform() {}
		hQuad(transform tform) : transform(tform) {}

		vector2 get(std::size_t index) const override {
			return position + rotation.rotate(scale * vector2(h_quad[index].x - 0.5_hf, h_quad[index].y - 0.5_hf));
		}

	}; // struct hQuad

} // namespace hrzn