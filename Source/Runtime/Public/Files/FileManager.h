#pragma once

#include "Core/Collections.h"

namespace EE
{
    class File
    {
    public:
        File( const File& file );
        File( const U8String& path );
        File( const std::u8string& path );

        ~File();

        virtual bool IsValid() const;

        U8String GetExtension() const;
        U8String GetFileName() const;
        U8String GetFileNameWithoutExtension() const;
        const U8String& GetPath() const;
        U8String GetShortPath() const;

    protected:
        U8String mPath;
    };

    typedef TList<File> FileList;

    class FileManager
    {
    private:
        static FileList files;

        static FileList::iterator FindInFiles( const U8String& filePath );

    public:
        static const File& GetFile( const U8String& filePath );

        static U8String GetFileExtension( const U8String& path );

        static U8String GetFullPath( const U8String& path );

        static U8String GetAppDirectory();

        static U8String ReadStream( const File* file );

        static TArray<char> ReadBinaryStream( const File* file );
    };
}