#pragma once

#if !defined(EMPTYENGINE_CORE)
#define EMPTYENGINE_CORE
#endif

#ifdef EMPTYENGINE_CORE_LOG
#ifdef EE_ENABLE_ASSERTS
#define EE_ASSERT(X, ...) { if(!(X)) { /* LOG_CRITICAL("Assertion Failed: " __VA_ARGS__); */ __debugbreak(); } }
#define EE_CORE_ASSERT(X, ...) { if(!(X)) { /* LOG_CORE_CRITICAL("Assertion Failed: " __VA_ARGS__); */ __debugbreak(); } }
#else
#define EE_ASSERT(X, ...)
#define EE_CORE_ASSERT(X, ...)
#endif
#endif // EMPTYENGINE_CORE_LOG

// #include "Platform/Platform.h"
#include "CoreTypes.h"

