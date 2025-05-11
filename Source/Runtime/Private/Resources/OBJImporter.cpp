
#include "CoreMinimal.h"

#include "Files/FileMap.h"

#include <Core/Name.h>
#include "Engine/Ticker.h"
#include "Utils/TextFormatting.h"
#include "Utils/Memory.h"
#include "Utils/Hasher.h"
#include "Math/CoreMath.h"
#include "Utils/TextFormattingMath.h"

#include "Resources/OBJImporter.h"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <cassert>

constexpr size_t kBufferBlockSize = 1u << 18u;
constexpr size_t kMaxLineSize = 1024u * 4u;

#ifdef EE_MAKE_HASHABLE
EE_MAKE_HASHABLE( EE::StaticVertex, t.position, t.normal, t.tangent, t.uv0, t.uv1, t.color )
#endif // EE_MAKE_HASHABLE

namespace EE
{
    struct ObjectData
    {
        struct Subdivision
        {
            U8String name;
            size_t vertexIndicesPos = 0;
            size_t vertexIndicesCount = 0;
        };

        U8String name;
        Box3f bounding;
        TArray<Subdivision> subdivisions;
        bool hasNormals;
        bool hasTextureCoords;
        size_t vertexIndicesPos = 0;
        size_t vertexIndicesCount = 0;
    };

    struct ExtractedData
    {
        TArray<ObjectData> objects;
        TArray<UIntVector3> vertexIndices;
        MeshVector3D positions;
        MeshVector3D normals;
        MeshUVs uvs;
    };

    inline constexpr void TrimLeft( std::string_view* text ) noexcept
    {
        size_t index = 0;
        while ( index < text->size() && ((*text)[ index ] == ' ' || (*text)[ index ] == '\t') )
        {
            ++index;
        }
        text->remove_prefix( index );
    }

    void ExtractVector3( std::string_view* text, Vector3f* vector )
    {
        TrimLeft( text );
        vector->x = Text::ParseFloat( text );
        vector->y = Text::ParseFloat( text );
        vector->z = Text::ParseFloat( text );
    }

    void ExtractVector2( std::string_view* text, Vector2f* vector )
    {
        vector->x = Text::ParseFloat( text );
        vector->y = Text::ParseFloat( text );
    }

    void ExtractUIntVector3( std::string_view* text, UIntVector3* vector )
    {
        vector->x = (uint32)Text::ParseInt( text );
        vector->y = (uint32)Text::ParseInt( text );
        vector->z = (uint32)Text::ParseInt( text );
        if ( text->empty() == false )
            text->remove_prefix( 1 );
    }

    bool GetSimilarVertexIndex( const StaticVertex& vertex, TMap<StaticVertex, uint32>& vertexToIndex, uint32& result )
    {
        TMap<StaticVertex, uint32>::const_iterator it = vertexToIndex.find( vertex );
        if ( it == vertexToIndex.end() )
        {
            return false;
        }
        else
        {
            result = it->second;
            return true;
        }
    }

    bool ParseLine( std::string_view line, ExtractedData& data )
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
            if ( data.objects.empty() )
            {
                ObjectData oData
                {
                    .name = "empty",
                    .hasNormals = false,
                    .hasTextureCoords = false,
                    .vertexIndicesPos = data.vertexIndices.size(),
                    .vertexIndicesCount = 0
                };

                data.objects.emplace_back( oData );
            }

