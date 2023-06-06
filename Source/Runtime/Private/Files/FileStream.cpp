
#include "CoreMinimal.h"
#include "Files/FileStream.h"
#include "Files/FileManager.h"

#ifdef __APPLE__
#include <unistd.h>
#endif
#include <sstream>
#include <fstream>
#include <iostream>

namespace EE
{
    FileStream::FileStream()
    {
        stream = NULL;
        path = L"";
        length = 0;
    }

    FileStream::FileStream( WString filePath )
    {
#ifdef EE_PLATFORM_WINDOWS
        stream = new std::wfstream( filePath );
#else
        stream = new std::wfstream( Text::WideToNarrow( filePath ) );
#endif

        path = filePath;
        Close();

        if ( !IsValid() )
            EE_LOG_CORE_ERROR( L"File '{}' is not valid or do not exist", filePath );
        else
            LocaleToUTF8( NULL );
    }

    template <typename T>
    inline const std::wistream& FileStream::operator>>( T value )
    {
        return (std::move( *stream ) >> value);
    }

    template <typename T>
    inline const std::wostream& FileStream::operator<<( T value )
    {
        return (std::move( *stream ) << value);
    }

    inline float FileStream::GetProgress() const
    {
        long progress = long( stream->tellg() );
        return progress / float( length );
    }

    inline long FileStream::GetPosition() const
    {
        return (long)stream->tellg();
    }

    WString FileStream::GetExtension() const
    {
        WString::size_type extensionIndex;

        extensionIndex = path.rfind( '.' );

        if ( extensionIndex != WString::npos )
        {
            return path.substr( extensionIndex + 1 );
        }
        else
        {
            return L"";
        }
    }

    WString FileStream::GetFileName() const
    {
        WChar separator = L'/';

#ifdef EE_PLATFORM_WINDOWS
        separator = L'\\';
#endif

        size_t i = path.rfind( separator, path.length() );
        if ( i != WString::npos )
        {
            return(path.substr( i + 1, path.length() - i ));
        }

        return L"";
    }

    WString FileStream::GetFileNameWithoutExtension() const
    {
        WString fileName = GetFileName();
        WString extension = GetExtension();
        fileName = fileName.substr( 0, fileName.size() - extension.size() - 1 );
        return fileName;
    }

    WString FileStream::GetPath() const
    {
        return path;
    }

    WString FileStream::GetShortPath() const
    {
        WString currentDirectory = FileManager::GetAppDirectory();
        WString returnValue = path;
        Text::Replace( returnValue, currentDirectory, L".." );

        return returnValue;
    }

    std::wstringstream FileStream::ReadStream() const
    {
        std::wstringstream stringStream;
        if ( IsValid() )
        {
            try
            {
                stringStream << stream->rdbuf();
            }
            catch ( ... ) {}
        }
        else
        {
            EE_LOG_CORE_ERROR( L"File '{}' is not valid or do not exist", path );
        }

        return stringStream;
    }

    bool FileStream::ReadNarrowStream( NString* output ) const
    {
        if ( IsValid() )
        {
            try
            {
                std::fstream narrowStream;
#ifdef EE_PLATFORM_WINDOWS
                narrowStream.open( path, std::ios::in );
#else
                narrowStream.open( Text::WideToNarrow( path ), std::ios::in | std::ios::binary );
#endif
                narrowStream.seekg( 0, std::ios::end );
                output->resize( narrowStream.tellg() );
                narrowStream.seekg( 0, std::ios::beg );
                narrowStream.read( &(*output)[ 0 ], output->size() );
                narrowStream.close();
            }
            catch ( ... )
            {
                return false;
            }
        }
        else
        {
            EE_LOG_CORE_ERROR( L"File '{}' is not valid or do not exist", path );
            return false;
        }

        return true;
    }

    WString FileStream::GetLine()
    {
        WString string;
        std::getline( *stream, string );
        return string;
    }

    bool FileStream::IsValid() const
    {
        return stream != NULL && !stream->fail() && stream->good();
    }

    void FileStream::LocaleToUTF8( const NChar* localeFormat ) const
    {
        if ( localeFormat == NULL )
            localeFormat = "en_US.UTF-8";
        static std::locale locale( localeFormat );
        stream->imbue( locale );
    }

    long FileStream::GetLenght()
    {
        return length;
    }

    bool FileStream::Open()
    {
#ifdef EE_PLATFORM_WINDOWS
        stream->open( path, std::ios::in | std::ios::out );
#else
        stream->open( Text::WideToNarrow( path ), std::ios::in );
#endif
        if ( stream->is_open() )
            Reset();

        return stream->is_open();
    }

    void FileStream::Clean()
    {
        if ( stream->is_open() )
            stream->close();
#ifdef EE_PLATFORM_WINDOWS
        stream->open( path, std::ios::in | std::ios::out | std::ios::trunc );
#else
        stream->open( Text::WideToNarrow( path ), std::ios::in | std::ios::out | std::ios::trunc );
#endif
    }

    void FileStream::Reset()
    {
        stream->clear();
        stream->seekg( 0, stream->end );
        length = long( stream->tellg() );
        stream->seekg( 0, stream->beg );
    }

    void FileStream::Close()
    {
        stream->close();
    }
}