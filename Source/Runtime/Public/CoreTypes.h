#pragma once

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

typedef char            NChar;
typedef wchar_t         WChar;
typedef std::string     NString;
typedef std::wstring    WString;

template<class T>
using TArray = std::vector<T>;
template<class T>
using TArrayInitializer = std::initializer_list<T>;
template<class T>
using TList = std::list<T>;
template<class T>
using TQueue = std::queue<T>;
template<class K, class T>
using TMap = std::unordered_map<K, T>;

typedef int8_t          int8;
typedef int16_t         int16;
typedef int32_t         int32;
typedef int64_t         int64;
typedef uint8_t         uint8;
typedef uint16_t        uint16;
typedef uint32_t        uint32;
typedef uint64_t        uint64;

typedef size_t          intPNT;

#ifdef EE_PLATFORM_CUDA
#include <cuda_runtime.h>
#define HOST_DEVICE __host__ __device__
#else
#define HOST_DEVICE
#endif

#if defined(_MSC_VER)
#	define FORCENOINLINE	__declspec(noinline)
#	define FORCEINLINE		__forceinline
#else
#	define FORCENOINLINE	inline __attribute__((noinline))
#	define FORCEINLINE		inline __attribute__((always_inline))
#endif

#if defined(_MSC_VER)
#define EE_DLLEXPORT __declspec(dllexport)
#define EE_DLLIMPORT __declspec(dllimport)
#else
#define EE_DLLEXPORT __attribute__((visibility("default")))
#define EE_DLLIMPORT __attribute__((visibility("default")))
#endif

#define EE_ARRAYSIZE( array ) (sizeof( array ) / sizeof( array[ 0 ] ))

#define ENUM_FLAGS_OPERATORS( Enum ) \
    inline           Enum& operator|=(Enum& lft, Enum rgt) { return lft = (Enum)((__underlying_type(Enum))lft | (__underlying_type(Enum))rgt); } \
    inline           Enum& operator&=(Enum& lft, Enum rgt) { return lft = (Enum)((__underlying_type(Enum))lft & (__underlying_type(Enum))rgt); } \
    inline           Enum& operator^=(Enum& lft, Enum rgt) { return lft = (Enum)((__underlying_type(Enum))lft ^ (__underlying_type(Enum))rgt); } \
    inline constexpr Enum  operator| (Enum  lft, Enum rgt) { return (Enum)((__underlying_type(Enum))lft | (__underlying_type(Enum))rgt); } \
    inline constexpr Enum  operator& (Enum  lft, Enum rgt) { return (Enum)((__underlying_type(Enum))lft & (__underlying_type(Enum))rgt); } \
    inline constexpr Enum  operator^ (Enum  lft, Enum rgt) { return (Enum)((__underlying_type(Enum))lft ^ (__underlying_type(Enum))rgt); } \
    inline constexpr bool  operator! (Enum  e)             { return !(__underlying_type(Enum))e; } \
    inline constexpr Enum  operator~ (Enum  e)             { return (Enum)~(__underlying_type(Enum))e; }


#define EE_CLASSNOCOPY( clz ) \
    clz(clz&) = delete;   \
    clz& operator=(clz&) = delete; \
    clz(const clz&) = delete; \
    clz& operator=(const clz&) = delete;