            if ( line[ 1 ] == ' ' || line[ 1 ] == '\t' )
            {
                line.remove_prefix( 2 );
                ExtractVector3( &line, &data.positions.emplace_back() );
            }
            else if ( line[ 1 ] == 'n' )
            {
                line.remove_prefix( 3 );
                ExtractVector3( &line, &data.normals.emplace_back() );
                data.objects.back().hasNormals = true;
            }
            else if ( line[ 1 ] == 't' )
            {
                line.remove_prefix( 3 );
                ExtractVector2( &line, &data.uvs.emplace_back() );
                data.objects.back().hasTextureCoords = true;
            }
            break;
        }
        case 'f':
        {
            line.remove_prefix( 2 );

            uint8 vertexCount = 0;
            ExtractUIntVector3( &line, &data.vertexIndices.emplace_back() );
            ++vertexCount;
            ++data.objects.back().vertexIndicesCount;
            if ( data.objects.back().subdivisions.size() == 0 )
            {
                data.objects.back().subdivisions.emplace_back( data.objects.back().name, data.vertexIndices.size(), 0 );
            }
            ++data.objects.back().subdivisions.back().vertexIndicesCount;
            if ( line.empty() == false )
            {
                while ( line.empty() == false )
                {
                    ++vertexCount;
                    if ( vertexCount == 4 )
                    {
                        data.vertexIndices.emplace_back( *(&data.vertexIndices.back() - 2) );
                        data.vertexIndices.emplace_back( *(&data.vertexIndices.back() - 1) );
                        ExtractUIntVector3( &line, &data.vertexIndices.emplace_back() );
                        data.objects.back().vertexIndicesCount += 3;
                        data.objects.back().subdivisions.back().vertexIndicesCount += 3;
                        break;
                    }
                    ++data.objects.back().vertexIndicesCount;
                    ++data.objects.back().subdivisions.back().vertexIndicesCount;
                    ExtractUIntVector3( &line, &data.vertexIndices.emplace_back() );
                }
            }
            break;
        }
        case '#':
            break;
        case 'g':
        {
            line.remove_prefix( 2 );
            U8String name = U8String( line );
            // Trim( name );
            
            if ( data.objects.empty() )
            {
                ObjectData oData
                {
                    .name = name,
                    .hasNormals = false,
                    .hasTextureCoords = false,
                    .vertexIndicesPos = data.vertexIndices.size(),
                    .vertexIndicesCount = 0
                };

                data.objects.emplace_back( oData );
            }
            data.objects.back().subdivisions.emplace_back( std::move( name ), data.vertexIndices.size(), 0 );
            break;
        }
        case 'o':
        {
            line.remove_prefix( 2 );
            U8String name = U8String( line );
            // Trim( name );

            ObjectData oData
            {
                .name = std::move( name ),
                .hasNormals = false,
                .hasTextureCoords = false,
                .vertexIndicesPos = data.vertexIndices.size(),
                .vertexIndicesCount = 0
            };

            data.objects.emplace_back( oData );
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

    bool OBJImporter::LoadModel( ModelImporter::ModelResult& info, const ModelImporter::Options& options )
    {
        ExtractedData parsedData;

        {
            Timestamp timer;

            timer.Begin();
            FileMap file( options.file );
            if ( file.GetError() != 0 )
            {
                EE_LOG_ERROR( "Error reading file '{}', returned code {}", options.file.GetPath(), file.GetError() );
                return false;
            }

            uint64 fileSize = file.GetSize();
            uint64 offset = 0;
            uint64 bytesRead = kBufferBlockSize;
            Memory::AlignedMemory<U8Char> frontBuffer = Memory::AlignedMemory<U8Char>( kBufferBlockSize + kMaxLineSize, kMaxLineSize );
            // Memory::AlignedMemory<U8Char> backBuffer = Memory::AlignedMemory<U8Char>( kBufferBlockSize + kMaxLineSize, kMaxLineSize );

            auto line = std::string_view();
            auto text = std::string_view();

            uint32 lineCount = 0;
            uint32 blockCount = 0;

            while ( bytesRead >= kBufferBlockSize )
            {
                if ( file.ReadBlock( offset, kBufferBlockSize + kMaxLineSize, frontBuffer.GetData() ) != 0 )
                {
                    EE_LOG_ERROR( "Error reading block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
                    return false;
                }

                if ( file.WaitForResult( &bytesRead ) != 0 )
                {
                    EE_LOG_ERROR( "Error reading result of block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
                    return false;
                }

                text = std::string_view( frontBuffer.GetData(), bytesRead );
                if ( const U8Char* endLine = static_cast<const U8Char*>(memchr( text.data(), '\n', kMaxLineSize )) )
                {
                    uint64 pos = static_cast<uint64>(endLine - text.data());
                    text.remove_prefix( pos + 1 );
                }
                else
                {
                    EE_LOG_ERROR( "Error reading file '{}', line is too log: \n{}", options.file.GetPath(), U8String( text, 0, kMaxLineSize ) );
                    return false;
                }

                bool isEndOfFile = bytesRead < (kBufferBlockSize + kMaxLineSize);
                while ( text.size() >= kMaxLineSize || (text.size() > 0 && isEndOfFile) )
                {
                    if ( const U8Char* endLine = static_cast<const U8Char*>(memchr( text.data(), '\n', text.size() )) )
                    {
                        uint64 pos = static_cast<uint64>(endLine - text.data());
                        ++lineCount;

                        if ( pos > kMaxLineSize )
                        {
                            EE_LOG_ERROR( "Error reading file '{}', line is too log: \n{}", options.file.GetPath(), U8String( text, 0, kMaxLineSize ) );
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
                            EE_LOG_ERROR( "Error reading file '{}', line is too log: \n{}", options.file.GetPath(), U8String( text, 0, kMaxLineSize ) );
                            return false;
                        }
                        break;
                    }

                    if ( ParseLine( line, parsedData ) == false )
                    {
                        EE_LOG_ERROR( "Error reading file '{}', error in line: \n{}", options.file.GetPath(), U8String( line, 0, kMaxLineSize ) );
                        return false;
                    }
                }

                blockCount++;
#ifdef EE_DEBUG
                EE_LOG_DEBUG( "Block [{}], Line [{}] %{}", blockCount, lineCount, (float)(offset + bytesRead) / (float)fileSize * 100 );
#endif
                offset += kBufferBlockSize;
            }
            timer.Stop();
            EE_LOG_INFO(
                "\u250C> Parsed {0} vertices and {1} triangles in {2:.3f}ms",
                Text::FormatUnit( parsedData.vertexIndices.size(), 2 ),
                Text::FormatUnit( parsedData.vertexIndices.size() / 3, 2 ),
                timer.GetDeltaTime<Ticker::Mili>()
            );
        }

        uint32* indices = new uint32[ parsedData.vertexIndices.size() ];
        uint32 count = 0;

        uint32 totalAllocatedSize = 0;
        uint32 totalUniqueVertices = 0;

        info.parentNode = ModelNode( "ParentNode" );
        Timestamp timer;
        timer.Begin();
        for ( int32 objectCount = 0; objectCount < parsedData.objects.size(); ++objectCount )
        {
            ObjectData& data = parsedData.objects[ objectCount ];
            if ( data.vertexIndicesCount == 0 ) continue;

            TMap<StaticVertex, unsigned> vertexToIndex;
            vertexToIndex.reserve( data.vertexIndicesCount );

            info.parentNode.children.push_back( new ModelNode( data.name ) );
            info.parentNode.children.back()->parent = &info.parentNode;
            info.parentNode.children.back()->hasMesh = true;
            info.parentNode.children.back()->meshKey = info.meshes.size();
            info.meshes.push_back( MeshData() );
            MeshData* outMesh = &info.meshes.back();
            outMesh->name = data.name;

            uint32 indexCount = 0;
            for ( int32 materialCount = 0; materialCount < data.subdivisions.size(); ++materialCount )
            {
                ObjectData::Subdivision& materialIndex = data.subdivisions[ materialCount ];
                outMesh->materialsMap.emplace( (int32)outMesh->materialsMap.size(), materialIndex.name );
                outMesh->subdivisionsMap.emplace( materialCount, Subdivision( { (uint32)materialCount, 0, indexCount, 0 } ) );

                uint32 initialCount = count;
                for ( ; count < initialCount + materialIndex.vertexIndicesCount; ++count )
                {
                    UIntVector3 vertexIndices = parsedData.vertexIndices[ count ];
                    StaticVertex newVertex = {
                        vertexIndices[ 0 ] >= 0 ? parsedData.positions[ vertexIndices[ 0 ] - 1 ] : 0,
                        data.hasNormals && vertexIndices[ 2 ] >= 0 ? parsedData.normals[ vertexIndices[ 2 ] - 1 ] : Vector3f( 0.F, 1.F, 0.F ),
                        0,
                        data.hasTextureCoords && vertexIndices[ 1 ] >= 0 ? parsedData.uvs[ vertexIndices[ 1 ] - 1 ] : 0,
                        data.hasTextureCoords && vertexIndices[ 1 ] >= 0 ? parsedData.uvs[ vertexIndices[ 1 ] - 1 ] : 0,
                        Vector4f( 1 )
                    };
                    data.bounding.Add( newVertex.position );

                    uint32 index = (uint32)count;
                    bool bFoundIndex = false;
                    if ( options.optimize )
                    {
                        bFoundIndex = GetSimilarVertexIndex( newVertex, vertexToIndex, index );
                    }

                    if ( bFoundIndex )
                    {
                        // --- A similar vertex is already in the VBO, use it instead !
                        indices[ count ] = index;
                    }
                    else
                    {
                        // --- If not, it needs to be added in the output data.
                        outMesh->staticVertices.push_back( newVertex );
                        uint32 newIndex = (uint32)outMesh->staticVertices.size() - 1;
                        indices[ count ] = newIndex;
                        if ( options.optimize ) vertexToIndex[ newVertex ] = newIndex;
                        totalUniqueVertices++;
                    }

                    if ( (count - initialCount + 1) % 3 == 0 )
                    {
                        outMesh->faces.push_back( { indices[ count - 2 ], indices[ count - 1 ], indices[ count ] } );
                        outMesh->subdivisionsMap[ materialCount ].indexCount += 3;
                        indexCount += 3;
                    }
                }
            }

            if ( parsedData.vertexIndices[ count - 1 ][ 1 ] >= 0 ) outMesh->uvChannels = 1;
            outMesh->hasNormals = data.hasNormals;
            outMesh->ComputeNormals();
            outMesh->ComputeTangents();
            outMesh->bounding = data.bounding;
            outMesh->hasBoundingBox = true;

#ifdef EE_DEBUG
            EE_LOG_DEBUG(
                "\u251C> Parsed {0}	vertices in {1}	at [{2:d}]'{3}'",
                Text::FormatUnit( data.vertexIndicesCount, 2 ),
                Text::FormatData( sizeof( MeshFaces ) * outMesh->faces.size() + sizeof( StaticVertex ) * outMesh->staticVertices.size(), 2 ),
                info.meshes.size(),
                outMesh->name
            );
#endif // EE_DEBUG

            totalAllocatedSize += sizeof( MeshFaces ) * outMesh->faces.size() + sizeof( StaticVertex ) * outMesh->staticVertices.size();
        }

        parsedData.normals.clear();
        parsedData.positions.clear();
        parsedData.uvs.clear();
        parsedData.vertexIndices.clear();

        delete[] indices;

        timer.Stop();
        EE_LOG_INFO( "\u2514> Allocated {0} in {1:.2f}ms", Text::FormatData( totalAllocatedSize, 2 ), timer.GetDeltaTime<Ticker::Mili>() );

        info.isValid = true;
        return true;
    }
}
