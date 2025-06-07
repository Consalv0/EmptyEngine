
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
        : File( file.mPath )
    {
    }

    File::File( const U8String& path )
        : mPath( path )
    {
    }

    File::File( const std::u8string& path )
        : mPath( (U8Char*)path.c_str() )
    {
    }

    File::~File()
    {
    }

    bool File::IsValid() const
    {
        return mPath.empty() == false;
    }

    U8String File::GetExtension() const
    {
        U8String::size_type extensionIndex;

        extensionIndex = mPath.rfind( '.' );

        if ( extensionIndex != U8String::npos )
        {
            return mPath.substr( extensionIndex + 1 );
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

        size_t i = mPath.rfind( separator, mPath.length() );
        if ( i != U8String::npos )
        {
            return(mPath.substr( i + 1, mPath.length() - i ));
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
        return mPath;
    }

    U8String File::GetShortPath() const
    {
        U8String currentDirectory = FileManager::GetAppDirectory();
        U8String returnValue = mPath;
        Text::Replace( returnValue, currentDirectory, ".." );

        return returnValue;
    }
}