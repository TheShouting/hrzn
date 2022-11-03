#pragma once

#include "hrzn.h"
#include <vector>

namespace hrzn {

	struct IPolygon {

		IPolygon() {}
		virtual ~IPolygon() {}

		virtual std::size_t count() const = 0;
		virtual hVector get(std::size_t index) const = 0;

		hVector operator[](std::size_t index) const { return get(index); }

		std::vector<hVector> list() const {
			std::vector<hVector> list;
			for (int i = 0; i < count(); ++i)
				list.emplace_back(get(i));
			return list;
		}

		hVector center() const {
			hVector avg;
			for (int i = 0; i < count(); ++i)
				avg += get(i);
			return avg / static_cast<hType_f>(count());
		}

		hType_f perimeter() const {
			hType_f length = 0._hf;
			for (int i = 0; i < count() - 1; ++i)
				length += (get(i) - get(i + 1)).length();
			return length;
		}

		hType_f perimeter_closed() const {
			hType_f length = 0._hf;
			for (int i = 0; i < count() - 1; ++i)
				length += (get(i) - get(i + 1)).length();
			length += (get(0) - get(count() - 1)).length();
			return length;
		}

	}; // struct IPolygon


	struct hPolygon : public IPolygon {
		std::vector<hVector> vertices;

		hPolygon() {}
		hPolygon(std::initializer_list<hVector> verts) : vertices(verts) {}
		hPolygon(const IPolygon& polygon) : vertices(polygon.list()) {}

		std::size_t count() const override { return vertices.size(); }
		hVector get(std::size_t index) const { return vertices[index]; }
	}; // struct hPolygon


	template <unsigned int N>
	struct IPolygonN : public IPolygon {
		std::size_t count() const override { return N; }
	}; // struct IPolygonN<int>


	struct hBox : public IPolygonN<4> {

		hVector v1;
		hVector v2;

		hBox() : v1(0._hf), v2(1._hf) {}
		hBox(hVector size) : v1(0._hf), v2(size) {}
		hBox(hVector v1, hVector v2) : v1(v1), v2(v2) {}
		hBox(hType_f w, hType_f h) : v1(0._hf), v2(w, h) {}
		hBox(hType_f ax, hType_f ay, hType_f bx, hType_f by) : v1(ax, ay), v2(bx, by) {}
		hBox(hArea area) : v1(area.x1, area.y1), v2(area.x2, area.y2) {}

		hVector get(std::size_t index) const override {
			return { (&v1)[h_corner[index].x].x, (&v1)[h_corner[index].y].y };
		}

	}; // struct hBox


	struct hQuad : public hTransform, IPolygonN<4> {

		hQuad() : hTransform() {}
		hQuad(hType_f w, hType_f h) : hTransform({ 0,0 }, 0.f, { w, h }) {}
		hQuad(hVector dim) : hTransform() {}
		hQuad(hTransform tform) : hTransform(tform) {}

		hVector get(std::size_t index) const override {
			return position + rotation.rotate(scale * hVector(h_quad[index].x - 0.5_hf, h_quad[index].y - 0.5_hf));
		}

	}; // struct hQuad

} // namespace hrzn