#pragma once

#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iterator>

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