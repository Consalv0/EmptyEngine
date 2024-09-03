#pragma once

#include "Core/Collections.h"

namespace EE
{
    class File
    {
    public:
        File( const File& file );
        File( const WString& path );

        ~File();

        virtual bool IsValid() const;

        WString GetExtension() const;
        WString GetFileName() const;
        WString GetFileNameWithoutExtension() const;
        const WString& GetPath() const;
        WString GetShortPath() const;

    protected:
        WString path_;
    };

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