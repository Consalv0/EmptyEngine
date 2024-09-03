
#include "CoreMinimal.h"

#include "Files/FileMap.h"

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
#elif defined(__APPLE__ || __linux__)
        int handlePtr = (int)(uintptr_t)(*handle);
        if ( handlePtr != -1 )
        {
            close( handlePtr );
        }
#endif
    }

    FORCEINLINE int OpenFileMap( const File& filePath, void** handle, size_t* outSize )
    {
#ifdef _WIN32
        *handle = new FileHandle();
        FileHandle* fileHandle = static_cast<FileHandle*>( *handle );
        fileHandle->file = CreateFileW(
            filePath.GetPath().c_str(),
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

        *outSize = static_cast<size_t>(size.QuadPart);
#elif defined(__APPLE__ || __linux__)
        NString path = Text::WideToNarrow( filePath.GetPath() );

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
#elif defined(__APPLE__ || __linux__)
        int handlePtr = (int)(uintptr_t)(handle);
        return handlePtr != -1;
#endif
    }

    int FileMap::ReadBlock( size_t offset, size_t size, NChar* buffer )
    {
#ifdef _WIN32
        EE_ASSERT( FileMapHandleValid( handle_ ) );

        ++requestCount_;

        FileHandle* fileHandle = static_cast<FileHandle*>(handle_);
        fileHandle->overlapped.hEvent = fileHandle->readEvent;
        fileHandle->overlapped.Offset = static_cast<DWORD>(offset);
        fileHandle->overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

        offset_ = static_cast<DWORD>(offset);
        size_ = static_cast<DWORD>(size);
        buffer_ = buffer;
        bool success = ReadFile( fileHandle->file, buffer_, static_cast<DWORD>(size), nullptr, &fileHandle->overlapped );

        if ( !success )
        {
            if ( DWORD ec = GetLastError(); ec != ERROR_IO_PENDING )
            {
                return error_ = static_cast<int>(ec);
            }
        }

        return 0;
#elif defined( __linux__ )
        EE_ASSERT( buffer_ );
        EE_ASSERT( FileMapHandleValid( handle_ ) );
        EE_ASSERT( std::uintptr_t( buffer_ ) % 4096 == 0 );

        ++requestCount_;

        offset_ = offset;
        size_ = size;
        buffer_ = buffer;

        auto result = readahead( (int)(uintptr_t)handle_, offset_, size_ );
        if ( result == -1 )
        {
            return error_ = errno;
        }
        return 0;
#elif defined( __APPLE__ )
        EE_ASSERT( buffer_ );
        EE_ASSERT( FileMapHandleValid( handle_ ) );
        EE_ASSERT( std::uintptr_t( buffer_ ) % 4096 == 0 );

        ++m_num_requests;

        offset_ = offset;
        size_ = size;
        buffer_ = buffer;

        radvisory args{};
        args.ra_offset = static_cast<off_t>(offset_);
        args.ra_count = static_cast<int>(size_);

        auto result = fcntl( (int)(uintptr_t)handle_, F_RDADVISE, &args );
        if ( result == -1 )
        {
            return error_ = errno;
        }
        return 0;
#endif
    }

    int FileMap::WaitForResult( size_t* outBytesRead )
    {
#ifdef _WIN32
        EE_ASSERT( FileMapHandleValid( handle_ ) );

        FileHandle* fileHandle = static_cast<FileHandle*>(handle_);

        DWORD bytesRead = DWORD{};
        bool success = GetOverlappedResult( handle_, &fileHandle->overlapped, &bytesRead, TRUE );

        if ( !success )
        {
            if ( DWORD ec = GetLastError(); ec != ERROR_HANDLE_EOF )
            {
                *outBytesRead = 0;
                return error_ = static_cast<int>(ec);
            }
        }

        bytesRead_ += bytesRead;
        *outBytesRead = bytesRead;

        return 0;
#elif defined( __linux__ )
        auto result = pread64( (int)(uintptr_t)handle_, buffer_, size_, offset_ );

        if ( result < 0 )
        {
            *outBytesRead = 0;
            return error_ = errno;
        }

        auto bytesRead = static_cast<size_t>(result);

        bytesRead_ += bytesRead;
        *outBytesRead = bytesRead;

        return 0;
#elif defined( __APPLE__ )
        auto result = pread( (int)(uintptr_t)handle_, m_buffer, m_size, m_offset );

        if ( result < 0 )
        {
            *outBytesRead = 0;
            return error_ = errno;
        }

        const size_t& bytesRead = static_cast<size_t>(result);

        bytesRead_ += bytesRead;
        *outBytesRead = bytesRead;

        return 0;
#endif
    }

    FileMap::FileMap( const File& filePath ) : File( filePath ),
        handle_{}, size_{}, error_{}
    {
        error_ = OpenFileMap( filePath, &handle_, &size_ );
    }

    FileMap::~FileMap() noexcept
    {
        CloseFileMap( &handle_ );
    }

    FileMap::operator bool() const { return FileMapHandleValid( handle_ ); }
}