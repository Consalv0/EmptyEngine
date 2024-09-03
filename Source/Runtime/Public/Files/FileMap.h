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

        int ReadBlock( size_t offset, size_t size, NChar* buffer );
        int WaitForResult( size_t* bytesRead );

        explicit operator bool() const;
        void* GetHandle() const { return handle_; }
        size_t GetSize() const { return size_; }
        int GetError() const { return error_; }
        uint32 NumRequests() const noexcept { return requestCount_; }
        size_t BytesRead() const noexcept { return bytesRead_; }
        int Error() const noexcept { return errorCode_; }

    protected:
        uint32  requestCount_{};
        size_t  bytesRead_{};
        int     errorCode_{};
        void*   handle_{};
        size_t  size_{};
        int     error_{};
        NChar*  buffer_{};
        size_t  offset_{};
    };
}