
#include "CoreMinimal.h"

#include <cctype>
#include <cstdio>
#include <future>

#include "Math/Transform.h"
#include "Resources/MeshParser.h"
#include "Resources/OBJParser.h"

// --- Visual Studio
#ifdef _MSC_VER 
#define strtok_r strtok_s
#if (_MSC_VER >= 1310) 
/*VS does not like fopen, but fopen_s is not standard C so unusable here*/
#pragma warning( disable : 4996 ) 
#endif
#endif

namespace EE
{
    // Original crack_atof version is at http://crackprogramming.blogspot.sg/2012/10/implement-atof.html
    // But it cannot convert floating point with high +/- exponent.
    // The version below by Tian Bo fixes that problem and improves performance by 10%
    // http://coliru.stacked-crooked.com/a/2e28f0d71f47ca5e
    inline float fast_strtof( const char* string, char** character )
    {
        *character = (char*)string;
        if ( !*character || !**character )
            return 0;

        int sign = 1;
        float intPart = 0.0;
        float fractionPart = 0.0;
        bool hasFraction = false;
        bool hasExpo = false;

        // Take care of +/- sign
        if ( **character == '-' ) { ++*character; sign = -1; }
        else if ( **character == '+' )++*character;

        while ( **character != '\0' && **character != ',' && **character != ' ' )
        {
            if ( **character >= '0' && **character <= '9' )
                intPart = intPart * 10 + (**character - '0');
            else if ( **character == '.' )
            {
                hasFraction = true;
                ++*character; break;
            }
            else if ( **character == 'e' )
            {
                hasExpo = true;
                ++*character; break;
            }
            else
                return sign * intPart;

            ++*character;
        }

        if ( hasFraction )
        {
            float fractionExponential = 0.1F;

            while ( **character != '\0' && **character != ',' && **character != ' ' )
            {
                if ( **character >= '0' && **character <= '9' )
                {
                    fractionPart += fractionExponential * (**character - '0');
                    fractionExponential *= 0.1F;
                }
                else if ( **character == 'e' )
                {
                    hasExpo = true;
                    ++*character;
                    break;
                }
                else
                {
                    return sign * (intPart + fractionPart);
                }
                ++ * character;
            }
        }

        float exponentPart = 1.0;
        if ( (**character != '\0' && **character != ',' && **character != ' ') && hasExpo )
        {
            int32 exponentSign = 1;
            if ( **character == '-' )
            {
                exponentSign = -1;
                ++*character;
            }
            else if ( **character == '+' )
            {
                ++*character;
            }

            int32 e = 0;
            while ( (**character != '\0' && **character != ',' && **character != ' ') && **character >= '0' && **character <= '9' )
            {
                e = e * 10 + **character - '0';
                ++*character;
            }

            exponentPart = Math::Pow10<float, int32>( exponentSign * e );
        }
        ++ * character;

        return sign * (intPart + fractionPart) * exponentPart;
    }

