
#include "CoreMinimal.h"
#include "Rendering/Mesh.h"

namespace EE
{
    StaticVertex::StaticVertex( const Vector3f& pos, const Vector3f& normal, const Vector2f& uv ) :
        position( pos ), normal( normal ), tangent(), uv0( uv ), uv1( uv ), color( 1.0F )
    {
    }

    StaticVertex::StaticVertex( const Vector3f& point, const Vector3f& normal, const Vector3f& tangent,
        const Vector2f& uv0, const Vector2f& uv1, const Vector4f& color ) :
        position( point ), normal( normal ), tangent( tangent ),
        uv0( uv0 ), uv1( uv1 ), color( color )
    {
    }

    bool StaticVertex::operator<( const StaticVertex other ) const
    {
        return memcmp( (void*)this, (void*)&other, sizeof( StaticVertex ) ) > 0;
    }

    bool StaticVertex::operator==( const StaticVertex& other ) const
    {
        return (position == other.position
            && normal == other.normal
            && tangent == other.tangent
            && uv0 == other.uv0
            && uv1 == other.uv1
            && color == other.color);
    };

    SkinVertex::SkinVertex( const StaticVertex& other )
    {
        position = other.position;
        normal = other.normal;
        tangent = other.tangent;
        uv0 = other.uv0;
        uv1 = other.uv1;
        color = other.color;
    }

    Mesh::Mesh()
    {
        // vertexArrayPointer = NULL;
        data = MeshData();
    }

    Mesh::Mesh( const MeshData& otherData )
    {
        // vertexArrayPointer = NULL;
        data = otherData;
    }

    Mesh::Mesh( MeshData* otherData )
    {
        // vertexArrayPointer = NULL;
        data.Transfer( *otherData );
    }

    void Mesh::SwapMeshData( MeshData& newData )
    {
        Clear(); data.Transfer( newData );
    }

    void Mesh::CopyMeshData( const MeshData& newData )
    {
        Clear(); data = newData;
    }

    // bool Mesh::SetUpBuffers()
    // {
        // if ( data.staticVertices.size() <= 0 || data.faces.size() <= 0 ) return false;
        // 
        // static BufferLayout DafultLayout = {
        //     { EShaderDataType::Float3, "_iVertexPosition" },
        //     { EShaderDataType::Float3, "_iVertexNormal", true },
        //     { EShaderDataType::Float3, "_iVertexTangent", true },
        //     { EShaderDataType::Float2, "_iVertexUV0" },
        //     { EShaderDataType::Float2, "_iVertexUV1" },
        //     { EShaderDataType::Float4, "_iVertexColor" }
        // };
        // 
        // // Give our vertices to VAO
        // VertexBufferPtr VertexBufferPointer = NULL;
        // VertexBufferPointer = VertexBuffer::Create( (float*)&data.staticVertices[ 0 ], (uint32)(data.staticVertices.size() * sizeof( StaticVertex )), UM_Static );
        // VertexBufferPointer->SetLayout( DafultLayout );
        // 
        // vertexArrayPointer = VertexArray::Create();
        // IndexBufferPtr IndexBufferPointer = IndexBuffer::Create(
        //     (uint32*)&data.faces[ 0 ],
        //     (uint32)data.faces.size() * 3, UM_Static
        // );
        // vertexArrayPointer->AddVertexBuffer( VertexBufferPointer );
        // vertexArrayPointer->AddIndexBuffer( IndexBufferPointer );
        // vertexArrayPointer->Unbind();
        // 
    //     return true;
    // }

    // void Mesh::ClearBuffers()
    // {
    //     // vertexArrayPointer = NULL;
    // }

    void Mesh::Clear()
    {
        data.Clear();
    }

    void MeshData::Transfer( MeshData& other )
    {
        name = other.name;
        faces.swap( other.faces );
        staticVertices.swap( other.staticVertices );
        skinVertices.swap( other.skinVertices );
        subdivisionsMap.swap( other.subdivisionsMap );
        materialsMap.swap( other.materialsMap );
        bounding = other.bounding;

        hasNormals = other.hasNormals;
        hasTangents = other.hasTangents;
        hasVertexColor = other.hasVertexColor;
        uvChannels = other.uvChannels;
        hasBoundingBox = other.hasBoundingBox;
        hasBones = other.hasBones;
    }

    void MeshData::ComputeBounding()
    {
        if ( !hasBoundingBox )
        {
            bounding = BoundingBox3f();
            for ( MeshVertices::const_iterator Vertex = staticVertices.begin(); Vertex != staticVertices.end(); ++Vertex )
            {
                bounding.Add( Vertex->position );
            }
            hasBoundingBox = true;
        }
    }

    void MeshData::ComputeTangents()
    {
        if ( uvChannels <= 0 || hasTangents )
            return;

        // --- For each triangle, compute the edge (DeltaPos) and the DeltaUV
        for ( MeshFaces::const_iterator triangle = faces.begin(); triangle != faces.end(); ++triangle )
        {
            const Vector3f& vertexA = staticVertices[ (*triangle).indx0 ].position;
            const Vector3f& vertexB = staticVertices[ (*triangle).indx1 ].position;
            const Vector3f& vertexC = staticVertices[ (*triangle).indx2 ].position;

            const Vector2f& uvA = staticVertices[ (*triangle).indx0 ].uv0;
            const Vector2f& uvB = staticVertices[ (*triangle).indx1 ].uv0;
            const Vector2f& uvC = staticVertices[ (*triangle).indx2 ].uv0;

            // --- Edges of the triangle : position delta
            const Vector3f edge1 = vertexB - vertexA;
            const Vector3f edge2 = vertexC - vertexA;

            // --- UV delta
            const Vector2f deltaUV1 = uvB - uvA;
            const Vector2f deltaUV2 = uvC - uvA;

            float r = 1.F / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            r = std::isfinite( r ) ? r : 0;

            Vector3f tangent;
            tangent.x = r * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = r * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = r * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent.Normalize();

            staticVertices[ (*triangle).indx0 ].tangent = tangent;
            staticVertices[ (*triangle).indx1 ].tangent = tangent;
            staticVertices[ (*triangle).indx2 ].tangent = tangent;
        }

        hasTangents = true;
    }

    void MeshData::ComputeNormals()
    {
        if ( staticVertices.size() <= 0 || hasNormals )
            return;

        for ( MeshFaces::const_iterator Triangle = faces.begin(); Triangle != faces.end(); ++Triangle )
        {
            const Vector3f& vertexA = staticVertices[ (*Triangle).indx0 ].position;
            const Vector3f& vertexB = staticVertices[ (*Triangle).indx1 ].position;
            const Vector3f& vertexC = staticVertices[ (*Triangle).indx2 ].position;

            const Vector3f edge1 = vertexB - vertexA;
            const Vector3f edge2 = vertexC - vertexA;

            const Vector3f normal = Vector3f::Cross( edge1, edge2 ).Normalized();

            staticVertices[ (*Triangle).indx0 ].normal = normal;
            staticVertices[ (*Triangle).indx1 ].normal = normal;
            staticVertices[ (*Triangle).indx2 ].normal = normal;
        }

        hasNormals = true;
    }

    void MeshData::Clear()
    {
        name.clear();
        faces.clear();
        staticVertices.clear();
        bounding = BoundingBox3f();
        hasNormals = false;
        hasTangents = false;
        hasVertexColor = false;
        uvChannels = 0;
        hasBoundingBox = true;
        hasBones = false;
    }
}