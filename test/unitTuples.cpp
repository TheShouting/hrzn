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
	TEST_CLASS(TupleTypes) {
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


			Assert::AreNotEqual(hrzn::vector2(), hrzn::vector2().epsilon_signed(), L"vector2 epsilon_signed failure.");
			Assert::AreNotEqual(hrzn::point2(), hrzn::point2().epsilon_signed(), L"point2 epsilon_signed failure.");
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
	};


}
