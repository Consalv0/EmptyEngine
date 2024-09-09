#pragma once

#include <string>

typedef char            NChar;
typedef wchar_t         WChar;
typedef std::string     NString;
typedef std::wstring    WString;

typedef int8_t          int8;
typedef int16_t         int16;
typedef int32_t         int32;
typedef int64_t         int64;
typedef uint8_t         uint8;
typedef uint16_t        uint16;
typedef uint32_t        uint32;
typedef uint64_t        uint64;

static_assert(sizeof( int8 ) == 1, "Invalid size of int8");
static_assert(sizeof( int16 ) == 2, "Invalid size of int16");
static_assert(sizeof( int32 ) == 4, "Invalid size of int32");
static_assert(sizeof( int64 ) == 8, "Invalid size of int64");
static_assert(sizeof( uint8 ) == 1, "Invalid size of uint8");
static_assert(sizeof( uint16 ) == 2, "Invalid size of uint16");
static_assert(sizeof( uint32 ) == 4, "Invalid size of uint32");
static_assert(sizeof( uint64 ) == 8, "Invalid size of uint64");