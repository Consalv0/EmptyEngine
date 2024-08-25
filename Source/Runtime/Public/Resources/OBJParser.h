#pragma once

#include "CoreMinimal.h"

#include "Rendering/Mesh.h"
#include "Utils/Hasher.h"

namespace EE
{
    class OBJLoader
    {
    private:
        struct ObjectData
        {
            struct Subdivision
            {
                NString name;
                int32 vertexIndicesPos = 0;
                int32 vertexIndicesCount = 0;
            };

            NString name;
            BoundingBox3f bounding;
            TArray<Subdivision> subdivisions;
            bool hasNormals;
            bool hasTextureCoords;
            int32 vertexIndicesPos = 0;
            int32 vertexIndicesCount = 0;
        };

        struct ExtractedData
        {
            TArray<ObjectData> objects;
            TArray<IntVector3> vertexIndices;
            MeshVector3D positions;
            MeshVector3D normals;
            MeshUVs uvs;
            TArray<const NChar*> lineVertexIndices;
            TArray<const NChar*> linePositions;
            TArray<const NChar*> lineNormals;
            TArray<const NChar*> lineUVs;
        };

        enum Keyword
        {
            Comment, Object, Group, Material, Vertex, Normal, TextureCoord, Face, CSType, Undefined
        };

        static Keyword GetKeyword( const NChar* line );

        static bool GetSimilarVertexIndex(
            const StaticVertex& vertex,
            TMap<StaticVertex, uint32>& vertexToIndex,
            uint32& result
        );

        static void ExtractVector3( const NChar* text, Vector3f* vertex );
        static void ExtractVector2( const NChar* text, Vector2f* vertex );
        static void ExtractIntVector3( const NChar* text, IntVector3* tertex );

        static void PrepareData( const NChar* inFile, ExtractedData& data );

        static void ParseVertexPositions( ExtractedData& data );
        static void ParseVertexNormals( ExtractedData& data );
        static void ParseVertexUVs( ExtractedData& data );
        static void ParseFaces( ExtractedData& data );

    public:
        /** Load mesh data from file extension Wavefront, it will return the models separated by objects, optionaly
          * there's a way to optimize the vertices. */
        static bool LoadModel( ModelParser::ModelDataInfo& info, const ModelParser::ParsingOptions& options );

    };
}

EE_MAKE_HASHABLE(EE::StaticVertex, t.position, t.normal, t.tangent, t.uv0, t.uv1, t.color)