/**************************************************************************************************
	Module:       CImportXFile.h
	Author:       Laurent Noel
	Date created: 11/10/05

	Class encapsulating the import of a Microsoft DirectX .X file

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#ifndef GEN_C_IMPORT_XFILE_H_INCLUDED
#define GEN_C_IMPORT_XFILE_H_INCLUDED

#include <vector>
using namespace std;
#include <d3d9.h>
#include <d3dx9.h>

#include "CVector3.h"
#include "CMatrix4x4.h"
#include "Mesh.h"

namespace gen
{

// List of errors returned from import functions
enum EImportError
{
	kSuccess           = 0,
	kSystemFailure     = 1,
	kOutOfSystemMemory = 2,
	kFileError         = 3,
	kInvalidData       = 4,
};


class CImportXFile
{
	GEN_CLASS( CImportXFile )

/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/
public:
	// Constructor
	CImportXFile()
	{
		m_bImported = false;
	}

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	CImportXFile( const CImportXFile& );
	CImportXFile& operator=( const CImportXFile& );


/*-----------------------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------------------*/
public:

	/////////////////////////////////////
	// File import

	// Return import status
	bool IsImported()
	{
		return m_bImported;
	}

	// Import a Microsoft X-File into a list of meshes and a frame hierarchy
	// Possible return values:
	//		kSuccess:			...
	//		kFileError:			Missing file or not an X-file
	//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
	//		kOutOfSystemMemory:	...
	//		kSystemFailure:		X-file API failure
	EImportError ImportFile
	(
		const string& sXName
	);


	/////////////////////////////////////
	// Data access

	// Get number of nodes in the mesh hierarchy (frames in an X-File)
	TUInt32 GetNumNodes() const
	{
		return static_cast<TUInt32>(m_Frames.size());
	}

	// Get a single node from the mesh hierarchy (a frame in an X-File), returned through a pointer
	void GetNode
	(
		const TUInt32    iNode,
		SMeshNode* const pNode
	) const;


	// Get number of sub-meshes in the mesh hierarchy (meshes in an X-File)
	TUInt32 GetNumSubMeshes() const
	{
		return static_cast<TUInt32>(m_Meshes.size());
	}

	// Get the render method used for the given sub-mesh
	ERenderMethod GetSubMeshRenderMethod( const TUInt32 iSubMesh ) const;
		
	// Get the specification and data for given submesh, returned through a pointer. May request
	// tangents to be calculated
	// Possible return values:
	//		kSuccess:			...
	//		kOutOfSystemMemory:	...
	EImportError CImportXFile::GetSubMesh
	(
		const TUInt32 iSubMesh,
		SSubMesh*     pSubMesh,
		bool          bTangents = false
	) const;


	// Get the number of materials used in the mesh (across all submeshes - i.e. in all meshes
	// in an X-File)
	TUInt32 GetNumMaterials() const
	{
		return static_cast<TUInt32>(m_Materials.size());
	}

	// Get specification of a given material, returned through a pointer
	// The render method of a material specifies how to draw geometry with this material
	void GetMaterial
	(
		const TUInt32        iMaterial,
		SMeshMaterial* const pMaterial
	) const;


	// TODO: bones


/*-----------------------------------------------------------------------------------------
	Extra public interface for CImportXFile
-----------------------------------------------------------------------------------------*/
public:

	// Tests if supplied filename is a Microsoft X-File
	// Possible return values:
	//		kSuccess:			...
	//		kFileError:			Missing file or not an X-file
	static bool IsXFile
	(
		const string& sXName
	);


