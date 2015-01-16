/*
===============================================================================

	ReflecTech
	==========
	File		:	RtMesh.cpp
	Author		:	Jamie Taylor
	Last Edit	:	31/07/13
	Desc		:	Describes a mesh used by the engine. Contents can be set
					by parsing a .rtm file or using the GeoPrimitiveGenerator.

===============================================================================
*/


#include "RtMesh.h"


/*
================
Mesh::Mesh
================
*/
Mesh::Mesh( void ) {
	vertexCount = 0;
	vertexData  = NULL;

	indexCount  = 0;
	indexData   = NULL;

	subMeshCount = 0;
	subMeshData  = NULL;

	materialCount = 0;
	materialData  = NULL;

	// TEMP
	worldMatrix = XMMatrixIdentity( );

	isRightHanded = false;

	isLoaded = false;
}

/*
================
Mesh::~Mesh
================
*/
Mesh::~Mesh( void ) {
	Release( );
}

/*
================
Mesh::LoadFromObjFile

Modelling packages tend to work in right-handed systems, D3D for one uses a left-handed system,
so some changes will be necessary to make sure geometry, texture and normals is/are handled correctly.

NOTE: Will replace the use of .obj files with .rtm files eventually 
but I've still lots to get on with/get done.
================
*/
bool Mesh::LoadFromObjFile( const I8 *fileName, bool rightHanded ) {
	isRightHanded = rightHanded;

	// Will move this file operation code to a minor platform neutral file 'helper' class
	// -------------------------------------------------------------------------------
	// load the file
	HANDLE fileHwnd = CreateFile( fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( fileHwnd == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	// read in the file
	LARGE_INTEGER fileSize;
	GetFileSizeEx( fileHwnd, &fileSize );

	I8 *fileBuffer = reinterpret_cast<I8*>( allocator.Allocate( sizeof( I8 ) * fileSize.QuadPart ) );
	U32 bytesRead = 0;

	if( ReadFile( fileHwnd, reinterpret_cast<void*>( &fileBuffer[0] ), fileSize.QuadPart,
		          reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
		CloseHandle( fileHwnd );
		Release( );
		return false;
	}

	CloseHandle( fileHwnd );
	// -------------------------------------------------------------------------------

	// parse the file buffer
	I8 delimiters[2] = { ' ', '/' };
	tokenizer.SetBuffer( ( &fileBuffer[0] ), fileSize.QuadPart );

	const U32 TOKEN_BUFFER_SIZE = 64;
	I8 tokenBuffer[TOKEN_BUFFER_SIZE] = { 0 };

	// for material parsing
	I8 materialFile[64] = { 0 };
	bool includesMaterial = false;

	// in an attempt to avoid structures like vectors that do frequent dynamic allocation, we
	// go through the file to work out the number of vertices etc and then make one allocation
	// for each buffer
	U32 TEXTURE_COORDINATE_COUNT = 0, VERTEX_NORMAL_COUNT = 0, FACE_COUNT = 0;

	while( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) {
		if( strcmp( tokenBuffer, "v" ) == 0 ) {
 			++vertexCount;
		}

		if( strcmp( tokenBuffer, "vt" ) == 0 ) {
			++TEXTURE_COORDINATE_COUNT;
		}

		if( strcmp( tokenBuffer, "vn" ) == 0 ) {
			++VERTEX_NORMAL_COUNT;
		}

		if( strcmp( tokenBuffer, "f" ) == 0 ) {
			++FACE_COUNT;
		}

		if( strcmp( tokenBuffer, "mtllib" ) == 0 ) {
			includesMaterial = true;

			// store the material file name
			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			strcpy( materialFile, tokenBuffer );
		}

        if( strcmp( tokenBuffer, "usemtl" ) == 0 ) {
            ++materialCount;
        }

		memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
	} // while( )
	tokenizer.ResetBuffer( );

	// work out how many components make a up a vector normal, texture coordinate and face
	U32 VERTEX_NORMAL_COMPONENT_COUNT = 0, TEXTURE_COORDINATE_COMPONENT_COUNT = 0, FACE_COMPONENT_COUNT = 0;

	while( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) {
		if( ( strcmp( tokenBuffer, "vt" ) == 0 ) && ( TEXTURE_COORDINATE_COMPONENT_COUNT == 0 ) ) {
			while( ( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) && 
				   ( strcmp( tokenBuffer, "" ) != 0 ) && ( strcmp( tokenBuffer, "vt" ) != 0 ) ) {
				++TEXTURE_COORDINATE_COMPONENT_COUNT;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
			}
		}

		if( ( strcmp( tokenBuffer, "vn" ) == 0 )  && ( VERTEX_NORMAL_COMPONENT_COUNT == 0 ) ) {
			while( ( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) && 
				   ( strcmp( tokenBuffer, "" ) != 0 ) && ( strcmp( tokenBuffer, "vn" ) != 0 ) ) {
				++VERTEX_NORMAL_COMPONENT_COUNT;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
			}
		}

		if( ( strcmp( tokenBuffer, "f" ) == 0 ) && ( FACE_COMPONENT_COUNT == 0 ) ) {
			while( ( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) &&
				   ( strcmp( tokenBuffer, "" ) != 0 ) && ( strcmp( tokenBuffer, "f" ) != 0 ) ) {
				++FACE_COMPONENT_COUNT;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
			}
		}

		memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
	} // while( )
	tokenizer.ResetBuffer( );

	// we now setup a few temporary buffers and then go through the file again (the first time was
	// to get the sizes for these buffers) and read in the actual model data

	// sets vertex info for the subsets, should look into getting rid of this but will keep it around
	// for now, still loads to do (AABB, picking, editor etc..)
	U32 *tempVertexOffsets = reinterpret_cast<U32*>( allocator.Allocate( sizeof( U32 ) * ( materialCount + 1 ) ) );

	F32 *tempVertexData = reinterpret_cast<F32*>( allocator.Allocate( sizeof( F32 ) * vertexCount * 3 ) );
	U32 *tempFaceData   = reinterpret_cast<U32*>( allocator.Allocate( sizeof( U32 ) * FACE_COUNT * FACE_COMPONENT_COUNT ) );

	F32 *tempTextureCoordinateData = NULL;
	if( TEXTURE_COORDINATE_COMPONENT_COUNT > 0 ) {
		tempTextureCoordinateData = reinterpret_cast<F32*>( allocator.Allocate( sizeof( F32 ) * TEXTURE_COORDINATE_COUNT * TEXTURE_COORDINATE_COMPONENT_COUNT ) );
	}

	F32 *tempVertexNormalData = NULL;
	if( VERTEX_NORMAL_COMPONENT_COUNT > 0 ) {
		tempVertexNormalData = reinterpret_cast<F32*>( allocator.Allocate( sizeof( F32 ) * VERTEX_NORMAL_COUNT * VERTEX_NORMAL_COMPONENT_COUNT ) );
	}

	subMeshCount = materialCount;
	subMeshData = reinterpret_cast<SubMesh*>( allocator.Allocate( sizeof( SubMesh ) * subMeshCount ) );
	materialData = reinterpret_cast<Material*>( allocator.Allocate( sizeof( Material ) * materialCount ) );

	// reset
	vertexCount = 0;
	TEXTURE_COORDINATE_COUNT = 0;
	VERTEX_NORMAL_COUNT = 0;

	U32 mtrlCount = 0, faceCount = 0, materialNumber = 0;
	while( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) {
		// added for material parsing
		if( strcmp( tokenBuffer, "usemtl" ) == 0 ) {
			// old material related code
			U32 temp = ( faceCount / ( FACE_COMPONENT_COUNT / 3 ) );
			tempVertexOffsets[mtrlCount] = temp;
			++mtrlCount;

			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			strcpy( materialData[materialNumber].materialName, tokenBuffer );
			++materialNumber;
        }

		if( strcmp( tokenBuffer, "v" ) == 0 ) {
			for( U32 i=0; i<3; ++i ) {
				tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
				if( ( i == 2 ) && ( rightHanded == true ) ) {
					tempVertexData[vertexCount] = -1.0f * atof( &tokenBuffer[0] );
				}
				else {
					tempVertexData[vertexCount] = atof( &tokenBuffer[0] );
				}
				++vertexCount;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
			} // for( )	
		} // if( )

		if( strcmp( tokenBuffer, "vt" ) == 0 && ( TEXTURE_COORDINATE_COMPONENT_COUNT > 0 ) ) {	
			for( U32 i=0; i<TEXTURE_COORDINATE_COMPONENT_COUNT; ++i ) {
				tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
				if( ( i == 1 ) && ( rightHanded == true ) ) {
					tempTextureCoordinateData[TEXTURE_COORDINATE_COUNT] = 1 - atof( &tokenBuffer[0] );
				}
				else {
					tempTextureCoordinateData[TEXTURE_COORDINATE_COUNT] =  atof( &tokenBuffer[0] );
				}
				++TEXTURE_COORDINATE_COUNT;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
			} // for( )
		} // if( )
		
		if( strcmp( tokenBuffer, "vn" ) == 0 && ( VERTEX_NORMAL_COMPONENT_COUNT > 0 ) ) {	
			for( U32 i=0; i<VERTEX_NORMAL_COMPONENT_COUNT; ++i ) {
				tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
				if( ( i == 2 ) && ( rightHanded == true ) ) {
					tempVertexNormalData[VERTEX_NORMAL_COUNT] = -1.0f *  atof( &tokenBuffer[0] );
				}
				else {
					tempVertexNormalData[VERTEX_NORMAL_COUNT] = atof( &tokenBuffer[0] );
				}
				++VERTEX_NORMAL_COUNT;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );		
			} // for( )
		} // if( )

		if( strcmp( tokenBuffer, "f" ) == 0 ) {	
			for( U32 i=0; i<FACE_COMPONENT_COUNT; ++i ) {
                tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 );
				tempFaceData[faceCount] = atoi( &tokenBuffer[0] );
				++faceCount;
				memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );	
			}		
		} // if( )

		memset( tokenBuffer, 0, TOKEN_BUFFER_SIZE );
	} // while( )

	// done with the buffer, can be released now
	tokenizer.ReleaseBuffer( );
	//tokenizer->ResetBuffer( );

	// -------------------------------------------------------------------------------
	// arrange the loaded .obj information int a list of triangles suitable for rendering
	// will this work with openGL? - investigate
	vertexCount = FACE_COUNT * 3;

	vertexData = reinterpret_cast<Vertex*>( allocator.Allocate( sizeof( Vertex ) * vertexCount ) );

	tempVertexOffsets[materialCount] = vertexCount;

	// -1 because .obj indexing starts at 1, c/c++ arrays start at 0
	U32 vIndex = 0, countTo = ( FACE_COUNT * FACE_COMPONENT_COUNT ), step = ( FACE_COMPONENT_COUNT / 3 );
	for( U32 i=0, vertexIndex=0; i<countTo; i+=step, ++vertexIndex ) {
		vertexData[vertexIndex].position[0] = tempVertexData[( tempFaceData[i] - 1 ) * 3 + 0];
		vertexData[vertexIndex].position[1] = tempVertexData[( tempFaceData[i] - 1 ) * 3 + 1];
		vertexData[vertexIndex].position[2] = tempVertexData[( tempFaceData[i] - 1 ) * 3 + 2];

		if( TEXTURE_COORDINATE_COMPONENT_COUNT > 0 ) {
			for( U32 j=0; j<TEXTURE_COORDINATE_COMPONENT_COUNT; ++j ) {
				vertexData[vertexIndex].textureCoordinates[j] = tempTextureCoordinateData[( tempFaceData[i + 1] - 1 ) * TEXTURE_COORDINATE_COMPONENT_COUNT + j];
			}	
		}

		if( VERTEX_NORMAL_COMPONENT_COUNT > 0 ) {
			for( U32 j=0; j<VERTEX_NORMAL_COMPONENT_COUNT; ++j ) {
				vertexData[vertexIndex].normal[j] = tempVertexNormalData[( tempFaceData[i + 2] - 1 ) * VERTEX_NORMAL_COMPONENT_COUNT + j];
			}	
		}

		vertexData[vertexIndex].diffuseColour[0] = 0.75f; vertexData[vertexIndex].diffuseColour[1] = 0.75f;
		vertexData[vertexIndex].diffuseColour[2] = 0.75f; vertexData[vertexIndex].diffuseColour[3] = 1.0f;
    } // for( )
	// -------------------------------------------------------------------------------

	// finished, release the temporary buffers allocated earlier
	allocator.DeAllocate( fileBuffer );
	fileBuffer = NULL;

	allocator.DeAllocate( tempVertexData );
	tempVertexData = NULL;

	allocator.DeAllocate( tempFaceData );
	tempFaceData = NULL;

	if( TEXTURE_COORDINATE_COMPONENT_COUNT > 0 ) {
		allocator.DeAllocate( tempTextureCoordinateData );
		tempTextureCoordinateData = NULL;
	}

	if( VERTEX_NORMAL_COMPONENT_COUNT > 0 ) {
		allocator.DeAllocate( tempVertexNormalData );
		tempVertexNormalData = NULL;
	}

	// load and set materials
	bool b = false;
	if( includesMaterial == true ) {
		b = LoadMaterialFile( materialFile );
	}

	// if the mesh doesn't specify a material file or we had issues in loading &| parsing
	// it, we use the default material
	if( includesMaterial == false || b == false ) {
		for( U32 i=0; i<materialCount; ++i ) {
			SetToDefaultMaterial( &materialData[i] );
			if( isRightHanded == true ) {
				materialData[i].renderState = MATERIAL_RENDER_STATE::SOLID_RH;
			}
		}
	}

	// set the submesh data
	for( U32 i=0; i<subMeshCount; ++i ) {
		subMeshData[i].subMeshId = i;

		subMeshData[i].startVertex  = tempVertexOffsets[i];
		subMeshData[i].vertexCount  = tempVertexOffsets[i+1] - tempVertexOffsets[i];

		subMeshData[i].startIndex = -1;
		subMeshData[i].indexCount = -1;
	}
	// current obj/model loading/parsing code doesn't utilise indexing yet 
	// so we set index info to -1
	indexCount = -1;

	allocator.DeAllocate( tempVertexOffsets );
	tempVertexOffsets = NULL;

	// finish by calculating bounding volume
	CalculateBoundingVolume( );

	isLoaded = true;

	// TEMP: Added for Booker mesh, need to set mesh to world origin in some automated way...
	worldMatrix *= XMMatrixTranslation( 0.0f, -boundingBox.centerY, 0.0f );

	return true;
}

