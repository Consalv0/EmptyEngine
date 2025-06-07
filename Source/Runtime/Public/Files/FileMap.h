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
        void* GetHandle() const { return mHandle; }
        uint64 GetSize() const { return mSize; }
        int GetError() const { return mError; }
        uint32 GetRequestsCount() const noexcept { return mRequestCount; }
        uint64 GetBytesRead() const noexcept { return mBytesRead; }
        int GetErrorCode() const noexcept { return mErrorCode; }

    protected:
        uint32  mRequestCount{};
        uint64  mBytesRead{};
        int     mErrorCode{};
        void*   mHandle{};
        uint64  mSize{};
        int     mError{};
        uint64  mOffset{};
        void*   mBuffer{};
    };
}