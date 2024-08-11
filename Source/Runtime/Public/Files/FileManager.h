#pragma once

#include "CoreTypes.h"
#include "Files/FileStream.h"

namespace EE
{
    typedef TList<File> FileList;

    class FileManager
    {
    private:
        static FileList files;

        static FileList::iterator FindInFiles( const WString& filePath );

    public:
        static const File& GetFile( const WString& filePath );

        static WString GetFileExtension( const WString& path );

        static WString GetFullPath( const WString& path );

        static WString GetAppDirectory();

        static WString ReadStream( const File* file );

        static NString ReadNarrowStream( const File* file );

        static TArray<char> ReadBinaryStream( const File* file );
    };
}