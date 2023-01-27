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

namespace hrznBasicTypesTest
{
	TEST_CLASS(TupleTypes)
	{
	public:

		TEST_METHOD(EpsilonTypeTest) {
			
#define ASSERT_EPSILON(T) Assert::AreNotEqual(T(0), hrzn::_lib::vEpsilon<T>::value, L"Failed epsilon equivelency for " L#T)

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
			Assert::AreNotEqual(hrzn::vector2(), hrzn::vector2::EPSILON2(), L"vector2 epsilon failure.");
			Assert::AreNotEqual(hrzn::point2(), hrzn::point2::EPSILON2(), L"point2 epsilon failure.");


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


}
