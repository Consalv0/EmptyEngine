#pragma once

#include "CoreMinimal.h"

#include "Utils/Memory.h"

namespace EE::Memory
{
    // Wrapper functions for aligned memory allocation
    // There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
    void* AlignedAlloc( size_t size, size_t alignment )
    {
        void* pointer;
#if defined(_MSC_VER)
        pointer = _aligned_malloc( size, alignment );
#elif ((defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200112L)) || defined(__linux__) || defined(__APPLE__))
        if ( ::posix_memalign( &pointer, alignment, size ) ) pointer = nullptr;
#else
        void* p1; // original block
        void** p2; // aligned block
        int offset = alignment - 1 + sizeof( void* );
        if ( (p1 = (void*)malloc( required_bytes + offset )) == NULL )
        {
            return NULL;
        }
        p2 = (void**)(((size_t)(p1)+offset) & ~(alignment - 1));
        p2[ -1 ] = p1;
        return p2;
#endif
        return pointer;
    }

    void AlignedFree( void* pointer )
    {
#if defined(_MSC_VER)
        _aligned_free( pointer );
#elif ((defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200112L)) || defined(__linux__) || defined(__APPLE__))
        ::free( data );
#else
        free( ((void**)p)[ -1 ] );
#endif
    }
}