#include "pch.h"
#include "CppUnitTest.h"

#include <type_traits>

#include "../include/htl/hrzn.h"
#include "../include/htl/containers.h"
#include "../include/htl/utility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {
	template<> inline std::wstring ToString<hrzn::vector2>(const hrzn::vector2& t) { RETURN_WIDE_STRING("vector2{ " << t.x << ", " << t.y << " }"); }
	template<> inline std::wstring ToString<hrzn::point2>(const hrzn::point2& t) { RETURN_WIDE_STRING("point2{ " << t.x << ", " << t.y << " }"); }
	template<> inline std::wstring ToString<hrzn::point_area>(const hrzn::point_area& a) { RETURN_WIDE_STRING("point_area{ " << a.x1 << ", " << a.y1 << ", " << a.x2 << ", " << a.y2 << " }"); }
}}}

namespace hrzn_test
{
	TEST_CLASS(HTL_BasicTypes)
	{
	public:

		TEST_METHOD(EpsilonTypeTest) {
			
#define ASSERT_EPSILON(T) Assert::AreNotEqual(T(0), hrzn::vEpsilon<T>::value, L"Failed epsilon equivelency for " L#T)

			ASSERT_EPSILON(bool);
			
			ASSERT_EPSILON(char);
			ASSERT_EPSILON(unsigned char);

			ASSERT_EPSILON(int);
			ASSERT_EPSILON(unsigned int);

			ASSERT_EPSILON(long);
			ASSERT_EPSILON(unsigned long);

			ASSERT_EPSILON(std::size_t);

			ASSERT_EPSILON(float);
			ASSERT_EPSILON(double);

		}

		TEST_METHOD(tuple2_EpsilonTest) {
			Assert::AreNotEqual(hrzn::vector2(), hrzn::vector2::EPSILON(), L"vector2 epsilon failure.");
			Assert::AreNotEqual(hrzn::point2(), hrzn::point2::EPSILON(), L"point2 epsilon failure.");


			Assert::AreNotEqual(hrzn::vector2(), hrzn::vector2().epsilonSigned(), L"vector2 epsilonSigned failure.");
			Assert::AreNotEqual(hrzn::point2(), hrzn::point2().epsilonSigned(), L"point2 epsilonSigned failure.");
		}

		TEST_METHOD(TypeTraits) {
			Assert::IsTrue(std::is_arithmetic<h_int>(), L"Test integral is arithmetic failure.");
			Assert::IsTrue(std::is_integral<h_int>(), L"Test is integral failure.");
			Assert::IsTrue(std::is_signed<h_int>(), L"Test integral signed failure.");

			Assert::IsTrue(std::is_arithmetic<h_float>(), L"Test floating point arithmetic failure.");
			Assert::IsTrue(std::is_floating_point<h_float>(), L"Test is floating point failure.");

			Assert::IsTrue(std::is_arithmetic<h_unsigned>(), L"Test unsigned is arithmetic failure.");
			Assert::IsTrue(std::is_integral<h_unsigned>(), L"Test unsigned is integral failure.");
			Assert::IsFalse(std::is_signed<h_unsigned>(), L"Test unsigned failure.");
		}

		TEST_METHOD(BasicOperators_tuple2) {
			int ax = 10;
			int ay = 34;
			int bx = 76;
			int by = 4;

			hrzn::point2 a = { ax, ay };
			hrzn::point2 b = { bx, by };

			Assert::AreEqual({ ax + bx, ay + by }, a + b, L"Addition failed.");
			Assert::AreEqual({ ax - bx, ay - by }, a - b, L"Subtraction failed.");
			Assert::AreEqual({ ax * bx, ay * by }, a * b, L"Multiplication failed.");
			Assert::AreEqual({ ax / bx, ay / by }, a / b, L"Division failed.");

			int v = 37;
			Assert::AreEqual({ ax * v, ay * v }, a * v, L"Scalar multiplication failed.");
			Assert::AreEqual({ ax / v, ay / v }, a / v, L"Scalar division failed.");

			Assert::AreEqual({ b.y, b.x }, ~b, L"Swizzle failed.");
			Assert::AreEqual({ -b.x, -b.y }, -b, L"Invert failed.");

		}

		TEST_METHOD(IndexAccessor_tuple2) {
			
			hrzn::point2 p{ 3, 11 };

			Assert::AreEqual(3, p[0], L"Index 0 failure on point.");
			Assert::AreEqual(11, p[1], L"Index 1 failure on point.");

		}

		TEST_METHOD(CornerIndexOutOfRange_point_area) {
			auto f1 = [this] {
				hrzn::point_area a = { 0, 0, 1, 1 };
				a.corner(4);
			};
			auto f2 = [this] {
				hrzn::point_area a = { 0, 0, 1, 1 };
				a.corner(-1);
			};

			Assert::ExpectException<std::out_of_range>(f1, L"Did not catch upper out of range index.");
			Assert::ExpectException<std::out_of_range>(f2, L"Did not catch lower out of range index.");

		}

		TEST_METHOD(CornerTests) {
			int x1 = -3;
			int y1 = -3;
			int x2 = 3;
			int y2 = 3;
			hrzn::point_area area = { x1, y1, x2, y2 };
			Assert::IsTrue(area.contains(area.center()), L"Center");
			Assert::IsTrue(area.contains(area.corner(0)), L"Top left corner");
			Assert::IsTrue(area.contains(area.corner(1)), L"Top right corner");
			Assert::IsTrue(area.contains(area.corner(2)), L"Bottom left corner");
			Assert::IsTrue(area.contains(area.corner(3)), L"Bottom right corner");
		}

		TEST_METHOD(OverlapChecking) {
			Assert::IsTrue(hrzn::overlap({ -1, -1, 3, 3 }, { 0, 0, 2, 2 }), L"Fully contained failure.");
			Assert::IsTrue(hrzn::overlap({ 0, 0, 10, 10 }, { 0, 0, 10, 10 }), L"Equality failure.");
		}

		TEST_METHOD(IntersectMethod) {
			hrzn::point2 p1 = { -9, -8 };
			hrzn::point2 p2 = { 2, 5 };
			hrzn::point2 p3 = { -5, -4 };
			hrzn::point2 p4 = { 8, 9 };

			hrzn::point_area area1(p1, p2);
			hrzn::point_area area2(p3, p4);

			Assert::AreEqual(hrzn::point_area(p3, p2), hrzn::intersect(area1, area2), L"Overlapping instersection does not match.");

			hrzn::point_area big_area = { 0, 0, 100, 100 };
			hrzn::point_area small_area = { 44, 7, 55, 60 };

			Assert::AreEqual(small_area, hrzn::intersect(big_area, small_area), L"Contained intersection of does not match.");


		}



	};

