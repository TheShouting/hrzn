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

	TEST_CLASS(RotationTypes) {
	public:

		TEST_METHOD(UnitConversions) {
			hrzn::angle a(0.5_hf);
			Assert::AreEqual(180._hf, a.deg(), hrzn::EPSILON, L"Conversions to degrees failed.");
			Assert::AreEqual(hrzn::PI, a.rad(), hrzn::EPSILON, L"Conversion to radians failed.");

			const auto degrees = 180._hf;
			const auto radian = 3.141592_hf;
			Assert::AreEqual(degrees, hrzn::angle::from_degrees(degrees).deg(), hrzn::EPSILON, L"Construction of angle with degrees failed.");
			Assert::AreEqual(radian, hrzn::angle::from_radians(radian).rad(), hrzn::EPSILON, L"Construction of angle with radians failed.");
			
			// TODO This comparison fail if Epsilon is too small.
			Assert::AreEqual(1._hf, hrzn::angle::from_radians(1._hf).rad(), hrzn::EPSILON, L"Construction of angle with radian literals failed.");
		}

		TEST_METHOD(AngleFromVector) {

			auto f = [this](h_float angle_t) {
				const hrzn::angle a(angle_t);
				hrzn::angle v_angle = hrzn::angle::from_vector(a.get_forward_vector());
				Assert::AreEqual(a.tau, v_angle.tau, hrzn::EPSILON, L"Failed to constuct angle from vector");
			};

			f(0.0_hf); // 0d
			f(0.25_hf); // 90d
			f(0.5_hf); // 180d
			f(0.75_hf); // 270d
			f(0.137_hf); // 49.32d
			f(0.014_hf); // 5.04d

		}

		TEST_METHOD(BasicOperations) {
			hrzn::angle a = hrzn::angle::DOWN() + hrzn::angle::LEFT();
			Assert::AreEqual(0.25_hf, a.tau, hrzn::EPSILON, L"Rotation over addition failed.");
		}

		TEST_METHOD(Comparison) {

			Assert::AreEqual(0.2_hf, hrzn::difference({ 0.9 }, { 0.1 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.9 - 0.1).");
			Assert::AreEqual(0.2_hf, hrzn::difference({ 0.1 }, { 0.9 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.1 - 0.9).");

			Assert::AreEqual(0.2_hf, hrzn::difference({ 0.4 }, { 0.6 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.4 - 0.6).");
			Assert::AreEqual(0.2_hf, hrzn::difference({ 0.6 }, { 0.4 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.6 - 0.4).");

			Assert::AreEqual(0.5_hf, hrzn::difference({ 0.25 }, { 0.75 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.25 - 0.75).");
			Assert::AreEqual(0.5_hf, hrzn::difference({ 0.75 }, { 0.25 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.75 - 0.25).");

			Assert::AreEqual(0.5_hf, hrzn::difference({ 0.0 }, { 0.5 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.0 - 0.5).");
			Assert::AreEqual(0.5_hf, hrzn::difference({ 0.5 }, { 0.0 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.5 - 0.0).");

			Assert::AreEqual(0._hf, hrzn::difference({ 0.0 }, { 1.0 }).tau, hrzn::EPSILON, L"Rotational difference failed (0.0 - 1.0).");
			Assert::AreEqual(0._hf, hrzn::difference({ 1.0 }, { 0.0 }).tau, hrzn::EPSILON, L"Rotational difference failed (1.0 - 0.0).");

			Assert::IsTrue(hrzn::compare(hrzn::angle::UP(), hrzn::angle()), L"Comparison of angle 0 failed.");
			Assert::IsFalse(hrzn::compare(hrzn::angle::UP(), hrzn::angle::DOWN()), L"Comparison of Up and Down angles failed.");
			Assert::IsFalse(hrzn::compare(hrzn::angle::LEFT(), hrzn::angle::RIGHT()), L"Comparison of Left and Right angles failed.");

			Assert::IsTrue(hrzn::compare(hrzn::angle(0.9), hrzn::angle(0.1), 0.25), L"Comparison with 90deg epsilon failed.");
			Assert::IsTrue(hrzn::compare(hrzn::angle(0.4), hrzn::angle(0.6), 0.25), L"Comparison with 90deg epsilon failed.");

		}

		TEST_METHOD(Direction) {
			Assert::IsTrue(hrzn::direction(hrzn::angle::UP(), hrzn::angle::RIGHT()), L"Up>Right Rotation Direction comparison failed.");
			Assert::IsTrue(hrzn::direction({ 0.9 }, { 0.1 }), L"0.9>0.1 Rotation Direction comparison failed.");

			Assert::IsFalse(hrzn::direction(hrzn::angle::LEFT(), hrzn::angle::DOWN()), L"Left>Down Rotation Direction comparison failed.");
			Assert::IsFalse(hrzn::direction({ 0.1 }, { 0.9 }), L"0.1>0.9 Rotation Direction comparison failed.");
		}


		TEST_METHOD(VectorDerivation) {
			const h_float epsilon = hrzn::EPSILON;
			Assert::AreEqual(hrzn::angle::UP().get_forward_vector().x, hrzn::vector2::UP().x, epsilon, L"Incorrect UP X Forward vector from angle.");
			Assert::AreEqual(hrzn::angle::UP().get_forward_vector().y, hrzn::vector2::UP().y, epsilon, L"Incorrect UP Y Forward vector from angle.");

			Assert::AreEqual(hrzn::angle::LEFT().get_forward_vector().x, hrzn::vector2::LEFT().x, epsilon, L"Incorrect LEFT X Forward vector from angle.");
			Assert::AreEqual(hrzn::angle::LEFT().get_forward_vector().y, hrzn::vector2::LEFT().y, epsilon, L"Incorrect LEFT Y Forward vector from angle.");

			Assert::AreEqual(hrzn::angle::DOWN().get_forward_vector().x, hrzn::vector2::DOWN().x, epsilon, L"Incorrect DOWN X Forward vector from angle.");
			Assert::AreEqual(hrzn::angle::DOWN().get_forward_vector().y, hrzn::vector2::DOWN().y, epsilon, L"Incorrect DOWN Y Forward vector from angle.");

			Assert::AreEqual(hrzn::angle::RIGHT().get_forward_vector().x, hrzn::vector2::RIGHT().x, epsilon, L"Incorrect RIGHT X Forward vector from angle.");
			Assert::AreEqual(hrzn::angle::RIGHT().get_forward_vector().y, hrzn::vector2::RIGHT().y, epsilon, L"Incorrect RIGHT Y Forward vector from angle.");

			Assert::AreEqual(hrzn::angle::UP().get_right_vector().x, hrzn::vector2::RIGHT().x, epsilon, L"Incorrect UP X Right vector from angle.");
			Assert::AreEqual(hrzn::angle::UP().get_right_vector().y, hrzn::vector2::RIGHT().y, epsilon, L"Incorrect UP Y Right vector from angle.");

			Assert::AreEqual(hrzn::angle::RIGHT().get_right_vector().x, hrzn::vector2::DOWN().x, epsilon, L"Incorrect RIGHT X Right vector from angle.");
			Assert::AreEqual(hrzn::angle::RIGHT().get_right_vector().y, hrzn::vector2::DOWN().y, epsilon, L"Incorrect RIGHT Y Right vector from angle.");

			Assert::AreEqual(hrzn::angle::DOWN().get_right_vector().x, hrzn::vector2::LEFT().x, epsilon, L"Incorrect DOWN X Right vector from angle.");
			Assert::AreEqual(hrzn::angle::DOWN().get_right_vector().y, hrzn::vector2::LEFT().y, epsilon, L"Incorrect DOWN Y Right vector from angle.");

			Assert::AreEqual(hrzn::angle::LEFT().get_right_vector().x, hrzn::vector2::UP().x, epsilon, L"Incorrect LEFT X Right vector from angle.");
			Assert::AreEqual(hrzn::angle::LEFT().get_right_vector().y, hrzn::vector2::UP().y, epsilon, L"Incorrect LEFT Y Right vector from angle.");
		}

		TEST_METHOD(VectorsConversion) {

			const h_float epsilon = hrzn::EPSILON;
			//const h_float epsilon = 0.001_hf;
			const hrzn::vector2 test_v = hrzn::vector2::UP();
			const hrzn::angle a1(0.037_hf);

			hrzn::vector2 v = a1.rotate_vector(test_v);
			Assert::AreEqual(a1.get_forward_vector().x, v.x, epsilon, L"Incorrect x-axis during vector rotation.");
			Assert::AreEqual(a1.get_forward_vector().y, v.y, epsilon, L"Incorrect y-axis during vector rotation.");

			const hrzn::angle a2(0.471_hf);

			v = a2.rotate_vector(v);
			Assert::AreEqual((a1 + a2).get_forward_vector().x, v.x, epsilon, L"Incorrect x-axis during second vector rotation.");
			Assert::AreEqual((a1 + a2).get_forward_vector().y, v.y, epsilon, L"Incorrect y-axis during second vector rotation.");


			const hrzn::angle a3(0.635f);
			hrzn::vector2 v_b = a3.rotate_vector(test_v);
			v_b = a3.unrotate_vector(v_b);
			Assert::AreEqual(test_v.x, v_b.x, epsilon, L"Incorrect x-axis during vector inverse rotation.");
			Assert::AreEqual(test_v.y, v_b.y, epsilon, L"Incorrect y-axis during vector inverse rotation.");
		}

	};

}