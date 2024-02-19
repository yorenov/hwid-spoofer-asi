#pragma once

#define xstr(x) _str(x)
#define _str(x) #x

#define finline __forceinline
#define toggle(var) var = !var

#ifdef _DEBUG

#include <cassert>

#define dbreak              __debugbreak

#define debugHW(header, fmt, ...)    printf_s("[" PROJECT_NAME_SHORT "/" header "] " fmt,       __VA_ARGS__)
#define debugHWL(header, fmt, ...)   printf_s("[" PROJECT_NAME_SHORT "/" header "] " fmt "\n",  __VA_ARGS__)

#define debugW(fmt, ...)    debugHW ("MAIN", fmt, __VA_ARGS__)
#define debugWL(fmt, ...)   debugHWL("MAIN", fmt, __VA_ARGS__)

#define weak_assert         assert

#else

#define dbreak std::terminate

#define debugHW(...)
#define debugHWL(...)

#define debugW(...)
#define debugWL(...)

#define weak_assert(...)

#endif