	TEST_CLASS(HTL_Containers)
	{
	public:

		TEST_METHOD(CellType_BasicMethods) {

			hrzn::Cell<char> cell_a(10, 10, 'A');

			hrzn::Cell<char> cell_b(10, 10, 'B');

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

			hrzn::Cell<char> cell_a(10, 10, 'A');
			hrzn::Cell<char> cell_b(15, 7, 'B');
			hrzn::Cell<char> cell_c(10, 10, 'C');

			hrzn::ICell<char>& a = cell_a;
			hrzn::ICell<char>& b = cell_b;

			a = b;

			Assert::IsTrue(cell_a.position() == cell_b.position(), L"Refrence assignment of cell positions failed.");
			Assert::IsTrue(cell_a.get() == cell_b.get(), L"Refrence assignment of cell contents failed.");

		}

		TEST_METHOD(HMap_BasicInsertionAndRetrieval) {
			hrzn::point_area area = { 20, 20, 100, 100 };
			hrzn::point2 location = { 25, 30 };
			char value = 'X';
			char fill = '-';

			hrzn::MapContainer<char> map_a(area, fill);

			map_a.set(location, value);
			Assert::AreEqual(value, map_a.at(location), L"Retrieval failure.");
		}

		TEST_METHOD(HMap_AccessOutOfBoundsException) {
			auto f = [this] {
				hrzn::MapContainer<char> map_a({ 0, 0, 1, 1 }, '.');
				map_a.at(2, 0);
			};
			Assert::ExpectException<std::out_of_range>(f);
		}

		TEST_METHOD(HMap_Iterator) {
			hrzn::MapContainer<int> map({7, 13, 111, 97}, -1);

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

		TEST_METHOD(HMap_IteratorMacro) {
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

		TEST_METHOD(HMap_StdFillTest) {
			hrzn::MapContainer<int> map(100, 100, -1);

			int fill_val = 999;
			std::fill(map.begin(), map.end(), fill_val);

			int deviations = 0;
			for (int x, y = map.y1; y < map.y2; ++y)
				for (x = map.x1; x < map.x2; ++x)
					if (map.at(x, y) != fill_val) deviations++;
			Assert::AreEqual(0, deviations, L"Iterator did not work properly with std::fill.");
		}


		TEST_METHOD(HMapRef_AccessTest) {
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


		TEST_METHOD(IMap_InPlaceTransformations) {
			
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

		TEST_METHOD(IMap_ExplicitCopyTest) {
			const hrzn::point2 pos = { 10, 10 };
			hrzn::MapContainer<char> map(100, 100, '.');

			auto copy = hrzn::copy(map);
			Assert::IsTrue(copy, L"Copy method failed.");
			Assert::AreEqual(copy.at(pos), map.at(pos), L"Map improperly copied.");

			copy.set(10, 10, '#');
			Assert::AreNotEqual(copy.at(pos), map.at(pos), L"Copied map is still improperly referenced.");
		}
	};



	TEST_CLASS(HTL_Utility) {
		TEST_METHOD(Util_DuplicateAndCompare) {
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

		TEST_METHOD(Util_IsEdgePoint) {
			hrzn::point_area area = { 1, 2, 12, 13 };
			int error = 0;
			for (int i = 0; i < 4; ++i) {
				if (!hrzn::is_edge(area, area.corner(i)))
					error++;
			}
			Assert::AreEqual(0, error, L"Edge detection failure.");
			Assert::IsFalse(hrzn::is_edge(area, area.center()), L"False positive test for center.");
		}

		TEST_METHOD(Util_OverlapAndContain) {
			hrzn::point_area area1 = { -10, -10, 50, 50 };
			hrzn::point_area area2 = { 20, 25, 100, 100 };
			hrzn::point_area area3 = { -3, -1, 10, 10 };
			Assert::IsTrue(hrzn::overlap(area1, area2), L"Failed overlap test.");
			Assert::IsFalse(hrzn::overlap(area3, area2), L"False positive overlap test.");
			Assert::IsTrue(hrzn::overlap(area1, area3), L"Failed contained overlap test.");

			
			Assert::IsTrue(hrzn::contains(area1, area3), L"Contains test false negative.");
			Assert::IsFalse(hrzn::contains(area1, area2), L"Contains test false positive.");
			Assert::IsTrue(hrzn::contains(area1, area3.center()), L"Contain point failure.");
			Assert::IsFalse(hrzn::contains(area1, hrzn::point2{ 1000, 1000 }), L"Outside contain point failure.");
		}

		TEST_METHOD(Util_CastCopyWriteFunction) {
			hrzn::MapContainer<bool> map_bool({ 50, 50 }, false);
			hrzn::MapContainer<int> map_int({ 2, 4, 30, 30 }, 99);

			hrzn::copy_into(map_int, map_bool);

			std::size_t count = 0;
			for (auto b : map_bool) {
				if (b) count++;
			}
			Assert::AreEqual(map_int.area(), count, L"Incomplete copy or cast failure.");

			int error = 0;
			for (int x, y = map_int.y1; y < map_int.y2; ++y)
				for (x = map_int.x1; x < map_int.x2; ++x)
					if (!map_bool.at(x, y)) error++;
			Assert::AreEqual(0, error, L"Malformed copy.");

		}

		TEST_METHOD(Util_TransposeListToMap_Undersized) {

			const int size = 67;
			const int value = 42;
			hrzn::point_area area(8, 3, 97, 53);

			std::vector<int> list;
			for (int i = 0; i < size; ++i)
				list.emplace_back(value);

			auto map = hrzn::transposeListToMap<int>(area, list.begin(), list.end());
			Assert::IsTrue(area == (hrzn::point_area)map, L"Map incorrectly created.");

			int count = 0;
			for (const auto& cell : map)
				if (cell == value) ++count;

			Assert::AreEqual(size, count, L"Copy failed while transposing undersized list to map.");
		}

		TEST_METHOD(Util_TransposeListToMap_Oversized) {

			const int value = 42;
			hrzn::point_area area(8, 3, 97, 53);
			const int size = (area.width() * area.height() + 13);

			std::vector<int> list;
			for (int i = 0; i < size; ++i)
				list.emplace_back(value);

			auto map = hrzn::transposeListToMap<int>(area, list.begin(), list.end());
			Assert::IsTrue(area == (hrzn::point_area)map, L"Map incorrectly created.");

			int count = 0;
			for (const auto& cell : map)
				if (cell == value) ++count;
			
			int area_size = area.area();
			Assert::AreEqual(area_size, count, L"Copy failed while transposing oversized list to map.");

		}

	};
}
