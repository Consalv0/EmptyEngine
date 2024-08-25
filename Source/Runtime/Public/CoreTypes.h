#pragma once

#include <string>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iterator>

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