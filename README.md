# HRZN Template Library

***Note:** This library is under active development and may undergo significant changes.*

HRZN is a C++ header-only utility library designed for managing vectors, grids, and other 2D coordinates. Additionally, it offers various containers and functions utilizing these types. This is a personal project meant as a learing experience for myself. I am not a professional programmer and I created this as a hobbiest. The library was initially created to support a personal project named Horizon, from which the hrzn namespace was derived.

## Project Structure

Only the header files located in the "include" folder are required for using HRZN. All headers depend on basic_types.h which is included in each of them. The Visual Studio project and main.cpp are not essential components of the library as they serve as minimal examples and for unit testing.

The HRZN Template Library is organized as follows:

```
include/
    hrzn/
        lib/
            basic_types.h 
            containers.h
            utility.h
            geometry.h
            stringify.h
```

Several methods exist within stringify.h meant to simplify displaying 2d maps to the console window.

## Usage

A minimum of C++17 is required to use HRZN.

Before including any HRZN headers, there are a few user-defined options that may be set:

- **HRZN_NOEXCEPTIONS**: Define this macro to disable the throwing of exceptions (optional).
- **HRZN_USE_DOUBLE_TYPE**: Define this macro to use doubles instead of floats (optional).
- **HRZN_USE_LONG_TYPE**: Define this macro to use long instead of int  (optional).

## Example 

```
#define HRZN_NOEXCEPTIONS
#include <htl/basic_types.h>
#include <htl/containers.h>
#include <iostream>

using namespace hrzn;

int main() {

    point2 location{ 11, 17 };

    MapContainer<std::string> treasure_map(50, 50);

    treasure_map[location] = "X marks the spot!";
    treasure_map.at(37, 5) = "The treasure is not here.";

    std::cout << treasure_map[location] << std::endl;
}
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.