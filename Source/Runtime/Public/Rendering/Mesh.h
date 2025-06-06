#pragma once

#include "Core/Collections.h"
#include "Math/CoreMath.h"
#include "Math/Transform.h"

namespace EE
{
    struct StaticVertex
    {
        Vector3f position;
        Vector3f normal;
        Vector3f tangent;
        Vector2f uv0, uv1;
        Vector4f color;

        StaticVertex() = default;
        StaticVertex( const StaticVertex& other ) = default;
        StaticVertex( StaticVertex&& other ) = default;
        StaticVertex( const Vector3f& pos, const Vector3f& normal, const Vector2f& uv );
        StaticVertex( const Vector3f& pos, const Vector3f& normal, const Vector3f& tangent, const Vector2f& uv0, const Vector2f& uv1, const Vector4f& color );
        StaticVertex& operator=( const StaticVertex& other ) = default;
        bool operator<( const StaticVertex other ) const;
        bool operator==( const StaticVertex& other ) const;
    };

    struct SkinVertex
    {
        Vector3f position;
        Vector3f normal;
        Vector3f tangent;
        Vector2f uv0, uv1;
        Vector4f color;

        uint32 influenceBones[ 4 ] = { 0, 0, 0, 0 };
        float weights[ 4 ]{ 0.0F, 0.0F, 0.0F, 0.0F };

        void AddBoneData( uint32 boneID, float weight )
        {
            for ( size_t i = 0; i < 4; i++ )
            {
                if ( weights[ i ] == 0.0 )
                {
                    influenceBones[ i ] = boneID;
                    weights[ i ] = weight;
                    return;
                }
            }
            EE_LOG_WARN( "Vertex has more than four bones/weights affecting it, extra data will be discarded (boneID={0}, weight={1})", boneID, weight );
        }

        SkinVertex() = default;
        SkinVertex( const SkinVertex& other ) = default;
        SkinVertex( SkinVertex&& other ) = default;
        SkinVertex( const StaticVertex& other );
        SkinVertex& operator=( const SkinVertex& other ) = default;
    };

    struct Subdivision
    {
    public:
        uint32 materialIndex;
        uint32 baseVertex;
        uint32 baseIndex;
        uint32 indexCount;

        bool operator==( const Subdivision& other ) const
        {
            return (materialIndex == other.materialIndex
                && baseVertex == other.baseVertex
                && baseIndex == other.baseIndex
                && indexCount == other.indexCount);
        }
    };

    struct MeshFace
    {
        union
        {
            struct { uint32 indx0, indx1, indx2; };
            struct { uint32 indices[ 3 ]; };
        };
    };

    typedef TArray<MeshFace>        MeshFaces;
    typedef TArray<Vector3f>        MeshVector3D;
    typedef TArray<Vector2f>        MeshUVs;
    typedef TArray<Vector4f>        MeshColors;
    typedef TArray<StaticVertex>    MeshVertices;
    typedef TMap<int32, U8String>   MeshMaterials;

    struct MeshData
    {
        U8String name;
        TArray<MeshFace> faces;
        TMap<int32, Subdivision> subdivisionsMap;
        TMap<int32, U8String> materialsMap;
        TArray<StaticVertex> staticVertices;
        TArray<SkinVertex> skinVertices;
        Box3f bounding;

        bool hasNormals = false;
        bool hasTangents = false;
        bool hasVertexColor = false;
        int32  uvChannels = 0;
        bool hasBoundingBox = false;
        bool hasBones = false;

        void Transfer( MeshData& Other );
        void ComputeBounding();
        void ComputeTangents();
        void ComputeNormals();
        void Clear();
    };

    class Mesh
    {
    public:
        Mesh();
        //* Copy the information to the mesh, the data will be duplicated
        Mesh( const MeshData& otherData );
        //* Transfer information to the mesh, the data will be swapped
        Mesh( MeshData* otherData );

        inline const MeshData& GetMeshData() const { return data; };

        //* Swap all contents of the mesh for new data
        void SwapMeshData( MeshData& data );

        //* Copy all contents of the mesh for new data
        void CopyMeshData( const MeshData& data );

        //* Get VertexArray in Mesh
        // VertexArrayPtr GetVertexArray() const { return vertexArrayPointer; };

        //* Clear the mesh entirely
        void Clear();

    private:
        // VertexArrayPtr vertexArrayPointer;

        MeshData data;
    };
}