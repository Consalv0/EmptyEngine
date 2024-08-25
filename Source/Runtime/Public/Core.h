#pragma once

#define EE_LOG_LEVEL_DEBUG_BIT      (1 << 0)
#define EE_LOG_LEVEL_INFO_BIT       (1 << 1)
#define EE_LOG_LEVEL_WARN_BIT       (1 << 2)
#define EE_LOG_LEVEL_ERROR_BIT      (1 << 3)
#define EE_LOG_LEVEL_CRITICAL_BIT   (1 << 4)
#define EE_LOG_LEVEL_OFF 0

#if !defined(EMPTYENGINE_CORE)
#define EMPTYENGINE_CORE
#else
#define EMPTYENGINE_CORE_LOG
#endif

#include "CoreTypes.h"

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