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

#include "pch.h"

#include "test_common.h"

namespace hrznBasicTypesTest {

TEST_CLASS(rectangle_types) {
public:

#ifndef HRZN_NOEXCEPTIONS
	TEST_METHOD(CornerIndexOutOfRange_point_area) {
		auto f1 = [this] {
			hrzn::rectangle a = { 0, 0, 1, 1 };
			a.corner(4);
		};
		auto f2 = [this] {
			hrzn::rectangle a = { 0, 0, 1, 1 };
			a.corner(-1);
		};

		Assert::ExpectException<std::out_of_range>(f1, L"Did not catch upper out of range index.");
		Assert::ExpectException<std::out_of_range>(f2, L"Did not catch lower out of range index.");

	}
#endif // !H_NOEXCEPT

	TEST_METHOD(contains_point_method) {
		hrzn::rectangle rect(-10, -10, 20, 20);	
		Assert::IsTrue(rect.contains(0, 0), L"Failed check for point inside rectangle.");
		Assert::IsFalse(rect.contains(0, 35), L"Failed check for point outside rectangle.");
	}

	TEST_METHOD(contains_rectangle_method) {
		hrzn::rectangle rect(-10, -10, 20, 20);
		Assert::IsTrue(rect.contains(hrzn::rectangle(0, 0, 5, 5)), L"Failed check for rectangle inside rectangle.");
		Assert::IsFalse(rect.contains(hrzn::rectangle(0, 0, 25, 25)), L"Failed check for rectangle partially in rectangle.");
		Assert::IsFalse(rect.contains(hrzn::rectangle(20, 20, 5, 5)), L"Failed check for rectangle outside rectangle.");
		Assert::IsFalse(rect.contains(hrzn::rectangle(0, 0, -1, -1)), L"Failed check for invalid rectangle.");
	}

	TEST_METHOD(corner_method) {
		int x1 = -3;
		int y1 = -3;
		int x2 = 3;
		int y2 = 3;
		hrzn::rectangle area = { x1, y1, x2, y2 };
		Assert::IsTrue(area.contains(area.center()), L"Center");
		Assert::IsTrue(area.contains(area.corner(0)), L"Top left corner");
		Assert::IsTrue(area.contains(area.corner(1)), L"Top right corner");
		Assert::IsTrue(area.contains(area.corner(2)), L"Bottom left corner");
		Assert::IsTrue(area.contains(area.corner(3)), L"Bottom right corner");
	}

	TEST_METHOD(overlap_method) {
		Assert::IsTrue(hrzn::overlap({ -1, -1, 3, 3 }, { 0, 0, 2, 2 }), L"Fully contained failure.");
		Assert::IsTrue(hrzn::overlap({ 0, 0, 10, 10 }, { 0, 0, 10, 10 }), L"Equality failure.");
	}

	TEST_METHOD(intersect_method_overlap) {
		hrzn::point2 p1 = { -9, -8 };
		hrzn::point2 p2 = { 2, 5 };
		hrzn::point2 p3 = { -5, -4 };
		hrzn::point2 p4 = { 8, 9 };

		auto area1 = hrzn::rectangle::from_corners(p1, p2);
		auto area2 = hrzn::rectangle::from_corners(p3, p4);

		Assert::AreEqual(hrzn::rectangle::from_corners(p3, p2), hrzn::intersect(area1, area2), L"Overlapping instersection does not match.");
	}

	TEST_METHOD(intersect_method_contained) {
		hrzn::rectangle big_area = { 0, 0, 100, 100 };
		hrzn::rectangle small_area = { 21, 7, 55, 60 };
		Assert::AreEqual(small_area, hrzn::intersect(big_area, small_area), L"Contained intersection of does not match.");
	}

	TEST_METHOD(wrap_point_method) {
		hrzn::rectangle rect = { 3, 5, 100, 100 };
		hrzn::point2 pt = rect.wrap({ -25, -30 });
		Assert::IsTrue(rect.contains(pt), L"Point is not located within the rectangle.");
	}

	};
}