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

namespace hrznContainerTests {

	TEST_CLASS(cell_pointer_type) {
public:

	TEST_METHOD(refernce_test) {
		hrzn::MapContainer<char> map(10, 10);
		map.fill('.');
		hrzn::point2 pt{ 5, 5 };
		hrzn::cell_pointer cell = map.get_cell(pt);
		cell.contents = 'X';
		Assert::AreEqual(cell.contents, map.at(pt), L"Reference variable of cell connector is not valid.");
	}

	};

	TEST_CLASS(MapType) {
public:

	TEST_METHOD(Map_StdFillTest) {
		hrzn::MapContainer<int> map(100, 100, -1);

		int fill_val = 999;
		std::fill(map.begin(), map.end(), fill_val);

		int deviations = 0;
		for (int x, y = map.y; y < map.last().y; ++y)
			for (x = map.x; x < map.last().x; ++x)
				if (map.at(x, y) != fill_val) deviations++;
		Assert::AreEqual(0, deviations, L"Iterator did not work properly with std::fill.");
	}

	TEST_METHOD(Map_InPlaceTransformations) {

		const hrzn::point2 edge(19, 19);

		const hrzn::point2 top_left(0, 0);
		const hrzn::point2 top_right(edge.x, 0);
		const hrzn::point2 bottom_right(edge.x, edge.y);
		const hrzn::point2 bottom_left(0, edge.y);

		hrzn::MapContainer<char> map(edge.x + 1, edge.y + 1, '?');

		map.set(top_left, 'A');
		map.flip_x();
		Assert::AreEqual('A', map.at(top_right), L"flip_x() method failed.");

		map.set(top_left, 'B');
		map.flip_y();
		Assert::AreEqual('B', map.at(bottom_left), L"flip_y() method failed.");

		map.set(top_left, 'C');
		map.reverse();
		Assert::AreEqual('C', map.at(bottom_right), L"reverse() method failed.");
	}

	TEST_METHOD(Map_ExplicitCopyTest) {
		const hrzn::point2 pos = { 10, 10 };
		hrzn::MapContainer<char> map(100, 100, '.');

		auto copy = hrzn::copy(map);
		Assert::IsTrue(copy, L"Copy method failed.");
		Assert::AreEqual(copy.at(pos), map.at(pos), L"I_Map improperly copied.");

		copy.set(10, 10, '#');
		Assert::AreNotEqual(copy.at(pos), map.at(pos), L"Copied map is still improperly referenced.");
	}

	TEST_METHOD(Map_Swap) {
		const hrzn::point2 a = { 85, 12 };
		const char a_val = 'A';

		const hrzn::point2 b = { 10, 36 };
		const char b_val = 'b';

		hrzn::MapContainer<char> map(100, 100, '.');
		map[a] = a_val;
		map[b] = b_val;

		map.swap(a, b);

		Assert::AreEqual(a_val, map.at(b), L"Value swap failed.");
		Assert::AreEqual(b_val, map.at(a), L"Value swap failed.");
	}

	TEST_METHOD(MapContainer_Retrieval) {
		const hrzn::rectangle area = { 20, 20, 100, 100 };
		const hrzn::point2 location = { 25, 30 };
		const char value = 'X';
		const char fill = '-';

		hrzn::MapContainer<char> map_a(area, fill);

		map_a.set(location, value);
		Assert::AreEqual(value, map_a.at(location), L"Retrieval failure with method.");
		Assert::AreEqual(value, map_a[location], L"Retrieval failure with indexer.");
	}

	TEST_METHOD(MapContainer_AccessOutOfBoundsException) {
		auto f = [this] {
			hrzn::MapContainer<char> map_a({ 0, 0, 1, 1 }, '.');
			map_a.at(2, 0);
		};
		Assert::ExpectException<std::out_of_range>(f);
	}

	TEST_METHOD(MapContainer_Iterator) {

		hrzn::rectangle rect = { 7, 13, 111, 97 };

		hrzn::MapContainer<int> map(rect, -1);

		try {
			int i1 = 0;
			for (auto& cell : map) {
				cell = i1;
				i1++;
			}
		} catch (std::exception e) {
			std::string str = e.what();
			std::wstring w(str.begin(), str.end());
			Assert::Fail(w.c_str());
		}

		int i2 = 0;
		int deviations = 0;
		for (int x, y = map.y; y < map.last().y; ++y)
			for (x = map.x; x < map.last().x; ++x) {
				if (i2 != map.at(x, y)) deviations++;
				i2++;
			}

		Assert::AreEqual(0, deviations, L"Iterator assignment failure.");
	}
	
	TEST_METHOD(MapContainer_IteratorMacro) {
		hrzn::MapContainer<int> map(100, 100, -1);

		int i1 = 0;
		HRZN_FOREACH_POINT(map, x, y) {
			map.set(x, y, i1);
			i1++;
		}

		int i2 = 0;
		int deviations = 0;
		for (int x, y = map.y; y < map.last().y; ++y)
			for (x = map.x; x < map.last().x; ++x) {
				if (i2 != map.at(x, y)) deviations++;
				i2++;
			}

		Assert::AreEqual(0, deviations, L"Iterator assignment failure.");

	}

	TEST_METHOD(MapReference_AccessTest) {
		hrzn::rectangle area = { -10, -10, 110, 110 };
		hrzn::MapContainer<char> map(100, 100, '.');

		auto ref1 = hrzn::GetReferenceArea(area, map);

		std::fill(ref1.begin(), ref1.end(), '#');

		std::size_t c1 = 0;
		for (auto cell : map) {
			if (cell == '#')
				c1++;
		}
		Assert::AreEqual(ref1.area(), c1, L"Source map sub-area does not map reference map.");
	}

	};
}
