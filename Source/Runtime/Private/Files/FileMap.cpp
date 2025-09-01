
#include "CoreMinimal.h"

#include "Utils/TextFormatting.h"
#include "Files/FileMap.h"

#include "Platform/PrePlatform.h"
#include "Platform/Platform.h"

namespace EE
{
#ifdef _WIN32
    struct FileHandle
    {
        HANDLE file;
        HANDLE readEvent;
        OVERLAPPED overlapped;
    };
#endif

    FORCEINLINE void CloseFileMap( void** handle )
    {
#ifdef _WIN32
        FileHandle* fileHandle = static_cast<FileHandle*>( *handle );
        if ( fileHandle->file != INVALID_HANDLE_VALUE )
        {
            CloseHandle( fileHandle->file );
            CloseHandle( fileHandle->readEvent );
            delete fileHandle;
        }
#elif defined(__APPLE__) || defined(__linux__)
        int handlePtr = (int)(uintptr_t)(*handle);
        if ( handlePtr != -1 )
        {
            close( handlePtr );
        }
#endif
    }

    FORCEINLINE int OpenFileMap( const File& filePath, void** handle, uint64* outSize )
    {
#ifdef _WIN32
        *handle = new FileHandle();
        FileHandle* fileHandle = static_cast<FileHandle*>( *handle );
        WString pathW = Text::UTF8ToWide( filePath.GetPath().c_str() );
        fileHandle->file = CreateFileW(
            pathW.c_str(),
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
            nullptr );

        if ( fileHandle->file == INVALID_HANDLE_VALUE )
        {
            return static_cast<int>(GetLastError());
        }

        LARGE_INTEGER size{};
        if ( !GetFileSizeEx( fileHandle->file, &size ) )
        {
            int error = static_cast<int>(GetLastError());
            CloseFileMap( handle );
            return error;
        }

        fileHandle->readEvent = CreateEventA( nullptr, FALSE, FALSE, nullptr );

        if ( fileHandle->readEvent == INVALID_HANDLE_VALUE )
        {
            return static_cast<int>(GetLastError());
        }

        *outSize = size.QuadPart;
#elif defined(__APPLE__) || defined(__linux__)
        const U8String& path = filePath.GetPath();

        struct stat info;
        if ( -1 == stat( path.c_str(), &info ) )
        {
            return errno;
        }

        *outSize = info.st_size;

        int handlePtr = open( path.c_str(), O_RDONLY );

        if ( -1 == handlePtr )
        {
            return errno;
        }

        *handle = (void*)(uintptr_t)(handlePtr);
#endif
        return 0;
    }

    FORCEINLINE bool FileMapHandleValid( void* handle )
    {
#ifdef _WIN32
        FileHandle* fileHandle = static_cast<FileHandle*>( handle );
        return fileHandle->file != nullptr && fileHandle->file != INVALID_HANDLE_VALUE &&
               fileHandle->readEvent != nullptr && fileHandle->readEvent != INVALID_HANDLE_VALUE;
#elif defined(__APPLE__) || defined(__linux__)
        int handlePtr = (int)(uintptr_t)(handle);
        return handlePtr != -1;
#endif
        return false;
    }

    int FileMap::ReadBlock( uint64 offset, uint64 size, void* buffer )
    {
#ifdef _WIN32
        EE_ASSERT( FileMapHandleValid( _handle ) );

        ++_requestCount;

        FileHandle* fileHandle = static_cast<FileHandle*>(_handle);
        fileHandle->overlapped.hEvent = fileHandle->readEvent;
        fileHandle->overlapped.Offset = static_cast<DWORD>(offset);
        fileHandle->overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

        _offset = static_cast<DWORD>(offset);
        _size = static_cast<DWORD>(size);
        _buffer = buffer;
        bool success = ReadFile( fileHandle->file, _buffer, static_cast<DWORD>(size), nullptr, &fileHandle->overlapped );

        if ( !success )
        {
            if ( DWORD ec = GetLastError(); ec != ERROR_IO_PENDING )
            {
                return _error = static_cast<int>(ec);
            }
        }
#elif defined( __linux__ )
        EE_ASSERT( _buffer );
        EE_ASSERT( FileMapHandleValid( _handle ) );
        EE_ASSERT( std::uintptr_t( _buffer ) % 4096 == 0 );

        ++_requestCount;

        _offset = offset;
        _size = size;
        _buffer = buffer;

        auto result = readahead( (int)(uintptr_t)_handle, _offset, _size );
        if ( result == -1 )
        {
            return _error = errno;
        }
#elif defined( __APPLE__ )
        EE_ASSERT( _buffer );
        EE_ASSERT( FileMapHandleValid( _handle ) );
        EE_ASSERT( std::uintptr_t( _buffer ) % 4096 == 0 );

        ++m_num_requests;

        _offset = offset;
        _size = size;
        _buffer = buffer;

        radvisory args{};
        args.ra_offset = static_cast<off_t>(_offset);
        args.ra_count = static_cast<int>(_size);

        auto result = fcntl( (int)(uintptr_t)_handle, F_RDADVISE, &args );
        if ( result == -1 )
        {
            return _error = errno;
        }
#endif

        return 0;
    }

    int FileMap::WaitForResult( uint64* outBytesRead )
    {
#ifdef _WIN32
        EE_ASSERT( FileMapHandleValid( _handle ) );

        FileHandle* fileHandle = static_cast<FileHandle*>(_handle);

        DWORD bytesRead = DWORD{};
        bool success = GetOverlappedResult( _handle, &fileHandle->overlapped, &bytesRead, TRUE );

        if ( !success )
        {
            if ( DWORD ec = GetLastError(); ec != ERROR_HANDLE_EOF )
            {
                *outBytesRead = 0;
                return _error = static_cast<int>(ec);
            }
        }

        _bytesRead += bytesRead;
        *outBytesRead = bytesRead;
#elif defined( __linux__ )
        auto result = pread64( (int)(uintptr_t)_handle, _buffer, _size, _offset );

        if ( result < 0 )
        {
            *outBytesRead = 0;
            return _error = errno;
        }

        auto bytesRead = static_cast<size_t>(result);

        _bytesRead += bytesRead;
        *outBytesRead = bytesRead;
#elif defined( __APPLE__ )
        auto result = pread( (int)(uintptr_t)_handle, _buffer, _size, _offset );

        if ( result < 0 )
        {
            *outBytesRead = 0;
            return _error = errno;
        }

        const size_t& bytesRead = static_cast<size_t>(result);

        _bytesRead += bytesRead;
        *outBytesRead = bytesRead;
#endif

        return 0;
    }

    FileMap::FileMap( const File& filePath ) : File( filePath ),
        _handle{}, _size{}, _error{}
    {
        _error = OpenFileMap( filePath, &_handle, &_size );
    }

    FileMap::~FileMap() noexcept
    {
        CloseFileMap( &_handle );
    }

    FileMap::operator bool() const { return FileMapHandleValid( _handle ); }
}

#include "Platform/PostPlatform.h"