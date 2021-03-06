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
 
 Notes:
*	This visual studio project and main.cpp is not required for use of this library but is provided
*	as an example of library usage. This is a header only library. Place the folder [hrzn/] in your include
*	directory to use.
*/
#include "htl/hrzn.h"
#include "htl/stringify.h"
#include "htl/utility.h"

#include <iostream>

int main() {

	hrzn::HMap<char> map(20,20);
	map.fill('.');
	hrzn::scatter(map, ':', 0.9);
	map.at(3, 3) = 'X';

	std::cout << hrzn::toStringTable(map) << "\n";

}
