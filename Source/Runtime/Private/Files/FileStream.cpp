
#include "CoreMinimal.h"

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
        : File( file.path )
    {
    }

    File::File( const WString& path )
        : path( path )
    {
    }

    File::~File()
    {
    }

    bool File::IsValid() const
    {
        return path.empty() == false;
    }

    WString File::GetExtension() const
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

    WString File::GetFileName() const
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

    WString File::GetFileNameWithoutExtension() const
    {
        WString fileName = GetFileName();
        WString extension = GetExtension();
        fileName = fileName.substr( 0, fileName.size() - extension.size() - 1 );
        return fileName;
    }

    WString File::GetPath() const
    {
        return path;
    }

    WString File::GetShortPath() const
    {
        WString currentDirectory = FileManager::GetAppDirectory();
        WString returnValue = path;
        Text::Replace( returnValue, currentDirectory, L".." );

        return returnValue;
    }
}