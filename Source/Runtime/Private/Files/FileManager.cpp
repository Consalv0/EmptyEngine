
#ifdef __APPLE__
#include <unistd.h>
#import <Foundation/Foundation.h>
#endif

#include "CoreMinimal.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

#include "Platform/Platform.h"

#include "Utils/TextFormatting.h"
#include "Files/FileManager.h"

namespace EE
{
    FileList FileManager::files = FileList();

    const File& FileManager::GetFile( const WString& filePath )
    {
        WString fullFilePath = GetFullPath( filePath );
        FileList::iterator found = FindInFiles( fullFilePath );

        if ( found != files.end() && found->IsValid() ) return *found;

        files.emplace_back( filePath );
        return files.back();
    }

    WString FileManager::GetFileExtension( const WString& path )
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

    WString FileManager::GetFullPath( const WString& path )
    {
#ifdef EE_PLATFORM_WINDOWS
        WChar fullPath[ MAX_PATH + 1 ];
        GetFullPathName( path.c_str(), MAX_PATH, fullPath, NULL );
        return fullPath;
#elif EE_PLATFORM_APPLE
        WString resourcesPath = path;
        WString resourcesFolderName = L"Resources/";
        Text::Replace( resourcesPath, resourcesFolderName, WString( L"" ) );
        NSString* NSStringToResourcesPath = [[NSString alloc]initWithBytes:resourcesPath.data()
            length : resourcesPath.size() * sizeof( wchar_t )
            encoding : NSUTF32LittleEndianStringEncoding];
        NSString* filePath = [[NSBundle mainBundle]pathForResource:NSStringToResourcesPath ofType : @""];
        NSStringEncoding Encode = CFStringConvertEncodingToNSStringEncoding( kCFStringEncodingUTF32LE );
        NSData* SData = [ filePath dataUsingEncoding : Encode ];

        return WString( (wchar_t*)[ SData bytes ], [ SData length ] / sizeof( wchar_t ) );
#else
        Char fullPath[ PATH_MAX + 1 ];
        Char* ptr = realpath( WStringToString( path ).c_str(), fullPath );
        return CharToWChar( fullPath );
#endif
    }

    WString FileManager::GetAppDirectory()
    {
#ifdef EE_PLATFORM_WINDOWS
        WChar buffer[ MAX_PATH ];
        GetCurrentDirectory( _MAX_DIR, buffer );
        WString currentDirectory( buffer );
#else
        NSStringEncoding encode = CFStringConvertEncodingToNSStringEncoding( kCFStringEncodingUTF32LE );
        NSData* sData = [[[NSBundle mainBundle]resourcePath] dataUsingEncoding:encode];

        WString currentDirectory( (wchar_t*)[ sData bytes ], [ sData length ] / sizeof( wchar_t ) );
#endif
        return currentDirectory;
    }

    FileList::iterator FileManager::FindInFiles( const WString& filePath )
    {
        return std::find_if( files.begin(), files.end(), [filePath]( File& file )
            -> bool
            {
                return file.GetPath() == filePath;
            }
        );
    }

    WString FileManager::ReadStream( const File* file )
    {
        FileStream<WString> stream( *file );
        WString str;
        if ( stream.Open( std::ios::in, true ) )
        {
            stream.ReadStream( str );
            stream.Close();
        }
        return str;
    }

    NString FileManager::ReadNarrowStream( const File* file )
    {
        FileStream<NString> stream( *file );
        NString str;
        if ( stream.Open( std::ios::in, true ) )
        {
            stream.ReadStream( str );
            stream.Close();
        }
        return str;
    }

    TArray<char> FileManager::ReadBinaryStream( const File* file )
    {
        FileStream<NString> stream( *file );
        TArray<char> buffer;
        if ( stream.Open( std::ios::ate | std::ios::binary | std::ios::in, false ) )
        {
            size_t fileSize = stream.GetPosition();
            buffer.resize( fileSize );

            stream.MoveCursor( 0 );
            stream.ReadStream( buffer.data(), fileSize );
            stream.Close();
        }
        return buffer;
    }
}