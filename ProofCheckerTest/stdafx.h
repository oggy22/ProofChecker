// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <set>

extern char const * const file_name;

void create_file(const std::initializer_list<wchar_t*> &lines, char const * const filename = file_name);