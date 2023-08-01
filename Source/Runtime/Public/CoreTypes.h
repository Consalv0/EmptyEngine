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
using TDictionary = std::unordered_map<K, T>;

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
