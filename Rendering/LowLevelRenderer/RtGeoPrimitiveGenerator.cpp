/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtGeoPrimtiveGenerator.cpp
    Author      :    Jamie Taylor
    Last Edit   :    27/06/13
    Desc        :    Class used to generate a handful of common geometric primitives.
                     Cubes, grids, cylinders and spheres are supported.

                     +Y
                     |
                     |    +Z
                     |   /
                     | /
                     +-------- +X

===============================================================================
*/


#include "RtGeoPrimitiveGenerator.h"


/*
================
GeoPrimitiveGenerator::GeoPrimitiveGenerator
================
*/
GeoPrimitiveGenerator::GeoPrimitiveGenerator( void ) {
    // ...
}

/*
================
GeoPrimitiveGenerator::~GeoPrimitiveGenerator
================
*/
GeoPrimitiveGenerator::~GeoPrimitiveGenerator( void ) {
    // ...
}

/*
================
GeoPrimitiveGenerator::GenerateCube

Create a cube of the specified dimensions centered at the origin.

NOTE: Added another version that uses floats to draw bounding volumes, may be for testing, may be kept...
================
*/
void GeoPrimitiveGenerator::GenerateCube( U32 unitsAlongX, U32 unitsAlongY, U32 unitsAlongZ, Mesh &mesh ) {
    if( mesh.vertexData != NULL ) {
        allocator.DeAllocate( mesh.vertexData );
        //delete [] mesh.vertexData;
        mesh.vertexData = NULL;
    }

    if( mesh.indexData != NULL ) {
        allocator.DeAllocate( mesh.indexData );
        //delete [] mesh.indexData;
        mesh.indexData = NULL;
    }

    if( mesh.subMeshData != NULL ) {
        allocator.DeAllocate( mesh.subMeshData );
        mesh.subMeshData = NULL;
    }

    // -( unitsAlongN / 2) [ORIGIN] +( unitsAlongN / 2)
    F32 xOver2 = static_cast<F32>( unitsAlongX / 2 );
    F32 yOver2 = static_cast<F32>( unitsAlongY / 2 );
    F32 zOver2 = static_cast<F32>( unitsAlongZ / 2 );

    // setup mesh verex data
    mesh.vertexCount = 24;
    mesh.vertexData  = reinterpret_cast<Vertex*>( allocator.Allocate( sizeof( Vertex ) * 24 ) ); //new Vertex[24];

    // front face
    mesh.vertexData[0]  = Vertex( -xOver2, -yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[1]  = Vertex( -xOver2, +yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[2]  = Vertex( +xOver2, +yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[3]  = Vertex( +xOver2, -yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // back face
    mesh.vertexData[4]  = Vertex( -xOver2, -yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[5]  = Vertex( +xOver2, -yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[6]  = Vertex( +xOver2, +yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[7]  = Vertex( -xOver2, +yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // top face
    mesh.vertexData[8]  = Vertex( -xOver2, +yOver2, -zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[9]  = Vertex( -xOver2, +yOver2, +zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[10] = Vertex( +xOver2, +yOver2, +zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[11] = Vertex( +xOver2, +yOver2, -zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // bottom face
    mesh.vertexData[12] = Vertex( -xOver2, -yOver2, -zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[13] = Vertex( +xOver2, -yOver2, -zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[14] = Vertex( +xOver2, -yOver2, +zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[15] = Vertex( -xOver2, -yOver2, +zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // left face
    mesh.vertexData[16] = Vertex( -xOver2, -yOver2, +zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[17] = Vertex( -xOver2, +yOver2, +zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[18] = Vertex( -xOver2, +yOver2, -zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[19] = Vertex( -xOver2, -yOver2, -zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // right face
    mesh.vertexData[20] = Vertex( +xOver2, -yOver2, -zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[21] = Vertex( +xOver2, +yOver2, -zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[22] = Vertex( +xOver2, +yOver2, +zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[23] = Vertex( +xOver2, -yOver2, +zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // setup mesh index data
    mesh.indexCount = 36;
    mesh.indexData  = reinterpret_cast<U32*>( allocator.Allocate( sizeof( U32 ) * 36 ) ); //new U32[36];

    // front face
    mesh.indexData[0] = 0; mesh.indexData[1] = 1; mesh.indexData[2] = 2;
    mesh.indexData[3] = 0; mesh.indexData[4] = 2; mesh.indexData[5] = 3;

    // back face
    mesh.indexData[6] = 4; mesh.indexData[7]  = 5; mesh.indexData[8]  = 6;
    mesh.indexData[9] = 4; mesh.indexData[10] = 6; mesh.indexData[11] = 7;

    // top face
    mesh.indexData[12] = 8; mesh.indexData[13]  = 9;  mesh.indexData[14] = 10;
    mesh.indexData[15] = 8; mesh.indexData[16]  = 10; mesh.indexData[17] = 11;

    // bottom face
    mesh.indexData[18] = 12; mesh.indexData[19]  = 13; mesh.indexData[20] = 14;
    mesh.indexData[21] = 12; mesh.indexData[22]  = 14; mesh.indexData[23] = 15;

    // left face
    mesh.indexData[24] = 16; mesh.indexData[25] = 17; mesh.indexData[26] = 18;
    mesh.indexData[27] = 16; mesh.indexData[28] = 18; mesh.indexData[29] = 19;

    // right face
    mesh.indexData[30] = 20; mesh.indexData[31] = 21; mesh.indexData[32] = 22;
    mesh.indexData[33] = 20; mesh.indexData[34] = 22; mesh.indexData[35] = 23;


    // setup the submesh info
    mesh.subMeshData = reinterpret_cast<SubMesh*>( allocator.Allocate( sizeof( SubMesh ) ) );
    mesh.subMeshData[0].subMeshId = mesh.subMeshCount++;
    mesh.subMeshData[0].materialId  = mesh.materialCount++;
    mesh.subMeshData[0].startVertex = 0;
    mesh.subMeshData[0].vertexCount = 24;
    mesh.subMeshData[0].startIndex  = 0;
    mesh.subMeshData[0].indexCount  = 36;

    // set the material data
    mesh.materialData = reinterpret_cast<Material*>( allocator.Allocate( sizeof( Material ) ) );
    SetToDefaultMaterial( mesh.materialData );

    mesh.isLoaded = true;
}

void GeoPrimitiveGenerator::GenerateCubeF( F32 unitsAlongX, F32 unitsAlongY, F32 unitsAlongZ, Mesh &mesh, bool useWorldOrigin ) {
    if( mesh.vertexData != NULL ) {
        allocator.DeAllocate( mesh.vertexData );
        //delete [] mesh.vertexData;
        mesh.vertexData = NULL;
    }

    if( mesh.indexData != NULL ) {
        allocator.DeAllocate( mesh.indexData );
        //delete [] mesh.indexData;
        mesh.indexData = NULL;
    }

    if( mesh.subMeshData != NULL ) {
        allocator.DeAllocate( mesh.subMeshData );
        mesh.subMeshData = NULL;
    }

    // -( unitsAlongN / 2) [ORIGIN] +( unitsAlongN / 2)
    F32 xOver2 = unitsAlongX, yOver2 = unitsAlongY, zOver2 = unitsAlongZ;
    if( useWorldOrigin == true ) {
        xOver2 = static_cast<F32>( unitsAlongX / 2 );
        yOver2 = static_cast<F32>( unitsAlongY / 2 );
        zOver2 = static_cast<F32>( unitsAlongZ / 2 );
    }

    // setup mesh verex data
    mesh.vertexCount = 24;
    mesh.vertexData  = reinterpret_cast<Vertex*>( allocator.Allocate( sizeof( Vertex ) * 24 ) ); //new Vertex[24];

    // front face
    mesh.vertexData[0]  = Vertex( -xOver2, -yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[1]  = Vertex( -xOver2, +yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[2]  = Vertex( +xOver2, +yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[3]  = Vertex( +xOver2, -yOver2, -zOver2, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // back face
    mesh.vertexData[4]  = Vertex( -xOver2, -yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[5]  = Vertex( +xOver2, -yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[6]  = Vertex( +xOver2, +yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[7]  = Vertex( -xOver2, +yOver2, +zOver2, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // top face
    mesh.vertexData[8]  = Vertex( -xOver2, +yOver2, -zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[9]  = Vertex( -xOver2, +yOver2, +zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[10] = Vertex( +xOver2, +yOver2, +zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[11] = Vertex( +xOver2, +yOver2, -zOver2, 0.0f, 1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // bottom face
    mesh.vertexData[12] = Vertex( -xOver2, -yOver2, -zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[13] = Vertex( +xOver2, -yOver2, -zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[14] = Vertex( +xOver2, -yOver2, +zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[15] = Vertex( -xOver2, -yOver2, +zOver2, 0.0f, -1.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // left face
    mesh.vertexData[16] = Vertex( -xOver2, -yOver2, +zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[17] = Vertex( -xOver2, +yOver2, +zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[18] = Vertex( -xOver2, +yOver2, -zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[19] = Vertex( -xOver2, -yOver2, -zOver2, -1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // right face
    mesh.vertexData[20] = Vertex( +xOver2, -yOver2, -zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[21] = Vertex( +xOver2, +yOver2, -zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[22] = Vertex( +xOver2, +yOver2, +zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );
    mesh.vertexData[23] = Vertex( +xOver2, -yOver2, +zOver2, 1.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 1.0f );

    // setup mesh index data
    mesh.indexCount = 36;
    mesh.indexData  = reinterpret_cast<U32*>( allocator.Allocate( sizeof( U32 ) * 36 ) ); //new U32[36];

    // front face
    mesh.indexData[0] = 0; mesh.indexData[1] = 1; mesh.indexData[2] = 2;
    mesh.indexData[3] = 0; mesh.indexData[4] = 2; mesh.indexData[5] = 3;

    // back face
    mesh.indexData[6] = 4; mesh.indexData[7]  = 5; mesh.indexData[8]  = 6;
    mesh.indexData[9] = 4; mesh.indexData[10] = 6; mesh.indexData[11] = 7;

    // top face
    mesh.indexData[12] = 8; mesh.indexData[13]  = 9;  mesh.indexData[14] = 10;
    mesh.indexData[15] = 8; mesh.indexData[16]  = 10; mesh.indexData[17] = 11;

    // bottom face
    mesh.indexData[18] = 12; mesh.indexData[19]  = 13; mesh.indexData[20] = 14;
    mesh.indexData[21] = 12; mesh.indexData[22]  = 14; mesh.indexData[23] = 15;

    // left face
    mesh.indexData[24] = 16; mesh.indexData[25] = 17; mesh.indexData[26] = 18;
    mesh.indexData[27] = 16; mesh.indexData[28] = 18; mesh.indexData[29] = 19;

    // right face
    mesh.indexData[30] = 20; mesh.indexData[31] = 21; mesh.indexData[32] = 22;
    mesh.indexData[33] = 20; mesh.indexData[34] = 22; mesh.indexData[35] = 23;


    // setup the submesh info
    mesh.subMeshData = reinterpret_cast<SubMesh*>( allocator.Allocate( sizeof( SubMesh ) ) );
    mesh.subMeshData[0].subMeshId = mesh.subMeshCount++;
    mesh.subMeshData[0].materialId  = mesh.materialCount++;
    mesh.subMeshData[0].startVertex = 0;
    mesh.subMeshData[0].vertexCount = 24;
    mesh.subMeshData[0].startIndex  = 0;
    mesh.subMeshData[0].indexCount  = 36;

    mesh.isLoaded = true;

    // set material info (set to default material)
}

/*
================
GeoPrimitiveGenerator::GenerateGrid

Create an m * n grid
================
*/
void GeoPrimitiveGenerator::GenerateGrid( U32 unitsAlongX, U32 unitsAlongZ, U32 vertsAlongX, U32 vertsAlongZ, Mesh &mesh ) {
    if( mesh.vertexData != NULL ) {
        allocator.DeAllocate( mesh.vertexData );
        //delete [] mesh.vertexData;
        mesh.vertexData = NULL;
    }

    if( mesh.indexData != NULL ) {
        allocator.DeAllocate( mesh.indexData );
        //delete [] mesh.indexData;
        mesh.indexData = NULL;
    }

    if( mesh.subMeshData != NULL ) {
        allocator.DeAllocate( mesh.subMeshData );
        mesh.subMeshData = NULL;
    }


    U32 vertexCount = vertsAlongX*vertsAlongZ;
    U32 faceCount   = (vertsAlongX-1)*(vertsAlongZ-1)*2;

    F32 halfWidth = 0.5f*unitsAlongX;
    F32 halfDepth = 0.5f*unitsAlongZ;

    F32 dx = static_cast<F32>( unitsAlongX / ( vertsAlongZ-1 ) );
    F32 dz = static_cast<F32>( unitsAlongZ / ( vertsAlongX-1 ) );

    F32 du = 1.0f / (vertsAlongX-1);
    F32 dv = 1.0f / (vertsAlongZ-1);

    // setup mesh verex data
    mesh.vertexData = reinterpret_cast<Vertex*>( allocator.Allocate( sizeof( Vertex ) * vertexCount ) ); // new Vertex[vertexCount];
    for(U32 i = 0; i < vertsAlongX; ++i) {
        F32 z = halfDepth - i*dz;
        for(U32 j = 0; j < vertsAlongZ; ++j) {
            F32 x = -halfWidth + j*dx;
            mesh.vertexData[i*vertsAlongZ+j] = Vertex( x, 0.0f, z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ); // texture co-ords?
        }
    }
 
    // setup mesh index data
    // 3 indices per face
    mesh.indexData = reinterpret_cast<U32*>( allocator.Allocate( sizeof( U32 ) * ( faceCount * 3 ) ) ); //new U32[faceCount * 3];

    // Iterate over each quad and compute indices.
    U32 k = 0;
    for(U32 i = 0; i < vertsAlongX-1; ++i) {
        for(U32 j = 0; j < vertsAlongZ-1; ++j) {
            mesh.indexData[k]   = i*vertsAlongZ+j;
            mesh.indexData[k+1] = i*vertsAlongZ+j+1;
            mesh.indexData[k+2] = (i+1)*vertsAlongZ+j;

            mesh.indexData[k+3] = (i+1)*vertsAlongZ+j;
            mesh.indexData[k+4] = i*vertsAlongZ+j+1;
            mesh.indexData[k+5] = (i+1)*vertsAlongZ+j+1;

            k += 6; // next quad
        }
    }

    // setup the submesh info
    mesh.subMeshData = reinterpret_cast<SubMesh*>( allocator.Allocate( sizeof( SubMesh ) ) );
    mesh.subMeshData[0].subMeshId   = mesh.subMeshCount++;
    mesh.subMeshData[0].materialId  = mesh.materialCount++;
    mesh.subMeshData[0].startVertex = 0;
    mesh.subMeshData[0].vertexCount = vertexCount;
    mesh.subMeshData[0].startIndex  = 0;
    mesh.subMeshData[0].indexCount  = faceCount * 3;;

    mesh.isLoaded = true;

    // set material info
}
