#pragma once

namespace EE
{
    inline void HashCombine( uint64* seed ) {}

    template <typename T, typename... Rest>
    inline void HashCombine( uint64* seed, const T& v, Rest... rest )
    {
        std::hash<T> hasher;
        *seed ^= hasher( v ) + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
        HashCombine( seed, rest... );
    }

    inline uint64 ConstU8StringToHash( const U8Char*& name )
    {
        static const std::hash<std::string_view> hasher;
        return hasher( std::string_view( name, std::strlen( name ) ) );
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

    constexpr unsigned CRCTable[] = { HASH_A( 0 ) };

    // Constexpr implementation and helpers
    constexpr uint32 CRC32Implementation( const uint8* p, uint64 len, uint32 crc )
    {
        return len ?
            CRC32Implementation( p + 1, len - 1, (crc >> 8) ^ CRCTable[ (crc & 0xFF) ^ *p ] )
            : crc;
    }

    constexpr uint32 EncodeCRC32( const uint8* data, uint64 length )
    {
        return ~CRC32Implementation( data, length, ~0 );
    }

    constexpr uint64 strlen_c( const U8Char* str )
    {
        return *str ? 1 + strlen_c( str + 1 ) : 0;
    }

    constexpr int WSID( const U8Char* str )
    {
        return EncodeCRC32( (uint8*)str, strlen_c( str ) );
    }

}

#define EE_MAKE_HASHABLE(type, ...) \
namespace std {\
    template<> struct hash<type> {\
        uint64 operator()(const type &t) const {\
            uint64 ret = 0;\
            EE::HashCombine(&ret, __VA_ARGS__);\
            return ret;\
        }\
    };\
}