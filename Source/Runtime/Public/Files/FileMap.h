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
        void* GetHandle() const { return handle_; }
        uint64 GetSize() const { return size_; }
        int GetError() const { return error_; }
        uint32 NumRequests() const noexcept { return requestCount_; }
        uint64 BytesRead() const noexcept { return bytesRead_; }
        int Error() const noexcept { return errorCode_; }

    protected:
        uint32  requestCount_{};
        uint64  bytesRead_{};
        int     errorCode_{};
        void*   handle_{};
        uint64  size_{};
        int     error_{};
        uint64  offset_{};
        void*   buffer_{};
    };
}