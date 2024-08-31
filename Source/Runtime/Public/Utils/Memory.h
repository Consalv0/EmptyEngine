
#include <memory>

namespace EE::Memory
{
    // Wrapper functions for aligned memory allocation
    // Free with AlignedFree
    void* AlignedAlloc( size_t size, size_t alignment );

    void AlignedFree( void* pointer );

    template<typename T>
    class AlignedMemory
    {
        EE_CLASSNOCOPY( AlignedMemory )
    private:
        union
        {
            void* pointer;
            T* data;
        };
        size_t alignment;
        size_t count;

    public:
        AlignedMemory() : pointer(NULL), count(), alignment() { }

        AlignedMemory( size_t size, size_t alignment ) : pointer(NULL), count(size), alignment( alignment )
        {
            Allocate();
        }

        ~AlignedMemory()
        {
            Deallocate();
        }

        inline void* GetData() const { return pointer; }

        inline size_t GetAlignment() const { return alignment; }

        inline size_t GetCount() const { return count; }

        inline size_t GetSize() const { return alignment * count; }

        void Resize( size_t newSize, size_t newAlignment )
        {
            if ( count == newSize && alignment == newAlignment )
                return;

            count = newSize;
            alignment = newAlignment;

            if ( pointer != NULL )
                Deallocate();
            
            Allocate();
        }

        T& operator[]( const size_t& index )
        {
            return *(T*)(((uint64)pointer + (index * alignment)));
        }

    private:
        inline void Allocate()
        {
            if ( alignment == sizeof( T ) )
                data = new T[ count ];
            else
                data = static_cast<T*>(AlignedAlloc( count, alignment ));
        }

        inline void Deallocate()
        {
            if ( alignment == sizeof( T ) )
                delete[] data;
            else
                AlignedFree( pointer );
        }
    };
}