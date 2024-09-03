
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

    File::File( const WString& path )
        : path_( path )
    {
    }

    File::~File()
    {
    }

    bool File::IsValid() const
    {
        return path_.empty() == false;
    }

    WString File::GetExtension() const
    {
        WString::size_type extensionIndex;

        extensionIndex = path_.rfind( '.' );

        if ( extensionIndex != WString::npos )
        {
            return path_.substr( extensionIndex + 1 );
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

        size_t i = path_.rfind( separator, path_.length() );
        if ( i != WString::npos )
        {
            return(path_.substr( i + 1, path_.length() - i ));
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

    const WString& File::GetPath() const
    {
        return path_;
    }

    WString File::GetShortPath() const
    {
        WString currentDirectory = FileManager::GetAppDirectory();
        WString returnValue = path_;
        Text::Replace( returnValue, currentDirectory, L".." );

        return returnValue;
    }
}