/*
================
Mesh::Release
================
*/
void Mesh::Release( void ) {
	if( vertexData != NULL ) {
		allocator.DeAllocate( vertexData );
		vertexData = NULL;
	}

	if( indexData != NULL ) {
		indexData = NULL;
	}

	if( subMeshData != NULL ) {
		allocator.DeAllocate( subMeshData );
		subMeshData = NULL;
	}

	if( materialData != NULL ) {
		allocator.DeAllocate( materialData );
		materialData = NULL;
	}

	vertexCount = 0;
	indexCount  = 0;
	
	subMeshCount = 0;
	materialCount = 0;

	// TEMP
	worldMatrix = XMMatrixIdentity( );

	isLoaded = false;
}

/*
================
Mesh::GetVertexCount
================
*/
U32 Mesh::GetVertexCount( void ) const {
	return vertexCount;
}

/*
================
Mesh::GetVertexData
================
*/
Vertex* Mesh::GetVertexData( void ) const {
	return const_cast<Vertex*>( vertexData );
}

/*
================
Mesh::GetIndexCount
================
*/
U32 Mesh::GetIndexCount( void ) const {
	return indexCount;
}

/*
================
Mesh::GetIndexData
================
*/
U32* Mesh::GetIndexData( void ) const {
	return const_cast<U32*>( indexData );
}

