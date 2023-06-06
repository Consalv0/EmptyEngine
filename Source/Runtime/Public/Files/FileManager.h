#pragma once

#include "CoreTypes.h"
#include "Files/FileStream.h"

namespace EE
{
    typedef TArray<FileStream*> FileList;

    class FileManager
    {
    private:
        static FileList files;

        static FileList::iterator FindInFiles( const WString& filePath );

    public:
        static FileStream* GetFile( const WString& filePath );

        static FileStream* MakeFile( const WString& filePath );

        static WString GetFileExtension( const WString& path );

        static WString GetFullPath( const WString& path );

        static WString GetAppDirectory();

        //* ReadStreams the file streams of the shader code
        static WString ReadStream( FileStream* stream );
    };
}