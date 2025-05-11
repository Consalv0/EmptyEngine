
#include "CoreMinimal.h"

#include "Platform/PrePlatform.h"
#include "Platform/Platform.h"

#include "Utils/TextFormatting.h"
#include "Files/FileManager.h"
#include "Files/FileStream.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

#ifdef __APPLE__
#include <unistd.h>
#import <Foundation/Foundation.h>
#endif

namespace EE
{
    FileList FileManager::files = FileList();

    const File& FileManager::GetFile( const U8String& filePath )
    {
        U8String fullFilePath = GetFullPath( filePath );
        FileList::iterator found = FindInFiles( fullFilePath );

        if ( found != files.end() && found->IsValid() ) return *found;

        files.emplace_back( filePath );
        return files.back();
    }

    U8String FileManager::GetFileExtension( const U8String& path )
    {
        U8String::size_type extensionIndex;

        extensionIndex = path.rfind( '.' );

        if ( extensionIndex != WString::npos )
        {
            return path.substr( extensionIndex + 1 );
        }
        else
        {
            return "";
        }
    }

    U8String FileManager::GetFullPath( const U8String& path )
    {
#ifdef EE_PLATFORM_WINDOWS
        WChar fullPath[ MAX_PATH + 1 ];
        GetFullPathName( Text::UTF8ToWide( path ).c_str(), MAX_PATH, fullPath, NULL );
        return Text::WideToUTF8( fullPath );
#elif EE_PLATFORM_APPLE
        WString resourcesPath = Text::UTF8ToWide( path );
        WString resourcesFolderName = L"Resources/";
        Text::Replace( resourcesPath, resourcesFolderName, WString( L"" ) );
        NSString* NSStringToResourcesPath = [[NSString alloc]initWithBytes:resourcesPath.data()
            length : resourcesPath.size() * sizeof( wchar_t )
            encoding : NSUTF32LittleEndianStringEncoding];
        NSString* filePath = [[NSBundle mainBundle]pathForResource:NSStringToResourcesPath ofType : @""];
        NSStringEncoding Encode = CFStringConvertEncodingToNSStringEncoding( kCFStringEncodingUTF32LE );
        NSData* SData = [ filePath dataUsingEncoding : Encode ];

        WString fullPath( (wchar_t*)[ sData bytes ], [ sData length ] / sizeof( wchar_t ) );
        return Text::WideToUTF8( fullPath );
#else
        U8Char fullPath[ PATH_MAX + 1 ];
        U8Char* ptr = realpath( path.c_str(), fullPath );
        return U8String( fullPath );
#endif
    }

    U8String FileManager::GetAppDirectory()
    {
#ifdef EE_PLATFORM_WINDOWS
        WChar buffer[ MAX_PATH ];
        GetCurrentDirectory( _MAX_DIR, buffer );
        U8String currentDirectory = Text::WideToUTF8( buffer );
#elif EE_PLATFORM_APPLE
        NSStringEncoding encode = CFStringConvertEncodingToNSStringEncoding( kCFStringEncodingUTF32LE );
        NSData* sData = [[[NSBundle mainBundle]resourcePath] dataUsingEncoding:encode];

        WString buffer( (wchar_t*)[ sData bytes ], [ sData length ] / sizeof( wchar_t ) );
        U8String currentDirectory = Text::WideToUTF8( buffer );
#else
        U8String currentDirectory{};
#endif
        return currentDirectory;
    }

    FileList::iterator FileManager::FindInFiles( const U8String& filePath )
    {
        return std::find_if( files.begin(), files.end(), [filePath]( File& file )
            -> bool
            {
                return std::strcmp( file.GetPath().c_str(), filePath.c_str()) == 0;
            }
        );
    }

    U8String FileManager::ReadStream( const File* file )
    {
        FileStream<U8String> stream( *file );
        U8String str;
        if ( stream.Open( std::ios::in, true ) )
        {
            stream.ReadStream( str );
            stream.Close();
        }
        return str;
    }

    TArray<char> FileManager::ReadBinaryStream( const File* file )
    {
        FileStream<U8String> stream( *file );
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

#include "Platform/PostPlatform.h"