/*-----------------------------------------------------------------------------------------
	Private interface
-----------------------------------------------------------------------------------------*/
private:

	/////////////////////////////////////
	// X-File types

	// Container types used
	typedef vector<TUInt32>  TXFileInts;
	typedef vector<CVector3> TXFileVectors;

	// Single face in an X-file - three vertex indices (will convert all faces to triangles)
	struct SXFileFace
	{
		TUInt32 aiVertex[3];
	};
	typedef vector<SXFileFace> TXFileFaces;


	// 2D texture coordinate in an X-file
	struct SXFileUV
	{
		TFloat32 fU;
		TFloat32 fV;
	};
	typedef vector<SXFileUV> TXFileUVs;


	// RGB colour used in structures below
	struct SXFileRGBColour
	{
		TFloat32 fRed;
		TFloat32 fGreen;
		TFloat32 fBlue;
	};

	// RGBA colour used in structures below
	struct SXFileRGBAColour
	{
		TFloat32 fRed;
		TFloat32 fGreen;
		TFloat32 fBlue;
		TFloat32 fAlpha;
	};
	typedef vector<SXFileRGBAColour> TXFileRGBAColours;


	// Material used in an X-file, material name, diffuse, specular and emmisive colours and a
	// single (diffuse) texture
	struct SXFileMaterial
	{
		string           sName;
		SXFileRGBAColour faceColour;
		TFloat32         fSpecularPower;
		SXFileRGBColour  specularColour;
		SXFileRGBColour  emmisiveColour;
		string           sTextureName;
	};
	typedef vector<SXFileMaterial> TXFileMaterials;

	// Equality operator for SXFileMaterial structure (needed for searching material lists)
	friend bool operator==
	(
		const SXFileMaterial& cmp1,
		const SXFileMaterial& cmp2
	);


	// Single bone weight as used in the bone structure below, contains the index of the affected
	// vertex and the weight that the bone applies to that vertex
	struct SXFileBoneWeight
	{
		TUInt32  iVertexIndex;
		TFloat32 fWeight;
	};
	typedef vector<SXFileBoneWeight> TXFileBoneWeights;

	// Bone structure in an X-file
	struct SXFileBone
	{
		// Container types used

		string            sFrameName;   // Name of the frame that drives this bone
		TUInt32           iFrame;       // Index of the frame that drives this bone
		TXFileBoneWeights weights;
		CMatrix4x4        offsetMatrix; // TODO: Would like aligned matrices - but vector can't do it

	};
	typedef vector<SXFileBone>       TXFileBones;


	// Frame in an X-file hierarchy
	struct SXFileFrame
	{
		string     sName;
		TUInt32    iDepth;
		TUInt32    iParentIndex;
		TUInt32    iNumChildren;
		CMatrix4x4 defaultMatrix; // TODO: Would like aligned matrices - but vector can't do it
		CMatrix4x4 offsetMatrix;
	};
	typedef vector<SXFileFrame> TXFileFrames;


	// A single mesh in an X-File
	struct SXFileMesh
	{
		// Index of frame that holds this mesh
		TUInt32           iParentFrame;

		// Vertex data - lists of vertices, normals, texture coords (UVs) and colours. Vertex list
		// is always present. The normal list may initially be a different length than the vertex
		// list (see below), but the importer will adjust the data duplication to match them. 
		// The vertex colours will be duplicated in a similar manner. The texture coord list must
		// be same length as vertices unless empty
		TXFileVectors     vertices;
		TXFileVectors     normals; 
		TXFileUVs         textureCoords; // Same number of texture cooords as vertices
		TXFileRGBAColours vertexColours;

		// Face data - each face is a triple of vertex indices representing a triangle (X-files may
		// contain larger polygons, these are converted into triangles in this implementation).
		// The face material list identifies the material used for each face, this list must be the
		// same length as the face list, except for a special case of just one entry - meaning that
		// all faces use the same material
		TXFileFaces       faces;
		TXFileInts        faceMaterials;

		// The faces are converted to triangles - but the number of edges on each of the original
		// faces is stored to help work with the normal face list and material list (each of which
		// match the original face list)
		TXFileInts        origFaceEdges;

		// The original normal faces should match faces in terms of numbers of edges (see above).
		// However, their indices may be different, especially if the normal duplication is not
		// the same as that for vertices across faces (e.g. a cube with sharp edges has 8 vertices,
		// but 6 normals - so the face indices would differ). The importer will remove these
		// differences so there is exactly one normal for each vertex making the two face lists
		// become identical
		TXFileFaces       normalFaces;

		// List of materials used in the face data above
		TXFileMaterials   materials;

		// Map from material indexes in the list above to material indexes in the global material
		// list CImportXFile::m_Materials
		TXFileInts        materialMap;

		// Adjacency data - usage unknown - TODO
		TXFileInts        adjacencyIndices;

		// Vertex duplication list - a per-vertex list of integers - each one the index of the 
		// vertex that this one is a duplicate of (i.e. in an identical position). This list must
		// be the same length as the vertex list or empty. Duplication occurs from a need to have
		// different normals, colours or UVs on different faces using the same vertex. The set of
		// unique vertices are identified by having a duplication index referencing themselves.
		// The total unique vertices is also stored here
		TUInt32           iNumUniqueVertices;
		TXFileInts        duplicateIndices;

		// List of bones affecting this mesh, also stored is the maximum bones affecting a single
		// vertex / face in this mesh
		TUInt16           iMaxBonesPerVertex;
		TUInt16           iMaxBonesPerFace;
		TXFileBones       bones;
	};
	typedef vector<SXFileMesh> TXFileMeshes;


	/////////////////////////////////////
	// X-File API support

	// Prepare and return an X-file object
	// Possible return values:
	//		kSuccess:			...
	//		kOutOfSystemMemory:	...
	//		kSystemFailure:		Problem registering X-file templates or other API error
	EImportError PrepareXFileObject
	(
		ID3DXFile** ppXFile
	);

	// Prepare and return an X-file enumerator given a filename and an X-file object
	// Possible return values:
	//		kSuccess:			...
	//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
	//		kSystemFailure:		X-file API error
	EImportError GetXFileEnumerator
	(
		const string&         sFilename,
		ID3DXFile*            pXFile,
		ID3DXFileEnumObject** ppXFileEnumer
	);


	/////////////////////////////////////
	// X-File parsing

	// Create a single root frame and parse the X-File to add all the bottom level frames and
	// meshes. Any frames and meshes found will be children of this root frame, child frames are
	// recursively parsed to create a frame hierarchy
	// Possible return values:
	//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
	EImportError ParseXFile
	(
		ID3DXFileEnumObject* pXFileEnumer
	);

	// Create a new frame and parse the X-File to add all the contained frames and meshes. Any
	// frames and meshes found will become children of this new frame. Child frames are recursively
	// parsed to create a frame hierarchy
	// Possible return values:
	//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
	EImportError ParseXFileFrame
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iParentFrame
	);


	// X-File parsing - collect mesh data
	EImportError ParseXFileMesh
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iCurrFrame
	);


	/////////////////////////////////////
	// X-File template parsing

	// Read vertex and face data from a mesh template
	EImportError ReadMeshData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a normal data mesh template
	EImportError ReadNormalData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a texture coordinate mesh template
	EImportError ReadTextureUVData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a vertex colour mesh template
	EImportError ReadVertexColourData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a vertex colour mesh template
	EImportError ReadMaterialData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a vertex duplication mesh template
	EImportError ReadDuplicationData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a adjacancy data mesh template
	EImportError ReadAdjacencyData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read skinning header mesh template
	EImportError ReadSkinDefnData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh
	);

	// Read a skinning weights mesh template
	EImportError ReadSkinWeightsData
	(
		ID3DXFileData* pXFileData,
		const TUInt32  iMesh,
		const TUInt32  iBone
	);


	/////////////////////////////////////
	// X-File parsing support

	EImportError GetXFileNumChildren
	(
		ID3DXFileEnumObject* pXFileEnumer, 
		TUInt32*             iNumChildren
	);

	EImportError GetXFileNumChildren
	(
		ID3DXFileData* pXFileData,
		TUInt32*       iNumChildren
	);


	EImportError GetXFileChild
	(
		ID3DXFileEnumObject* pXFileEnumer,
		const TUInt32        iChild, 
		ID3DXFileData**      ppChildData, 
		GUID*                pChildGUID
	);

	EImportError GetXFileChild
	(
		ID3DXFileData*  pXFileData,
		const TUInt32   iChild, 
		ID3DXFileData** ppChildData, 
		GUID*           pChildGUID
	);


	EImportError GetXFileDataName
	(
		ID3DXFileData* pXFileData,
		string&        sName
	);


	EImportError LockXFileData
	(
		ID3DXFileData* pXFileData,
		const TUInt8** ppLockData,
		TUInt32*       pSize = 0
	);

	void UnlockXFileData
	(
		ID3DXFileData* pXFileData
	);

	EImportError CopyXFileData
	(
		ID3DXFileData* pXFileData,
		TUInt8*        pDest,
		TUInt32*       pSize
	);

	void ReadXFileLockedData
	(
		const   TUInt8*& pMeshData,
		TUInt8*	         pDest,
		const   TUInt32  iSize
	);

	void ReadXFileLockedUInt
	(
		const TUInt8*& pMeshData,
		TUInt32*       piDest
	);

	void ReadXFileLockedUInt16
	(
		const TUInt8*& pMeshData,
		TUInt16*       piDest
	);


	/////////////////////////////////////
	// Geometry processing

	// Match the face lists of vertices and normals, so there is exactly one normal per vertex
	// See the comment to SXFileMesh::normalFaces above
	void MatchFaceLists
	(
		const TUInt32  iMesh
	);

	// Create a global list of materials used by all the meshes - removing any duplicates. Also 
	// create a list for each mesh mapping local material indices to global ones
	void MakeGlobalMaterialList();


	/////////////////////////////////////
	// Bone support functions

	// Add new bone weight/index to a vertex - maximum of 4, removes least signficant if necessary
	static void AddBoneInfluence( TUInt32 bone, TFloat32 weight,
	                              TFloat32* vertWeights, TUInt8* vertBones );

	// Match the bones in each mesh to their frames
	EImportError ProcessBones();


	/////////////////////////////////////
	// Mesh processing

	// Split each mesh into a set of meshes - each of which contains only a single material
	void SplitMeshes();

	// Create a list of tangent vectors for the given mesh. The tangent vector is the direction of
	// a vertex's texture U axis in model-space. Returns true on success
	bool CalculateTangents
	(
		TUInt32 iMesh,
		TXFileVectors* pTangents
	) const;


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// Has any data been loaded into the lists below
	bool            m_bImported;

	// The list of frames forms a flattened depth-first hierarchy
	TXFileFrames    m_Frames;

	// Each mesh is held by a frame in the hierarchy above
	TXFileMeshes    m_Meshes;

	// Global list of materials used by all the meshes
	TXFileMaterials m_Materials;
};


} // namespace gen

#endif // GEN_C_IMPORT_XFILE_H_INCLUDED