/*
================
Mesh::GetMaterialCount
================
*/
U32 Mesh::GetMaterialCount( void ) const {
	return materialCount;
}

/*
================
Mesh::GetMaterialCount
================
*/
Material* Mesh::GetMaterialData( void ) const {
	return const_cast<Material*>( materialData );
}

/*
================
Mesh::GetSubMeshCount
================
*/
U32 Mesh::GetSubMeshCount( void ) const {
	return subMeshCount;
}

/*
================
Mesh::GetSubMeshCount
================
*/
SubMesh* Mesh::GetSubMeshData( void ) const {
	return const_cast<SubMesh*>( subMeshData );
}

/*
================
Mesh::GetWorldMatrix
================
*/
XMMATRIX* Mesh::GetWorldMatrix( void ) const {
	return const_cast<XMMATRIX*>( &worldMatrix );
}

void Mesh::Translate( F32 xTranslation, F32 yTranslation, F32 zTranslation ) {
	XMMATRIX translation = XMMatrixTranslation( xTranslation, yTranslation, zTranslation );
	worldMatrix *= translation;
}

/*
================
Mesh::CalculateBoundingVolume
================
*/
void Mesh::CalculateBoundingVolume( void ) {
	for( U32 i=0; i<vertexCount; ++i ) {
		// check x position
		if( vertexData[i].position[0] < boundingBox.minX ) {
			boundingBox.minX = vertexData[i].position[0];
		}
		else if ( vertexData[i].position[0] > boundingBox.maxX ) {
			boundingBox.maxX = vertexData[i].position[0];
		}

		// check y position
		if( vertexData[i].position[1] < boundingBox.minY ) {
			boundingBox.minY = vertexData[i].position[1];
		}
		else if ( vertexData[i].position[1] > boundingBox.maxY ) {
			boundingBox.maxY = vertexData[i].position[1];
		}

		// check z position
		if( vertexData[i].position[2] < boundingBox.minZ ) {
			boundingBox.minZ = vertexData[i].position[2];
		}
		else if ( vertexData[i].position[2] > boundingBox.maxZ ) {
			boundingBox.maxZ = vertexData[i].position[2];
		}
	}

	boundingBox.centerX = ( boundingBox.maxX - boundingBox.minX ) / 2.0f;
	boundingBox.centerY = ( boundingBox.maxY - boundingBox.minY ) / 2.0f;
	boundingBox.centerZ = ( boundingBox.maxZ - boundingBox.minZ ) / 2.0f;
}

