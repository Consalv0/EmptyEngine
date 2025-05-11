
#include "CoreMinimal.h"

#include "Utils/TextFormatting.h"
#include "Core/Collections.h"
#include "Files/FileStream.h"
#include "Files/FileManager.h"

#ifdef __APPLE__
#include <unistd.h>
#endif
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>

namespace EE
{
    File::File( const File& file )
        : File( file.path_ )
    {
    }

    File::File( const U8String& path )
        : path_( path )
    {
    }

    File::File( const std::u8string& path )
        : path_( (U8Char*)path.c_str() )
    {
    }

    File::~File()
    {
    }

    bool File::IsValid() const
    {
        return path_.empty() == false;
    }

    U8String File::GetExtension() const
    {
        U8String::size_type extensionIndex;

        extensionIndex = path_.rfind( '.' );

        if ( extensionIndex != U8String::npos )
        {
            return path_.substr( extensionIndex + 1 );
        }
        else
        {
            return "";
        }
    }

    U8String File::GetFileName() const
    {
        U8Char separator = '/';

#ifdef EE_PLATFORM_WINDOWS
        separator = '\\';
#endif

        size_t i = path_.rfind( separator, path_.length() );
        if ( i != U8String::npos )
        {
            return(path_.substr( i + 1, path_.length() - i ));
        }

        return "";
    }

    U8String File::GetFileNameWithoutExtension() const
    {
        U8String fileName = GetFileName();
        U8String extension = GetExtension();
        fileName = fileName.substr( 0, fileName.size() - extension.size() - 1 );
        return fileName;
    }

    const U8String& File::GetPath() const
    {
        return path_;
    }

    U8String File::GetShortPath() const
    {
        U8String currentDirectory = FileManager::GetAppDirectory();
        U8String returnValue = path_;
        Text::Replace( returnValue, currentDirectory, ".." );

        return returnValue;
    }
}