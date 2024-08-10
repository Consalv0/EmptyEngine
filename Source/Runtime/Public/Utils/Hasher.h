#pragma once

#include "CoreTypes.h"

namespace EE
{
	inline void HashCombine(std::size_t& seed) { }

	template <typename T, typename... Rest>
	inline void HashCombine(std::size_t& seed, const T& v, Rest... rest) {
		std::hash<T> Hasher;
		seed ^= Hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		HashCombine(seed, rest...);
	}

	inline size_t WStringToHash(const WString & name) {
		static const std::hash<WString> Hasher;
		return Hasher(name);
	}

	// Code taken from https://stackoverflow.com/a/28801005 by tux3
	// Generate CRC lookup table
	template <unsigned C, int K = 8>
	struct GenCRCTable : GenCRCTable<((C & 1) ? 0xedb88320 : 0) ^ (C >> 1), K - 1> {};
	template <unsigned C> struct GenCRCTable<C, 0> { enum { value = C }; };

#define HASH_A(x) HASH_B(x) HASH_B(x + 128)
#define HASH_B(x) HASH_C(x) HASH_C(x +  64)
#define HASH_C(x) HASH_D(x) HASH_D(x +  32)
#define HASH_D(x) HASH_E(x) HASH_E(x +  16)
#define HASH_E(x) HASH_F(x) HASH_F(x +   8)
#define HASH_F(x) HASH_G(x) HASH_G(x +   4)
#define HASH_G(x) HASH_H(x) HASH_H(x +   2)
#define HASH_H(x) HASH_I(x) HASH_I(x +   1)
#define HASH_I(x) GenCRCTable<x>::value,

	constexpr unsigned CRCTable[] = { HASH_A(0) };

	// Constexpr implementation and helpers
	constexpr uint32 CRC32Implementation(const uint8_t* p, size_t len, uint32 crc) {
		return len ?
			CRC32Implementation(p + 1, len - 1, (crc >> 8) ^ CRCTable[(crc & 0xFF) ^ *p])
			: crc;
	}

	constexpr uint32 EncodeCRC32(const uint8_t* data, size_t length) {
		return ~CRC32Implementation(data, length, ~0);
	}

	constexpr size_t strlen_c(const char* str) {
		return *str ? 1 + strlen_c(str + 1) : 0;
	}

	constexpr int WSID(const char* str) {
		return EncodeCRC32((uint8_t*)str, strlen_c(str));
	}

}

#define EE_MAKE_HASHABLE(type, ...) \
namespace std {\
    template<> struct hash<type> {\
        std::size_t operator()(const type &t) const {\
            std::size_t ret = 0;\
            EE::HashCombine(ret, __VA_ARGS__);\
            return ret;\
        }\
    };\
}