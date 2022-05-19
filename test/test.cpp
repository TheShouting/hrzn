#include "pch.h"
#include "CppUnitTest.h"

#include "../include/htl/hrzn.h"
#include "../include/htl/utility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {
	template<> inline std::wstring ToString<hrzn::hVector>(const hrzn::hVector& t) { RETURN_WIDE_STRING("hVector{ " << t.x << ", " << t.y << " }"); }
	template<> inline std::wstring ToString<hrzn::hPoint>(const hrzn::hPoint& t) { RETURN_WIDE_STRING("hPoint{ " << t.x << ", " << t.y << " }"); }
	template<> inline std::wstring ToString<hrzn::hArea>(const hrzn::hArea& a) { RETURN_WIDE_STRING("hArea{ " << a.x1 << ", " << a.y1 << ", " << a.x2 << ", " << a.y2 << " }"); }
}}}

namespace hrzn
{
	TEST_CLASS(HTL_BasicTypes)
	{
	public:

		TEST_METHOD(hTuple_BasicOperators) {
			int ax = 10;
			int ay = 34;
			int bx = 76;
			int by = 4;

			hrzn::hPoint a = { ax, ay };
			hrzn::hPoint b = { bx, by };

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

		TEST_METHOD(hArea_ValidateCornerIndexOutOfRange) {
			auto f1 = [this] {
				hrzn::hArea a = { 0, 0, 1, 1 };
				a.corner(4);
			};
			auto f2 = [this] {
				hrzn::hArea a = { 0, 0, 1, 1 };
				a.corner(-1);
			};

			Assert::ExpectException<std::out_of_range>(f1, L"Did not catch upper out of reange index.");
			Assert::ExpectException<std::out_of_range>(f2, L"Did not catch lower out of reange index.");

		}

		TEST_METHOD(hArea_CornersAndCenter) {
			int x1 = -3;
			int y1 = -3;
			int x2 = 3;
			int y2 = 3;
			hArea area = { x1, y1, x2, y2 };
			Assert::IsTrue(area.contains(area.center()), L"Center");
			Assert::IsTrue(area.contains(area.corner(0)), L"Top left corner");
			Assert::IsTrue(area.contains(area.corner(1)), L"Top right corner");
			Assert::IsTrue(area.contains(area.corner(2)), L"Bottom left corner");
			Assert::IsTrue(area.contains(area.corner(3)), L"Bottom right corner");
		}

		TEST_METHOD(hArea_OverlapingChecking) {
			Assert::IsTrue(overlapping({ -1, -1, 3, 3 }, { 0, 0, 2, 2 }), L"Fully contained failure.");
			Assert::IsTrue(overlapping({ 0, 0, 10, 10 }, { 0, 0, 10, 10 }), L"Equality failure.");
		}

		TEST_METHOD(hArea_Intersect) {
			hPoint p1 = { -9, -8 };
			hPoint p2 = { 2, 5 };
			hPoint p3 = { -5, -4 };
			hPoint p4 = { 8, 9 };

			hArea area1(p1, p2);
			hArea area2(p3, p4);

			Assert::AreEqual(hArea(p3, p2), hrzn::intersect(area1, area2), L"Overlapping instersection does not match.");

			hArea big_area = { 0, 0, 100, 100 };
			hArea small_area = { 44, 7, 55, 60 };

			Assert::AreEqual(small_area, hrzn::intersect(big_area, small_area), L"Contained intersection of does not match.");


		}



	};

	TEST_CLASS(HTL_MapContainter)
	{
	public:

		TEST_METHOD(HMap_BasicInsertionAndRetrieval) {
			hrzn::hArea area = { 20, 20, 100, 100 };
			hrzn::hPoint location = { 25, 30 };
			char value = 'X';
			char fill = '-';

			hrzn::HMap<char> map_a(area, fill);

			map_a.set(location, value);
			Assert::AreEqual(value, map_a.at(location), L"Retrieval failure.");
		}

		TEST_METHOD(HMap_AccessOutOfBoundsException) {
			auto f = [this] {
				hrzn::HMap<char> map_a({ 0, 0, 1, 1 }, '.');
				map_a.at(2, 0);
			};
			Assert::ExpectException<std::out_of_range>(f);
		}
		TEST_METHOD(HMap_Iterator) {
			hrzn::HMap<int> map(100, 100, -1);

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

		TEST_METHOD(HMap_StdFillTest) {
			hrzn::HMap<int> map(100, 100, -1);

			int fill_val = 999;
			std::fill(map.begin(), map.end(), fill_val);

			int deviations = 0;
			for (int x, y = map.y1; y < map.y2; ++y)
				for (x = map.x1; x < map.x2; ++x)
					if (map.at(x, y) != fill_val) deviations++;
			Assert::AreEqual(0, deviations, L"Iterator did not work properly with std::fill.");
		}


		TEST_METHOD(HMapRef_AccessTest) {
			hArea area = { -10, -10, 110, 110 };
			hrzn::HMap<char> map(100, 100, '.');

			auto ref1 = hrzn::ReferenceArea(area, map);

			std::fill(ref1.begin(), ref1.end(), '#');

			std::size_t c1 = 0;
			for (auto cell : map) {
				if (cell == '#')
					c1++;
			}
			Assert::AreEqual(ref1.area(), c1, L"Source map sub-area does not map reference map.");
		}
	};



	TEST_CLASS(HTL_Utility) {
		TEST_METHOD(Util_DuplicateAndCompare) {
			char val1 = 'X';
			char val2 = '+';
			hrzn::hPoint loc = { 2, 2 };

			hrzn::HMap<char> map_a({ 0, 0, 4, 4 }, '.');
			map_a.set(loc, val1);

			for (int i = 0; i < 4; ++i)
				map_a.set(map_a.corner(i), val2);

			hrzn::HMap<char> map_b = hrzn::duplicate<char>(map_a);

			Assert::AreEqual(val1, map_b.at(loc), L"Direct equivelence failure.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(0)), L"Comparison failure at corner 0.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(1)), L"Comparison failure at corner 1.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(2)), L"Comparison failure at corner 2.");
			Assert::AreEqual(val2, map_b.at(map_b.corner(3)), L"Comparison failure at corner 3.");


			for (int i = 0; i < 4; ++i)
				map_b.set(map_b.corner(i), val1);

			Assert::IsFalse(hrzn::compare(map_a, map_b), L"Comparison function faild.");
		}

		TEST_METHOD(Util_IsEdgePoint) {
			hrzn::hArea area = { 1, 2, 12, 13 };
			int error = 0;
			for (int i = 0; i < 4; ++i) {
				if (!hrzn::isEdgePoint(area, area.corner(i)))
					error++;
			}
			Assert::AreEqual(0, error, L"Edge detection failure.");
			Assert::IsFalse(hrzn::isEdgePoint(area, area.center()), L"False positive test for center.");
		}

		TEST_METHOD(Util_OverlapAndContain) {
			hrzn::hArea area1 = { -10, -10, 50, 50 };
			hrzn::hArea area2 = { 20, 25, 100, 100 };
			hrzn::hArea area3 = { -3, -1, 10, 10 };
			Assert::IsTrue(hrzn::overlapping(area1, area2), L"Failed overlap test.");
			Assert::IsFalse(hrzn::overlapping(area3, area2), L"False positive overlap test.");
			Assert::IsTrue(hrzn::overlapping(area1, area3), L"Failed contained overlap test.");

			
			Assert::IsTrue(hrzn::contains(area1, area3), L"Contains test false negative.");
			Assert::IsFalse(hrzn::contains(area1, area2), L"Contains test false positive.");
			Assert::IsTrue(hrzn::contains(area1, area3.center()), L"Contain point failure.");
			Assert::IsFalse(hrzn::contains(area1, hPoint{ 1000, 1000 }), L"Outside contain point failure.");
		}

		TEST_METHOD(Util_CastCopyFunction) {
			HMap<bool> map_bool({ 50, 50 }, false);
			HMap<int> map_int({ 2, 4, 30, 30 }, 99);

			hrzn::copy(map_int, map_bool);

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

	};
}
