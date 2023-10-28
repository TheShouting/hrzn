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
#include <vector>

namespace hrzn {


	/******************************************************************************************************************
		Transformation types
	******************************************************************************************************************/


	/// <summary>
	/// A class holding position, angle, and scale values for use in transformation of various other coordinates in 2D space.
	/// </summary>
	struct transform {
		hrzn::vector2 position;
		hrzn::angle rotation;
		hrzn::vector2 scale;

		constexpr transform() : position(0._hf, 0._hf), rotation(0._hf), scale(1._hf, 1._hf) {}

		constexpr transform(hrzn::vector2 p, hrzn::angle r, hrzn::vector2 s) : position(p), rotation(r), scale(s) {}

		vector2 get_forward_vector() const {
			return rotation.get_forward_vector(scale.y);
		}

		vector2 get_right_vector() const {
			return rotation.get_right_vector(scale.x);
		}

		vector2 childPositon(vector2 pos) const {
			return position + rotation.rotate_vector(pos * scale);
		}

		hrzn::angle childRotation(hrzn::angle r) const {
			return rotation + r;
		}

		vector2 childScale(vector2 s) const {
			return scale * s;
		}

		//vector2 inversePosition(vector2 pos) {
		//	//return rotation.unrotate_vector(pos / scale.epsilon()) - pos;
		//	return rotation.unrotate_vector(pos / vector2::EPSILON2()) - pos;
		//}

		transform childTransform(const transform& child) const {
			return { childPositon(child.position), childRotation(child.rotation), childScale(child.scale) };
		}

	}; // struct transform

	struct i_polygon {

		i_polygon() {}
		virtual ~i_polygon() {}

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

	}; // struct i_polygon


	struct polygon : public i_polygon {
		std::vector<vector2> vertices;

		polygon() {}
		polygon(std::initializer_list<vector2> verts) : vertices(verts) {}
		polygon(const i_polygon& polygon) : vertices(polygon.list()) {}

		std::size_t count() const override { return vertices.size(); }
		vector2 get(std::size_t index) const { return vertices[index]; }
	}; // struct polygon


	template <unsigned int N>
	struct i_ngon : public i_polygon {
		std::size_t count() const override { return N; }
	}; // struct i_ngon<int>


	//struct hBox : public i_ngon<4> {

	//	vector2 v1;
	//	vector2 v2;

	//	hBox() : v1(0._hf), v2(1._hf) {}
	//	hBox(vector2 size) : v1(0._hf), v2(size) {}
	//	hBox(vector2 v1, vector2 v2) : v1(v1), v2(v2) {}
	//	hBox(h_float w, h_float h) : v1(0._hf), v2(w, h) {}
	//	hBox(h_float ax, h_float ay, h_float bx, h_float by) : v1(ax, ay), v2(bx, by) {}
	//	hBox(rectangle area) : v1(area.x, area.y), v2(area.x + area.w, area.y + area.h) {}

	//	vector2 get(std::size_t index) const override {
	//		return { (&v1)[h_corner[index].x].x, (&v1)[h_corner[index].y].y };
	//	}

	//}; // struct hBox


	//struct quad : public transform, i_ngon<4> {

	//	quad() : transform() {}
	//	quad(h_float w, h_float h) : transform({ 0,0 }, 0.f, { w, h }) {}
	//	quad(vector2 dim) : transform() {}
	//	quad(transform tform) : transform(tform) {}

	//	vector2 get(std::size_t index) const override {
	//		return position + rotation.rotate_vector(scale * vector2(h_quad[index].x - 0.5_hf, h_quad[index].y - 0.5_hf));
	//	}

	//}; // struct quad

} // namespace hrzn