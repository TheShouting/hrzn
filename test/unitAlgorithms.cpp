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

namespace hrznAlgorithmTests {

	TEST_CLASS(UtilityFunctions) {
		TEST_METHOD(ClassMap_DuplicateAndCompare) {
			char val1 = 'X';
			char val2 = '+';
			hrzn::point2 loc = { 2, 2 };

			hrzn::MapContainer<char> map_a({ 0, 0, 4, 4 }, '.');
			map_a.set(loc, val1);

			for (int i = 0; i < 4; ++i)
				map_a.set(map_a.corner(i), val2);

			hrzn::MapContainer<char> map_b = hrzn::copy_each<char>(map_a);

			Assert::AreEqual(val1, map_b.at(loc), L"Direct equivelence failure.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(0)), L"Comparison failure at corner 0.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(1)), L"Comparison failure at corner 1.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(2)), L"Comparison failure at corner 2.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(3)), L"Comparison failure at corner 3.");


			for (int i = 0; i < 4; ++i)
				map_b.set(map_b.corner(i), val1);

			Assert::IsFalse(hrzn::equal(map_a, map_b), L"Comparison function faild.");
		}

		TEST_METHOD(rectangle_IsEdgePoint) {
			hrzn::rectangle area = { 1, 2, 12, 13 };
			int error = 0;
			for (int i = 0; i < 4; ++i) {
				if (!hrzn::is_edge(area, area.corner(i)))
					error++;
			}
			Assert::AreEqual(0, error, L"Edge detection failure.");
			Assert::IsFalse(hrzn::is_edge(area, area.center()), L"False positive test for center.");
		}

		TEST_METHOD(rectangle_OverlapAndContain) {
			hrzn::rectangle area1 = { -10, -10, 50, 50 };
			hrzn::rectangle area2 = { 20, 25, 100, 100 };
			hrzn::rectangle area3 = { -3, -1, 10, 10 };
			Assert::IsTrue(hrzn::overlap(area1, area2), L"Failed overlap test.");
			Assert::IsFalse(hrzn::overlap(area3, area2), L"False positive overlap test.");
			Assert::IsTrue(hrzn::overlap(area1, area3), L"Failed contained overlap test.");


			Assert::IsTrue(hrzn::contains(area1, area3), L"Contains test false negative.");
			Assert::IsFalse(hrzn::contains(area1, area2), L"Contains test false positive.");
			Assert::IsTrue(hrzn::contains(area1, area3.center()), L"Contain point failure.");
			Assert::IsFalse(hrzn::contains(area1, hrzn::point2{ 1000, 1000 }), L"Outside contain point failure.");
		}

		TEST_METHOD(Map_CastCopyWriteFunction) {
			hrzn::MapContainer<bool> map_bool({ 50, 50 }, false);
			hrzn::MapContainer<int> map_int({ 2, 4, 30, 30 }, 99);

			hrzn::copy_into(map_int, map_bool);

			std::size_t count = 0;
			for (auto b : map_bool) {
				if (b) count++;
			}
			Assert::AreEqual(map_int.area(), count, L"Incomplete copy or cast failure.");

			int error = 0;
			for (int x, y = map_int.y; y < map_int.last().y; ++y)
				for (x = map_int.x; x < map_int.last().x; ++x)
					if (!map_bool.at(x, y)) error++;
			Assert::AreEqual(0, error, L"Malformed copy.");

		}

		TEST_METHOD(method_transpose_list_to_map_test_undersized) {

			const int size = 67;
			const int value = 42;
			hrzn::rectangle area(8, 3, 97, 53);

			std::vector<int> list;
			for (int i = 0; i < size; ++i)
				list.emplace_back(value);

			auto map = hrzn::transpose_list_to_map<int>(area, list.begin(), list.end());
			Assert::IsTrue(area == (hrzn::rectangle)map, L"I_Map incorrectly created.");

			int count = 0;
			for (const auto& cell : map)
				if (cell == value) ++count;

			Assert::AreEqual(size, count, L"Copy failed while transposing undersized list to map.");
		}

		TEST_METHOD(method_transpose_list_to_map_test_oversized) {

			const int value = 42;
			hrzn::rectangle area(8, 3, 97, 53);
			const int size = (area.width() * area.height() + 13);

			std::vector<int> list;
			for (int i = 0; i < size; ++i)
				list.emplace_back(value);

			auto map = hrzn::transpose_list_to_map<int>(area, list.begin(), list.end());
			Assert::IsTrue(area == (hrzn::rectangle)map, L"I_Map incorrectly created.");

			int count = 0;
			for (const auto& cell : map)
				if (cell == value) ++count;

			int area_size = area.area();
			Assert::AreEqual(area_size, count, L"Copy failed while transposing oversized list to map.");

		}

		TEST_METHOD(method_project_from_point) {

			const int length = 10;

			hrzn::MapContainer<char> map(100, 100);

			map.fill('0');

			auto l = hrzn::project_from_point(map, { 10, 10 }, { 0, 1 }, length);

			for (auto i : l) {
				i.contents = 'A';
			}

			int count = 0;
			for (auto c : map) {
				if (c == 'A') count++;
			}

			Assert::AreEqual(count, length, L"projection length error");
			
		}

	};
}