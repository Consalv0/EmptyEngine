#pragma once

#include "Graphics/Mesh.h"
#include "Math/Vector3.h"

namespace EE
{
    namespace MeshPrimitives
    {
        static MeshData CreateCubeMeshData( const Vector3& position, const Vector3& Size )
        {
            MeshData data;
            data.name = "Cube";
            data.faces = {
                {  0,  1,  2 }, {  3,  4,  5 },
                {  6,  7,  8 },	{  9, 10, 11 },
                { 12, 13, 14 },	{ 15, 16, 17 },
                {  0, 18,  1 },	{  3, 19,  4 },
                {  6, 20,  7 },	{  9, 21, 10 },
                { 12, 22, 13 },	{ 15, 23, 16 }
            };

            data.materialsMap.emplace( 0, "default" );
            data.subdivisionsMap.emplace( 0, Subdivision( { 0, 0, 0, 12 * 3 } ) );

            Matrix4x4f transform = (Matrix4x4f)( Matrix4x4::Translation( position ) * Matrix4x4::Scaling( Size ) );
            data.staticVertices = {
                { transform.MultiplyPoint( { 0.5, -0.5,  0.5} ), { 0, -1,  0}, { 0,  0, 1}, {1, 0}, {1, 0}, {1} },
                { transform.MultiplyPoint( {-0.5, -0.5, -0.5} ), { 0, -1,  0}, { 0,  0, 1}, {0, 1}, {0, 1}, {1} },
                { transform.MultiplyPoint( { 0.5, -0.5, -0.5} ), { 0, -1,  0}, {-0,  0, 1}, {0, 0}, {0, 0}, {1} },
                { transform.MultiplyPoint( {-0.5,  0.5, -0.5} ), { 0,  1,  0}, {-1,  0, 0}, {1, 0}, {1, 0}, {1} },
                { transform.MultiplyPoint( { 0.5,  0.5,  0.5} ), { 0,  1,  0}, {-1,  0, 0}, {0, 1}, {0, 1}, {1} },
                { transform.MultiplyPoint( { 0.5,  0.5, -0.5} ), { 0,  1,  0}, {-1,  0, 0}, {0, 0}, {0, 0}, {1} },
                { transform.MultiplyPoint( { 0.5,  0.5, -0.5} ), { 1, -0,  0}, { 0,  1, 0}, {1, 0}, {1, 0}, {1} },
                { transform.MultiplyPoint( { 0.5, -0.5,  0.5} ), { 1, -0,  0}, { 0,  1, 0}, {0, 1}, {0, 1}, {1} },
                { transform.MultiplyPoint( { 0.5, -0.5, -0.5} ), { 1, -0,  0}, { 0,  1, 0}, {0, 0}, {0, 0}, {1} },
                { transform.MultiplyPoint( { 0.5,  0.5,  0.5} ), { 0, -0,  1}, { 0,  1, 0}, {1, 0}, {1, 0}, {1} },
                { transform.MultiplyPoint( {-0.5, -0.5,  0.5} ), { 0, -0,  1}, { 0,  1, 0}, {0, 1}, {0, 1}, {1} },
                { transform.MultiplyPoint( { 0.5, -0.5,  0.5} ), { 0, -0,  1}, {-0,  1, 0}, {0, 0}, {0, 0}, {1} },
                { transform.MultiplyPoint( {-0.5, -0.5,  0.5} ), {-1, -0, -0}, { 0,  1, 0}, {0, 0}, {0, 0}, {1} },
                { transform.MultiplyPoint( {-0.5,  0.5, -0.5} ), {-1, -0, -0}, { 0,  1, 0}, {1, 1}, {1, 1}, {1} },
                { transform.MultiplyPoint( {-0.5, -0.5, -0.5} ), {-1, -0, -0}, { 0,  1, 0}, {0, 1}, {0, 1}, {1} },
                { transform.MultiplyPoint( { 0.5, -0.5, -0.5} ), { 0,  0, -1}, { 0, -1, 0}, {1, 0}, {1, 0}, {1} },
                { transform.MultiplyPoint( {-0.5,  0.5, -0.5} ), { 0,  0, -1}, { 0, -1, 0}, {0, 1}, {0, 1}, {1} },
                { transform.MultiplyPoint( { 0.5,  0.5, -0.5} ), { 0,  0, -1}, {-0, -1, 0}, {0, 0}, {0, 0}, {1} },
                { transform.MultiplyPoint( {-0.5, -0.5,  0.5} ), { 0, -1,  0}, { 0,  0, 1}, {1, 1}, {1, 1}, {1} },
                { transform.MultiplyPoint( {-0.5,  0.5,  0.5} ), { 0,  1,  0}, {-1,  0, 0}, {1, 1}, {1, 1}, {1} },
                { transform.MultiplyPoint( { 0.5,  0.5,  0.5} ), { 1, -0,  0}, { 0,  1, 0}, {1, 1}, {1, 1}, {1} },
                { transform.MultiplyPoint( {-0.5,  0.5,  0.5} ), { 0, -0,  1}, { 0,  1, 0}, {1, 1}, {1, 1}, {1} },
                { transform.MultiplyPoint( {-0.5,  0.5,  0.5} ), {-1, -0, -0}, { 0,  1, 0}, {1, 0}, {1, 0}, {1} },
                { transform.MultiplyPoint( {-0.5, -0.5, -0.5} ), { 0,  0, -1}, { 0, -1, 0}, {1, 1}, {1, 1}, {1} }
            };

            data.hasBoundingBox = false;
            data.hasNormals = true;
            data.hasVertexColor = true;
            data.uvChannels = 1;
            data.ComputeTangents();
            data.ComputeBounding();
            return data;
        }

        static MeshData CreateQuadMeshData( const Vector3& position, const Vector3& Size )
        {
            MeshData data;
            data.name = "Quad";
            data.faces = {
                {0, 1, 2}, {0, 3, 1}
            };

            data.materialsMap.emplace( 0, "default" );
            data.subdivisionsMap.emplace( 0, Subdivision( { 0, 0, 0, 2 * 3 } ) );

            Matrix4x4 transform = Matrix4x4::Translation( position ) * Matrix4x4::Scaling( Size );
            data.staticVertices = {
                { Vector4f( transform.MultiplyPoint( { 1, -1, -0} ) ), {0, 0, 1}, {1, -0, -0}, {1, 0}, {1, 0}, {1} },
                { Vector4f( transform.MultiplyPoint( {-1,  1,  0} ) ), {0, 0, 1}, {1, -0, -0}, {0, 1}, {0, 1}, {1} },
                { Vector4f( transform.MultiplyPoint( { 1,  1,  0} ) ), {0, 0, 1}, {1, -0, -0}, {1, 1}, {1, 1}, {1} },
                { Vector4f( transform.MultiplyPoint( {-1, -1, -0} ) ), {0, 0, 1}, {1, -0, -0}, {0, 0}, {0, 0}, {1} }
            };

            data.hasBoundingBox = false;
            data.hasNormals = true;
            data.hasVertexColor = true;
            data.uvChannels = 1;
            data.ComputeTangents();
            data.ComputeBounding();
            return data;
        }
    }
}