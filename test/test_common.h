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

#include "CppUnitTest.h"

#include "../include/htl/basic_types.h"
#include "../include/htl/containers.h"
#include "../include/htl/utility.h"
#include "../include/htl/stringify.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> inline std::wstring ToString<hrzn::vector2>(const hrzn::vector2& t) { RETURN_WIDE_STRING("vector2{ x:" << t.x << " y:" << t.y << " }"); }
			template<> inline std::wstring ToString<hrzn::point2>(const hrzn::point2& t) { RETURN_WIDE_STRING("point2{ x:" << t.x << " y:" << t.y << " }"); }
			template<> inline std::wstring ToString<hrzn::rect_i>(const hrzn::rect_i& a) { RETURN_WIDE_STRING("rect_i{ x:" << a.x << " y:" << a.y << " w:" << a.w << " h:" << a.h << " }"); }
			template<> inline std::wstring ToString<hrzn::angle>(const hrzn::angle& a) { RETURN_WIDE_STRING("angle{ tau:" << a.tau<< " (" << a.deg() << "deg) }"); }
		}
	}
}

static constexpr auto TEST_EPSILON = hrzn::EPSILON;