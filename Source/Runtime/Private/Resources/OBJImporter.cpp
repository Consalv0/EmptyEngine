
#include "CoreMinimal.h"

#include "Resources/OBJImporter.h"
#include "Files/FileMap.h"

#include <Core/Name.h>
#include "Engine/Ticker.h"
#include "Utils/TextFormatting.h"
#include "Utils/Memory.h"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <cassert>

constexpr size_t kBufferBlockSize = 1u << 18u;
constexpr size_t kMaxLineSize = 1024u * 4u;

namespace EE
{
    bool ParseLine( std::string_view line, ModelParser::ModelDataInfo& info )
    {
        size_t index = 0;
        while ( index < line.size() && (line[ index ] == ' ' || line[ index ] == '\t') )
        {
            ++index;
        }
        line.remove_prefix( index );

        if ( line.empty() )
            return true;

        switch ( line.front() )
        {
        case 'v':
        {
            line.remove_prefix( 2 );
            info.meshes.back().staticVertices.emplace_back( 0.F, 0.F, 0.F );
            break;
        }
        case 'f':
        {
            line.remove_prefix( 2 );
            MeshFace face;
            info.meshes.back().faces.push_back( face );
            break;
        }
        case '#':
            break;
        case 'g':
        {
            line.remove_prefix( 2 );
            NString name = NString( line );
            // Trim( name );
            
            info.meshes.emplace_back( name );
            EE_LOG_INFO( L"Mesh group {}", Text::NarrowToWide( name ) );
            break;
        }
        case 'o':
        {
            line.remove_prefix( 2 );
            NString name = NString( line );
            // Trim( name );

            info.meshes.emplace_back( name );
            EE_LOG_INFO( L"Mesh object {}", Text::NarrowToWide( name ) );
            break;
        }
        case 'm':
        {
            break;
        }
        case 'u':
        {
            break;
        }
        case 's':
        {
            break;
        }
        case 'l':
        {
            break;
        }
        case 'p':
        {
            break;
        }
        default:
            return false;
        }

        return true;
    }

    bool OBJImporter::LoadModel( ModelParser::ModelDataInfo& info, const ModelParser::ParsingOptions& options )
    {
        FileMap file( options.file );
        if ( file.GetError() != 0 )
        {
            EE_LOG_ERROR( L"Error reading file '{}', returned code {}", options.file.GetPath(), file.GetError());
            return false;
        }

        size_t fileSize = file.GetSize();
        size_t offset = 0;
        size_t bytesRead = kBufferBlockSize;
        Memory::AlignedMemory<NChar> frontBuffer = Memory::AlignedMemory<NChar>( kBufferBlockSize + kMaxLineSize, kMaxLineSize );
        // Memory::AlignedMemory<NChar> backBuffer = Memory::AlignedMemory<NChar>( kBufferBlockSize + kMaxLineSize, kMaxLineSize );

        auto line = std::string_view();
        auto text = std::string_view();

        size_t lineCount = 0;
        size_t blockCount = 0;

        Ticker::Second::ReturnType start = Ticker::GetEpochTimeNow<Ticker::Second>();
        while ( bytesRead >= kBufferBlockSize )
        {
            if ( file.ReadBlock( offset, kBufferBlockSize + kMaxLineSize, frontBuffer.GetData() ) != 0 )
            {
                EE_LOG_ERROR( L"Error reading block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
                return false;
            }

            if ( file.WaitForResult( &bytesRead ) != 0 )
            {
                EE_LOG_ERROR( L"Error reading result of block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
                return false;
            }

            text = std::string_view( frontBuffer.GetData(), bytesRead );
            if ( const NChar* endLine = static_cast<const NChar*>(memchr( text.data(), '\n', kMaxLineSize )) )
            {
                size_t pos = static_cast<size_t>(endLine - text.data() );
                text.remove_prefix( pos + 1 );
            }
            else
            {
                EE_LOG_ERROR( L"Error reading file '{}', line is too log: \n{}", options.file.GetPath(), Text::NarrowToWide( NString( text, 0, kMaxLineSize ) ) );
                return false;
            }

            bool isEndOfFile = bytesRead < (kBufferBlockSize + kMaxLineSize);
            while ( text.size() >= kMaxLineSize || ( text.size() > 0 && isEndOfFile ) )
            {
                if ( const NChar* endLine = static_cast<const NChar*>(memchr( text.data(), '\n', text.size() )) )
                {
                    size_t pos = static_cast<size_t>(endLine - text.data());
                    ++lineCount;

                    if ( pos > kMaxLineSize )
                    {
                        EE_LOG_ERROR( L"Error reading file '{}', line is too log: \n{}", options.file.GetPath(), Text::NarrowToWide( NString( text, 0, kMaxLineSize ) ) );
                        return false;
                    }
                    line = text.substr( 0, pos );

                    if ( line.length() > 0 && line.back() == '\r' )
                    {
                        line.remove_suffix( 1 );
                    }
                    text.remove_prefix( pos + 1 );
                }
                else
                {
                    if ( text.size() > kMaxLineSize )
                    {
                        EE_LOG_ERROR( L"Error reading file '{}', line is too log: \n{}", options.file.GetPath(), Text::NarrowToWide( NString( text, 0, kMaxLineSize ) ) );
                        return false;
                    }
                    // if ( last_block )
                    // {
                    //     line = text;
                    //     ++chunk->text.line_count;
                    //     if ( auto rc = ProcessLine( line, chunk, context ); rc != rapidobj_errc::Success )
                    //     {
                    //         chunk->error = Error{ make_error_code( rc ), std::string( line ), chunk->text.line_count };
                    //         return;
                    //     }
                    // }
                    // else
                    // {
                    //     remainder = text.size();
                    //     memcpy( back_buffer + kMaxLineLength - remainder, text.data(), remainder );
                    // }
                    // text = {};
                    break;
                }

                if ( ParseLine( line, info ) == false/*ProcessLine( line, chunk, context )*/ )
                {
                    EE_LOG_ERROR( L"Error reading file '{}', error in line: \n{}", options.file.GetPath(), Text::NarrowToWide( NString( line, 0, kMaxLineSize ) ) );
                    return false;
                }
            }

            blockCount++;
            EE_LOG_DEBUG( L"Block [{}], Line [{}] %{}", blockCount, lineCount, (float)(offset + bytesRead) / (float)fileSize * 100 );
            offset += kBufferBlockSize;
        }

        Ticker::Second::ReturnType end = Ticker::GetEpochTimeNow<Ticker::Second>();
        EE_LOG_DEBUG( L"Finished in {}s", end - start );

        return true;
    }
}
