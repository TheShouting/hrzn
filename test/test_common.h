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
			template<> inline std::wstring ToString<hrzn::vector2>(const hrzn::vector2& t) { RETURN_WIDE_STRING("vector2{ " << t.x << ", " << t.y << " }"); }
			template<> inline std::wstring ToString<hrzn::point2>(const hrzn::point2& t) { RETURN_WIDE_STRING("point2{ " << t.x << ", " << t.y << " }"); }
			template<> inline std::wstring ToString<hrzn::point_area>(const hrzn::point_area& a) { RETURN_WIDE_STRING("point_area{ " << a.x1 << ", " << a.y1 << ", " << a.x2 << ", " << a.y2 << " }"); }
			template<> inline std::wstring ToString<hrzn::angle>(const hrzn::angle& a) { RETURN_WIDE_STRING("angle{ " << a.deg() << "d }"); }
		}
	}
}

static constexpr auto TEST_EPSILON = hrzn::EPSILON;