// TEMP
AxisAlignedBox* Mesh::TempGetBoundingVolume( void ) const {
	return const_cast<AxisAlignedBox*>( &boundingBox );
}

/*
================
Mesh::IsRightHanded
================
*/
bool Mesh::IsRightHanded( void ) const {
	return isRightHanded;
}

/*
================
Mesh::IsLoaded
================
*/
bool Mesh::IsLoaded( void ) const {
	return isLoaded;
}

/*
================
Mesh::LoadMaterialFile
================
*/
bool Mesh::LoadMaterialFile( const I8 *fileName ) {
	// load the file
	HANDLE fileHwnd = CreateFile( fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( fileHwnd == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	// read in the file
	LARGE_INTEGER fileSize;
	GetFileSizeEx( fileHwnd, &fileSize );

	I8 *fileBuffer = reinterpret_cast<I8*>( allocator.Allocate( sizeof( I8 ) * fileSize.QuadPart ) );
	U32 bytesRead = 0;

	if( ReadFile( fileHwnd, reinterpret_cast<void*>( &fileBuffer[0] ), fileSize.QuadPart,
		          reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
		CloseHandle( fileHwnd );
		Release( );
		return false;
	}

	CloseHandle( fileHwnd );

	// parse the material
	I8 delimiters[2] = { ' ', '/' };
	tokenizer.SetBuffer( ( &fileBuffer[0] ), fileSize.QuadPart );

	I8 tokenBuffer[64] = { 0 };

	// fill in the material properties
	I32 i = 0, j = 0;
	while( tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 2 ) == true ) {
	
		if( strcmp( tokenBuffer, "newmtl" ) == 0 ) {
            // get the relevant material
			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			for( U32 i=0; i<materialCount; ++i ) {
				if( strcmp( materialData[i].materialName, &tokenBuffer[0] ) == 0 ) {
					j = i;
					break;
				}
			}
		}

		if( strcmp( tokenBuffer, "Ka" ) == 0 ) {
			for( U32 i=0; i<3; ++i ) {
				tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
				materialData[j].ambientColour[i] = atof( &tokenBuffer[0] );
				memset( tokenBuffer, 0, 64 );
			}
		}

		if( strcmp( tokenBuffer, "Kd" ) == 0 ) {
			for( U32 i=0; i<3; ++i ) {
				tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
				materialData[j].diffuseColour[i] = atof( &tokenBuffer[0] );
				memset( tokenBuffer, 0, 64 );
			}
		}

		if( strcmp( tokenBuffer, "Ks" ) == 0 ) {
			for( U32 i=0; i<3; ++i ) {
				tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
				materialData[j].specularColour[i] = atof( &tokenBuffer[0] );
				memset( tokenBuffer, 0, 64 );
			}
		}

		if( strcmp( tokenBuffer, "Ns" ) == 0 ) {
			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			materialData[j].specularCoefficient = atof( &tokenBuffer[0] );
		}

		if( strcmp( tokenBuffer, "map_Kd" ) == 0 ) {
			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			strcpy( materialData[j].diffuseMapName, &tokenBuffer[0] );
		}

		if( strcmp( tokenBuffer, "map_Ks" ) == 0 ) {
			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			strcpy( materialData[j].specularMapName, &tokenBuffer[0] );
		}

		if( strcmp( tokenBuffer, "map_bump" ) == 0 || strcmp( tokenBuffer, "bump" ) == 0 ) {
			tokenizer.GetNextToken( ( &tokenBuffer[0] ), delimiters, 1 );
			strcpy( materialData[j].normalMapName, &tokenBuffer[0] );
		}

		memset( tokenBuffer, 0, 64 );
	}
	
	tokenizer.ReleaseBuffer( );
	allocator.DeAllocate( fileBuffer );

	return true;
}