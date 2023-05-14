#pragma once

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace EEngine 
{
	typedef char			NChar;
	typedef wchar_t			WChar;
	typedef std::string		NString;
	typedef std::wstring	WString;

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
}

using EEngine::NChar;
using EEngine::WChar;
using EEngine::NString;
using EEngine::WString;

using EEngine::TArray;
using EEngine::TArrayInitializer;
using EEngine::TList;
using EEngine::TQueue;
using EEngine::TDictionary;

#ifdef ES_PLATFORM_CUDA
#include <cuda_runtime.h>
#define HOST_DEVICE __host__ __device__
#else
#define HOST_DEVICE
#endif

#ifdef ES_PLATFORM_APPLE
#define FORCEINLINE inline
#else
#define FORCEINLINE __forceinline
#endif
