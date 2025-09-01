#pragma once

#include "Files/FileManager.h"

namespace EE
{
    class FileMap : public File
    {
    private:
        EE_CLASSNOCOPY( FileMap )

    public:
        FileMap( const File& filePath );
        ~FileMap() noexcept;

        int ReadBlock( uint64 offset, uint64 size, void* buffer );
        int WaitForResult( uint64* bytesRead );

        explicit operator bool() const;
        void* GetHandle() const { return _handle; }
        uint64 GetSize() const { return _size; }
        int GetError() const { return _error; }
        uint32 GetRequestsCount() const noexcept { return _requestCount; }
        uint64 GetBytesRead() const noexcept { return _bytesRead; }
        int GetErrorCode() const noexcept { return _errorCode; }

    protected:
        uint32  _requestCount{};
        uint64  _bytesRead{};
        int     _errorCode{};
        void*   _handle{};
        uint64  _size{};
        int     _error{};
        uint64  _offset{};
        void*   _buffer{};
    };
}