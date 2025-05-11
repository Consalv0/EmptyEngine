
// Generic types that could change based on platform
struct EGenericPlatformTypes
{
    // 8-bit unsigned integer
    typedef unsigned char       uint8;
    // 16-bit unsigned integer
    typedef unsigned short int  uint16;
    // 32-bit unsigned integer
    typedef unsigned int        uint32;
    // 64-bit unsigned integer
    typedef unsigned long long  uint64;
    // 8-bit signed integer
    typedef signed char         int8;
    // 16-bit signed integer
    typedef signed short int    int16;
    // 32-bit signed integer
    typedef signed int          int32;
    // 64-bit signed integer
    typedef signed long long    int64;

    // An UTF-8 character. 8-bit representation of Unicode unit char
    typedef char                U8Char;
    // An 8-bit character type. In-memory only. 8-bit representation.
    typedef unsigned char       UChar8;
    // A wide character of different bit sizes depending on platform.
    typedef wchar_t             WChar;
};

#ifdef EE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsPlatformTypes.h"
#endif
#ifdef EE_PLATFORM_WEB
#include "Platform/Web/WebPlatformTypes.h"
#endif

// An UTF-8 character. 8-bit representation of Unicode unit char
typedef EPlatformTypes::U8Char      U8Char;
// A wide character of different bit sizes depending on platform.
typedef EPlatformTypes::WChar       WChar;
// An 8-bit character type. In-memory only. 8-bit representation.
typedef EPlatformTypes::UChar8      UChar8;

// Don't confuse with std::u8string. A UTF-8 string. 8-bit representation of Unicode unit char
typedef std::basic_string<U8Char>   U8String;
typedef std::basic_string<WChar>    WString;

typedef EPlatformTypes::int8    int8;
typedef EPlatformTypes::int16   int16;
typedef EPlatformTypes::int32   int32;
typedef EPlatformTypes::int64   int64;
typedef EPlatformTypes::uint8   uint8;
typedef EPlatformTypes::uint16  uint16;
typedef EPlatformTypes::uint32  uint32;
typedef EPlatformTypes::uint64  uint64;

static_assert(sizeof( int8 ) == 1, "Invalid size of int8");
static_assert(sizeof( int16 ) == 2, "Invalid size of int16");
static_assert(sizeof( int32 ) == 4, "Invalid size of int32");
static_assert(sizeof( int64 ) == 8, "Invalid size of int64");
static_assert(sizeof( uint8 ) == 1, "Invalid size of uint8");
static_assert(sizeof( uint16 ) == 2, "Invalid size of uint16");
static_assert(sizeof( uint32 ) == 4, "Invalid size of uint32");
static_assert(sizeof( uint64 ) == 8, "Invalid size of uint64");