    bool OBJLoader::GetSimilarVertexIndex( const StaticVertex& vertex, TDictionary<StaticVertex, uint32>& vertexToIndex, uint32& result )
    {
        TDictionary<StaticVertex, uint32>::iterator it = vertexToIndex.find( vertex );
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

    void OBJLoader::ExtractVector3( const NChar* text, Vector3* vector )
    {
        NChar* lineState;
        vector->x = fast_strtof( text, &lineState );
        vector->y = fast_strtof( lineState, &lineState );
        vector->z = fast_strtof( lineState, &lineState );
    }

    void OBJLoader::ExtractVector2( const NChar* text, Vector2* vector )
    {
        NChar* lineState;
        vector->x = fast_strtof( text, &lineState );
        vector->y = fast_strtof( lineState, &lineState );
    }

    void OBJLoader::ExtractIntVector3( const NChar* text, IntVector3* vector )
    {
        NChar* lineState;
        vector->x = (int32)fast_strtof( text, &lineState );
        vector->y = (int32)fast_strtof( lineState, &lineState );
        vector->z = (int32)fast_strtof( lineState, NULL );
    }

    void OBJLoader::ParseVertexPositions( ExtractedData& data )
    {
        for ( TArray<const NChar*>::const_iterator Line = data.linePositions.begin(); Line != data.linePositions.end(); ++Line )
        {
            const size_t Pos = Line - data.linePositions.begin();
            ExtractVector3( (*Line) + 1, &data.positions[ Pos ] );
        }
    }

    void OBJLoader::ParseVertexNormals( ExtractedData& data )
    {
        for ( TArray<const NChar*>::const_iterator Line = data.lineNormals.begin(); Line != data.lineNormals.end(); ++Line )
        {
            const size_t Pos = Line - data.lineNormals.begin();
            ExtractVector3( (*Line) + 1, &data.normals[ Pos ] );
        }
    }

    void OBJLoader::ParseVertexUVs( ExtractedData& data )
    {
        for ( TArray<const NChar*>::const_iterator Line = data.lineUVs.begin(); Line != data.lineUVs.end(); ++Line )
        {
            const size_t Pos = Line - data.lineUVs.begin();
            ExtractVector2( (*Line) + 1, &data.uvs[ Pos ] );
        }
    }

    void OBJLoader::ParseFaces( ExtractedData& data )
    {
        if ( data.objects.empty() ) return;

        // 0 = Vertex, 1 = TextureCoords, 2 = Normal
        IntVector3 vertexIndex = IntVector3( -1 );
        bool warned = false;
        auto currentObject = data.objects.begin();
        auto currentObjectMaterial = currentObject->subdivisions.begin();
        int32 vertexCount = 0;
        const NChar* lineChar;
        for ( TArray<const NChar*>::const_iterator Line = data.lineVertexIndices.begin(); Line != data.lineVertexIndices.end(); ++Line )
        {
            while ( (currentObject + 1) != data.objects.end() &&
                Line - data.lineVertexIndices.begin() == (currentObject + 1)->vertexIndicesPos )
            {
                ++currentObject;
                currentObjectMaterial = currentObject->subdivisions.begin();
            }

            if ( (currentObjectMaterial + 1) != currentObject->subdivisions.end() &&
                Line - data.lineVertexIndices.begin() == (currentObjectMaterial + 1)->vertexIndicesPos )
            {
                currentObjectMaterial++;
            }

            vertexIndex[ 0 ] = vertexIndex[ 1 ] = vertexIndex[ 2 ] = -1;
            vertexCount = 0;
            lineChar = *Line;
            while ( *lineChar != '\0' && *lineChar != '\n' )
            {
                vertexIndex[ 0 ] = -1;
                if ( *(lineChar + 1) != '/' )
                    vertexIndex[ 0 ] = atoi( ++lineChar );
                while ( *lineChar != '/' && !isspace( *lineChar ) ) ++lineChar;
                if ( *lineChar == '/' )
                {
                    vertexIndex[ 1 ] = -1;
                    if ( *(lineChar + 1) != '/' )
                        vertexIndex[ 1 ] = atoi( ++lineChar );
                    while ( *lineChar != '/' && !isspace( *lineChar ) ) ++lineChar;
                    if ( *lineChar == '/' )
                    {
                        vertexIndex[ 2 ] = -1;
                        if ( *(lineChar + 1) != '/' )
                            vertexIndex[ 2 ] = atoi( ++lineChar );
                        while ( !isspace( *++lineChar ) );
                    }
                }
                ++vertexCount;

                if ( vertexCount == 4 )
                {
                    data.vertexIndices.push_back( data.vertexIndices[ data.vertexIndices.size() - 3 ] );
                    data.vertexIndices.push_back( data.vertexIndices[ data.vertexIndices.size() - 2 ] );
                    data.vertexIndices.push_back( vertexIndex );
                    currentObject->vertexIndicesCount += 3;
                    currentObjectMaterial->vertexIndicesCount += 3;
                }
                else
                {
                    data.vertexIndices.push_back( vertexIndex );
                    currentObject->vertexIndicesCount++;
                    currentObjectMaterial->vertexIndicesCount++;
                }

                if ( vertexCount > 4 && !warned )
                {
                    warned = true;
                    EE_LOG_CORE_WARN( L"The model has n-gons, this may lead to unwanted geometry" );
                }
            }
        }
    }

    OBJLoader::Keyword OBJLoader::GetKeyword( const NChar* word )
    {
        if ( word + 1 == NULL || word + 2 == NULL ) return Undefined;

        if ( word[ 0 ] == 'f' ) return Face;
        if ( word[ 0 ] == 'v' )
        {
            if ( word[ 1 ] == ' ' ) return Vertex;
            if ( word[ 1 ] == 'n' && word[ 2 ] == ' ' ) return Normal;
            if ( word[ 1 ] == 't' && word[ 2 ] == ' ' ) return TextureCoord;
        }
        if ( word[ 0 ] == '#' && word[ 1 ] == ' ' ) return Comment;
        if ( word[ 0 ] == 'o' && word[ 1 ] == ' ' ) return Object;
        if ( word[ 0 ] == 'g' && word[ 1 ] == ' ' ) return Group;
        if ( word[ 0 ] == 'u' && word[ 1 ] == 's' && word[ 2 ] == 'e' ) return Material;
        if ( word[ 0 ] == 'c' ) return CSType;

        return Undefined;
    }

    void OBJLoader::PrepareData( const NChar* inFile, ExtractedData& modelData )
    {
        const NChar* pointer = inFile;
        int32 vertexCount = 0;
        int32 normalCount = 0;
        int32 uvCount = 0;
        int32 faceCount = 0;
        Keyword keyword;

        while ( *pointer != '\0' )
        {
            keyword = GetKeyword( pointer );
            if ( keyword == Undefined )
            {
                while ( *pointer != '\n' && *pointer != '\0' ) ++pointer;
                ++pointer;
                continue;
            };
            if ( keyword == Comment )
            {
                while ( *pointer != '\n' && *pointer != '\0' ) ++pointer;
                ++pointer;
                continue;
            };
            if ( keyword == Vertex )
            {
                modelData.linePositions.push_back( ++pointer );
                while ( *pointer != '\n' && *pointer != '\0' ) ++pointer;
                ++pointer;
                vertexCount++; continue;
            };
            if ( keyword == Normal )
            {
                modelData.lineNormals.push_back( pointer += 2 );
                while ( *pointer != '\n' && *pointer != '\0' ) ++pointer;
                ++pointer;
                normalCount++; continue;
            };
            if ( keyword == TextureCoord )
            {
                modelData.lineUVs.push_back( pointer += 2 );
                while ( *pointer != '\n' && *pointer != '\0' ) ++pointer;
                ++pointer;
                uvCount++;     continue;
            };
            if ( keyword == Face )
            {
                if ( modelData.objects.size() == 0 )
                {
                    modelData.objects.push_back( ObjectData() );
                    modelData.objects.back().name = "default";
                }

                if ( modelData.objects.back().subdivisions.empty() )
                {
                    modelData.objects.back().subdivisions.push_back( ObjectData::Subdivision() );
                    modelData.objects.back().subdivisions.back().name = "default";
                }

                modelData.lineVertexIndices.push_back( ++pointer );
                while ( *pointer != '\n' && *pointer != '\0' ) ++pointer;
                ++pointer;
                faceCount++;   continue;
            };
            if ( keyword == Group || keyword == Object )
            {
                modelData.objects.push_back( ObjectData() );
                modelData.objects.back().vertexIndicesPos = (int32)modelData.lineVertexIndices.size();
                ++pointer;
                while ( *(pointer + 1) != '\n' && *(pointer + 1) != '\0' )
                {
                    modelData.objects.back().name += *++pointer;
                }
                continue;
            }
            if ( keyword == Material )
            {
                if ( modelData.objects.size() == 0 )
                {
                    modelData.objects.push_back( ObjectData() );
                    modelData.objects.back().vertexIndicesPos = (int32)modelData.lineVertexIndices.size();
                    modelData.objects.back().name = "default";
                }
                pointer += 6;
                NString name;
                while ( *(pointer + 1) != '\n' && *(pointer + 1) != '\0' ) { name += *++pointer; }
                modelData.objects.back().subdivisions.push_back( ObjectData::Subdivision() );
                modelData.objects.back().subdivisions.back().name = name;
                modelData.objects.back().subdivisions.back().vertexIndicesPos = (int32)modelData.lineVertexIndices.size();
                continue;
            }

            ++pointer;
        }

        modelData.positions.resize( vertexCount );
        modelData.normals.resize( normalCount );
        modelData.uvs.resize( uvCount );
        modelData.vertexIndices.reserve( faceCount * 4 );
    }

    bool OBJLoader::LoadModel( ModelParser::ModelDataInfo& info, const ModelParser::ParsingOptions& options )
    {
        if ( options.file == NULL || !options.file->IsValid() ) return false;

        ExtractedData modelData;

        // --- Read File
        {
            Timestamp timer;

            timer.Begin();
            NString* memoryText = new NString();
            options.file->ReadNarrowStream( memoryText );

            PrepareData( memoryText->c_str(), modelData );
            ParseFaces( modelData );
            ParseVertexPositions( modelData );
            ParseVertexNormals( modelData );
            ParseVertexUVs( modelData );

            modelData.lineNormals.clear();
            modelData.linePositions.clear();
            modelData.lineUVs.clear();
            modelData.lineVertexIndices.clear();

            delete memoryText;

            timer.Stop();
            EE_LOG_CORE_INFO(
                L"\u250C> Parsed {0} vertices and {1} triangles in {2:.3f}ms",
                Text::FormatUnit( modelData.vertexIndices.size(), 2 ),
                Text::FormatUnit( modelData.vertexIndices.size() / 3, 2 ),
                timer.GetDeltaTime<Ticker::Mili>()
            );
        }

        uint64* indices = new uint64[ modelData.vertexIndices.size() ];
        size_t count = 0;

        size_t totalAllocatedSize = 0;
        size_t totalUniqueVertices = 0;

        info.parentNode = ModelNode( "ParentNode" );
        Timestamp timer;
        timer.Begin();
        for ( int32 objectCount = 0; objectCount < modelData.objects.size(); ++objectCount )
        {
            ObjectData& data = modelData.objects[ objectCount ];
            if ( data.vertexIndicesCount == 0 ) continue;

            TDictionary<StaticVertex, unsigned> vertexToIndex;
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

                size_t initialCount = count;
                for ( ; count < initialCount + materialIndex.vertexIndicesCount; ++count )
                {
                    IntVector3 vertexIndices = modelData.vertexIndices[ count ];
                    StaticVertex newVertex = {
                        vertexIndices[ 0 ] >= 0 ? modelData.positions[ vertexIndices[ 0 ] - 1 ] : 0,
                        vertexIndices[ 2 ] >= 0 ? modelData.normals[ vertexIndices[ 2 ] - 1 ] : Vector3( 0., 1., 0. ),
                        0,
                        vertexIndices[ 1 ] >= 0 ? modelData.uvs[ vertexIndices[ 1 ] - 1 ] : 0,
                        vertexIndices[ 1 ] >= 0 ? modelData.uvs[ vertexIndices[ 1 ] - 1 ] : 0,
                        Vector4( 1 )
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

            if ( modelData.vertexIndices[ count - 1 ][ 1 ] >= 0 ) outMesh->uvChannels = 1;
            if ( modelData.vertexIndices[ count - 1 ][ 2 ] >= 0 ) outMesh->hasNormals = true;
            outMesh->ComputeTangents();
            outMesh->bounding = data.bounding;
            outMesh->hasBoundingBox = true;

#ifdef EE_DEBUG
            EE_LOG_CORE_DEBUG(
                L"\u251C> Parsed {0}	vertices in {1}	at [{2:d}]'{3}'",
                Text::FormatUnit( data.vertexIndicesCount, 2 ),
                Text::FormatData( sizeof( MeshFaces ) * outMesh->faces.size() + sizeof( StaticVertex ) * outMesh->staticVertices.size(), 2 ),
                info.meshes.size(),
                Text::NarrowToWide( outMesh->name )
            );
#endif // EE_DEBUG

            totalAllocatedSize += sizeof( MeshFaces ) * outMesh->faces.size() + sizeof( StaticVertex ) * outMesh->staticVertices.size();
        }

        modelData.normals.clear();
        modelData.positions.clear();
        modelData.uvs.clear();
        modelData.vertexIndices.clear();

        delete[] indices;

        timer.Stop();
        EE_LOG_CORE_INFO( L"\u2514> Allocated {0} in {1:.2f}ms", Text::FormatData( totalAllocatedSize, 2 ), timer.GetDeltaTime<Ticker::Mili>() );
        
        info.isValid = true;
        return true;
    }
}