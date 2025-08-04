#pragma once

// Yes, we do define these types just to remove the "_t" from stdint.h... get over it.

//--------------------------------- Unsigned base types

// 8-bit unsigned integer
using uint8  = unsigned char;

// 16-bit unsigned integer
using uint16 = unsigned short int;

// 32-bit unsigned integer
using uint32 = unsigned int;

// 64-bit unsigned integer
using uint64 = unsigned long long;

// 64-bit pointer.
using UINTPTR = uint64;

//--------------------------------- Signed base types

// 8-bit signed integer
using int8   = signed char;

// 16-bit signed integer
using int16  = signed short int;

// 32-bit signed integer
using int32  = signed int;

// 64-bit signed integer
using int64  = signed long long;

// Size type. Leave _t to not interfere with SIZE in windef.h
using SIZE_T = size_t;

//--------------------------------- Character types

// An ANSI character. 8-bit fixed-width representation of 7-bit characters.
using ANSICHAR = char;

// A wide character. In-memory only. ?-bit fixed-width representation of the platform's natural wide character set.
using WIDECHAR = wchar_t;

// A switchable character. In-memory only. Either ANSICHAR or WIDECHAR.
using TCHAR = WIDECHAR;
