/*
===============================================================================

	ReflecTech
	==========
	File		:	RtMesh.h
	Author		:	Jamie Taylor
	Last Edit	:	31/07/13
	Desc		:	Describes a mesh used by the engine. Contents can be set
					by parsing a .obj|&.rtm file or using the GeoPrimitiveGenerator.

===============================================================================
*/


#ifndef RT_MESH_H
#define RT_MESH_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../CoreSystems/RtHeapAllocator.h"
//#include "../../CoreSystems/RtTokenizer.h"
#include "temptok.h"
#include "RtVertex.h"
#include "RtMaterial.h"
#include "../../Collision&Phsysics/RtAxisAlignedBox.h"


// TEMP: replace with platform netural math code once things are up and running
#include<xnamath.h>


/*
===============================================================================

SubMesh struct, a mesh is effectively made up of a collection of submeshes (aka subsets),
each sub mesh desribes a subset of the overall mesh and the material to be applied.

===============================================================================
*/
struct SubMesh {
	SubMesh( void ) { 
		subMeshId = materialId = -1; 
		startVertex = vertexCount = startIndex = indexCount = 0; 
	}

	U32 subMeshId;
	U32 materialId;

	U32 startVertex;
	U32 vertexCount;
	U32 startIndex;
	U32 indexCount;
};


/*
===============================================================================

Mesh class

===============================================================================
*/
class Mesh {
public:
				   // geoPrimGen can set the meshes data directly
	friend class   GeoPrimitiveGenerator;

				   Mesh( void );
				   ~Mesh( void );

				   // load from OBJ file (switch to RTM later, stay with OBJ so we can focus on other things - AABB etc...)
	bool		   LoadFromObjFile( const I8 *fileName, bool rightHanded );
	void		   Release( void );

	U32			   GetVertexCount( void ) const;
	Vertex		 * GetVertexData( void ) const;

	U32			   GetIndexCount( void ) const;
	U32			 * GetIndexData( void ) const;

	U32			   GetMaterialCount( void ) const;
	Material     * GetMaterialData( void ) const;

	U32		       GetSubMeshCount( void ) const;
	SubMesh		 * GetSubMeshData( void ) const;

	XMMATRIX     * GetWorldMatrix( void ) const;

				   // transformations
	void		   Translate( F32 xTranslation, F32 yTranslation, F32 zTranslation );

	void		   CalculateBoundingVolume( void );
				   // TEMP?
	AxisAlignedBox * TempGetBoundingVolume( void ) const;

	bool		   IsRightHanded( void ) const;

	bool		   IsLoaded( void ) const;

private:
	HeapAllocator<void> allocator;

	// made static to 'fix' lnk2019 mesh::tokenizer( ) and ~( )
	// feels like 'fixing' a symptom and not the genuine issue
	Tokenizer	   tokenizer;

	U32			   vertexCount;
	Vertex		 * vertexData;
	
	U32			   indexCount;
	U32			 * indexData;
	
	U32			   subMeshCount;
	SubMesh		 * subMeshData;

	U32			   materialCount;
	Material	 * materialData;

	// TEMP:	   replace with platform neutral math code once things are up and running
	XMMATRIX       worldMatrix;

	AxisAlignedBox boundingBox;

	bool		   isRightHanded;

	bool		   isLoaded;

	bool		   LoadMaterialFile( const I8 *fileName );
};


#endif // RT_MESH_H