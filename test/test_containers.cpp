#include "pch.h"

#include "test_common.h"

namespace hrznContainerTests {

	TEST_CLASS(CellType) {
public:

	TEST_METHOD(CellType_BasicMethods) {

		hrzn::CellObject<char> cell_a(10, 10, 'A');

		hrzn::CellObject<char> cell_b(10, 10, 'B');

		Assert::IsTrue(cell_a == cell_b, L"Comparison between cells failed.");
		Assert::IsFalse(cell_a != cell_b, L"Difference comparison between cells failed.");
		Assert::IsTrue(cell_a.get() != cell_b.get(), L"Comparison between cell contents using get() failed.");

		Assert::AreEqual('A', cell_a.get(), L"Cell accessor method failed.");
		Assert::IsTrue(cell_a == hrzn::point2(10, 10), L"Cell and point2 comparison failed.");

		cell_a = cell_b;
		Assert::AreEqual('B', cell_a.get(), L"Cell to Cell assignment failed.");

		cell_b.get() = 'C';
		Assert::AreEqual('C', cell_b.get(), L"Cell get() method failed.");

	}

	TEST_METHOD(CellType_AbstractionAssignment) {

		hrzn::CellObject<char> cell_a(10, 10, 'A');
		hrzn::CellObject<char> cell_b(15, 7, 'B');
		hrzn::CellObject<char> cell_c(10, 10, 'C');

		hrzn::Cell<char>& a = cell_a;
		hrzn::Cell<char>& b = cell_b;

		a = b;

		Assert::IsTrue(cell_a.position() == cell_b.position(), L"Refrence assignment of cell positions failed.");
		Assert::IsTrue(cell_a.get() == cell_b.get(), L"Refrence assignment of cell contents failed.");

	}
	};

	TEST_CLASS(MapType) {
public:

	TEST_METHOD(Map_StdFillTest) {
		hrzn::MapContainer<int> map(100, 100, -1);

		int fill_val = 999;
		std::fill(map.begin(), map.end(), fill_val);

		int deviations = 0;
		for (int x, y = map.y1; y < map.y2; ++y)
			for (x = map.x1; x < map.x2; ++x)
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
		map.flipX();
		Assert::AreEqual('A', map.at(top_right), L"flipX() method failed.");

		map.set(top_left, 'B');
		map.flipY();
		Assert::AreEqual('B', map.at(bottom_left), L"flipY() method failed.");

		map.set(top_left, 'C');
		map.reverse();
		Assert::AreEqual('C', map.at(bottom_right), L"reverse() method failed.");
	}

	TEST_METHOD(Map_ExplicitCopyTest) {
		const hrzn::point2 pos = { 10, 10 };
		hrzn::MapContainer<char> map(100, 100, '.');

		auto copy = hrzn::copy(map);
		Assert::IsTrue(copy, L"Copy method failed.");
		Assert::AreEqual(copy.at(pos), map.at(pos), L"Map improperly copied.");

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
		const hrzn::point_area area = { 20, 20, 100, 100 };
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
		hrzn::MapContainer<int> map({ 7, 13, 111, 97 }, -1);

		int i1 = 0;
		for (auto& cell : map) {
			cell = i1;
			i1++;
		}

		int i2 = 0;
		int deviations = 0;
		for (int x, y = map.y1; y < map.y2; ++y)
			for (x = map.x1; x < map.x2; ++x) {
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
		for (int x, y = map.y1; y < map.y2; ++y)
			for (x = map.x1; x < map.x2; ++x) {
				if (i2 != map.at(x, y)) deviations++;
				i2++;
			}

		Assert::AreEqual(0, deviations, L"Iterator assignment failure.");

	}

	TEST_METHOD(MapReference_AccessTest) {
		hrzn::point_area area = { -10, -10, 110, 110 };
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
