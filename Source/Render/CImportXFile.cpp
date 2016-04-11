/**************************************************************************************************
	Module:       CImportXFile.cpp
	Author:       Laurent Noel
	Date created: 11/10/05

	Class encapsulating the import of a Microsoft DirectX .X file

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include <algorithm>
#include <numeric>
using namespace std;

#define INITGUID
#include <windows.h>
#include <dxfile.h>
#include <rmxfguid.h>
#include <rmxftmpl.h>

#include "Error.h"
#include "CImportXFile.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	CImportXFile public member functions
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////
// File import

// Tests if supplied filename is a Microsoft X-File
// Possible return values:
//		kSuccess:			...
//		kFileError:			Missing file or not an X-file
bool CImportXFile::IsXFile
(
	const string& sFileName
)
{
	GEN_GUARD;

	if (!sFileName.length())
	{
		return false;
	}

	FILE* pFile = fopen( sFileName.c_str(), "rb" );
	if (!pFile)
	{
		return false;
	}

	bool bXFile = (getc( pFile ) == 'x' && getc( pFile ) == 'o' && getc( pFile ) == 'f' && getc( pFile ) == ' ');
	fclose( pFile );

	return bXFile;

	GEN_ENDGUARD;
}

	
// Import a Microsoft X-File into a list of meshes and a frame hierarchy
// Possible return values:
//		kSuccess:			...
//		kFileError:			Missing file or not an X-file
//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
//		kOutOfSystemMemory:	...
//		kSystemFailure:		X-file API failure
EImportError CImportXFile::ImportFile
(
	const string& sFileName
)
{
	GEN_GUARD;

	// Wipe any existing data
	m_Frames.clear();
	m_Meshes.clear();
	m_bImported = false;

	// Ensure the file is an X-file
	if (!IsXFile( sFileName ))
	{
		return kFileError;
	}

	// Create X-File object
	ID3DXFile* pXFile;
	EImportError eError = PrepareXFileObject( &pXFile );
	if (eError != kSuccess)
	{
		return eError;
	}

	// Get X-File enumerator
	ID3DXFileEnumObject* pXFileEnumer;
	eError = GetXFileEnumerator( sFileName, pXFile, &pXFileEnumer );
	if (eError != kSuccess)
	{
		pXFile->Release();
		return eError;
	}

	// Parse X file to create frame hierachy and meshes
	eError = ParseXFile( pXFileEnumer );

	// Release X-File interfaces
	pXFileEnumer->Release();
	pXFile->Release();

	// Check for errors
	if (eError != kSuccess)
	{
		m_Frames.clear();
		m_Meshes.clear();
		return eError;
	}

	// Split into meshes containing only one material each
	SplitMeshes();

	// Mark file as loaded
	m_bImported = true;

	return kSuccess;

	GEN_ENDGUARD;
}


/////////////////////////////////////
// Data access

// Get a single node from the mesh hierarchy (a frame in an X-File), returned through a pointer
void CImportXFile::GetNode
(
	const TUInt32    iNode,
	SMeshNode* const pOutNode
) const
{
	GEN_GUARD;

	pOutNode->name = m_Frames[iNode].sName;
	pOutNode->depth = m_Frames[iNode].iDepth;
	pOutNode->parent = m_Frames[iNode].iParentIndex;
	pOutNode->numChildren = m_Frames[iNode].iNumChildren;
	pOutNode->positionMatrix = m_Frames[iNode].defaultMatrix;
	pOutNode->invMeshOffset = m_Frames[iNode].offsetMatrix;

	GEN_ENDGUARD;
}


// Get the render method used for the given sub-mesh
ERenderMethod CImportXFile::GetSubMeshRenderMethod( const TUInt32 iSubMesh ) const
{
	const SXFileMaterial& xFileMaterial = m_Materials[m_Meshes[iSubMesh].materialMap.front()];
	return RenderMethodFromMaterial( xFileMaterial.sName, xFileMaterial.sTextureName );
}


// Get the specification and data for given sub-mesh, returned through a pointer. May request
// tangents to be calculated
// Possible return values:
//		kSuccess:			...
//		kOutOfSystemMemory:	...
EImportError CImportXFile::GetSubMesh
(
	const TUInt32 iSubMesh,
	SSubMesh*     pOutSubMesh,
	bool          bTangents /*= false*/
) const
{
	GEN_GUARD;

	// Set sub-mesh owner node
	pOutSubMesh->node = m_Meshes[iSubMesh].iParentFrame;

	// Calculate tangents if required
	TXFileVectors tangents;
	pOutSubMesh->hasTangents = bTangents;
	if (pOutSubMesh->hasTangents)
	{
		CalculateTangents( iSubMesh, &tangents );
	}

	// Find what vertex data there is and calculate total vertex size
	pOutSubMesh->hasSkinningData = (m_Meshes[iSubMesh].bones.size() > 0);
	pOutSubMesh->hasNormals = (m_Meshes[iSubMesh].normals.size() > 0);
	pOutSubMesh->hasTextureCoords = (m_Meshes[iSubMesh].textureCoords.size() > 0);
	pOutSubMesh->hasVertexColours = (m_Meshes[iSubMesh].vertexColours.size() > 0);
	pOutSubMesh->vertexSize = sizeof(CVector3) + 
							  (pOutSubMesh->hasSkinningData ? 4 * sizeof(TFloat32) + sizeof(TUInt32) : 0) +
	                          (pOutSubMesh->hasNormals ? sizeof(CVector3) : 0) +
	                          (pOutSubMesh->hasTangents ? sizeof(CVector3) : 0) +
	                          (pOutSubMesh->hasTextureCoords ? sizeof(SXFileUV) : 0) +
	                          (pOutSubMesh->hasVertexColours ? sizeof(SXFileRGBAColour) : 0);
	                          // Skinning data: assuming 4 float weights / 4 byte indices in TUInt32

	// Set number of vertices and reserve space for vertex data
	pOutSubMesh->numVertices = static_cast<TUInt32>(m_Meshes[iSubMesh].vertices.size());
	pOutSubMesh->vertices = new TUInt8[pOutSubMesh->numVertices * pOutSubMesh->vertexSize];
	if (!pOutSubMesh->vertices)
	{
		return kOutOfSystemMemory;
	}

	// Prefetch relevant vertex list info
	TXFileVectors::const_iterator itVertex = m_Meshes[iSubMesh].vertices.begin();
	TXFileVectors::const_iterator itVertexEnd = m_Meshes[iSubMesh].vertices.end();
	TXFileVectors::const_iterator itNormal = m_Meshes[iSubMesh].normals.begin();
	TXFileVectors::const_iterator itTangent = tangents.begin();
	TXFileUVs::const_iterator itTextureCooord = m_Meshes[iSubMesh].textureCoords.begin();
	TXFileRGBAColours::const_iterator itVertexColour = m_Meshes[iSubMesh].vertexColours.begin();

	// Loop through vertices, add each component present to the raw output stream
	TUInt8* pVertexData = pOutSubMesh->vertices;
	while (itVertex != itVertexEnd)
	{
		*reinterpret_cast<CVector3*>(pVertexData) = *itVertex++;
		pVertexData += sizeof(CVector3);
		if (pOutSubMesh->hasSkinningData)
		{
			// Initialise vertex with no influencing bones
			*reinterpret_cast<TFloat32*>(pVertexData) = 0.0f;
			pVertexData += sizeof(TFloat32);
			*reinterpret_cast<TFloat32*>(pVertexData) = 0.0f;
			pVertexData += sizeof(TFloat32);
			*reinterpret_cast<TFloat32*>(pVertexData) = 0.0f;
			pVertexData += sizeof(TFloat32);
			*reinterpret_cast<TFloat32*>(pVertexData) = 0.0f;
			pVertexData += sizeof(TFloat32);
			*reinterpret_cast<TUInt32*>(pVertexData) = 0;
			pVertexData += sizeof(TUInt32);
		}
		if (pOutSubMesh->hasNormals)
		{
			*reinterpret_cast<CVector3*>(pVertexData) = *itNormal++;
			pVertexData += sizeof(CVector3);
		}
		if (pOutSubMesh->hasTangents)
		{
			*reinterpret_cast<CVector3*>(pVertexData) = *itTangent++;
			pVertexData += sizeof(CVector3);
		}
		if (pOutSubMesh->hasTextureCoords)
		{
			*reinterpret_cast<SXFileUV*>(pVertexData) = *itTextureCooord++;
			pVertexData += sizeof(SXFileUV);
		}
		if (pOutSubMesh->hasVertexColours)
		{
			*reinterpret_cast<SXFileRGBAColour*>(pVertexData) = *itVertexColour++;
			pVertexData += sizeof(SXFileRGBAColour);
		}
	}

	// Calculate bone influences if necessary
	if (pOutSubMesh->hasSkinningData)
	{
		// Offsets to bone data in a vertex (data is immediately after vertex coord)
		TUInt32 boneWeightsOffset = sizeof(CVector3);
		int boneIndicesOffset = boneWeightsOffset + 4 * sizeof(TFloat32);

		// For each bone...
		TXFileBones::const_iterator itBone = m_Meshes[iSubMesh].bones.begin();
		TXFileBones::const_iterator itBoneEnd = m_Meshes[iSubMesh].bones.end();
		while (itBone != itBoneEnd)
		{
			// For each bone weight (influence)...
			TXFileBoneWeights::const_iterator itBoneWeight = itBone->weights.begin();
			TXFileBoneWeights::const_iterator itBoneWeightEnd = itBone->weights.end();
			while (itBoneWeight != itBoneWeightEnd)
			{
				// Find affected vertex data - weights and bone indexes
				TUInt8* pVert = pOutSubMesh->vertices + 
					itBoneWeight->iVertexIndex * pOutSubMesh->vertexSize;
				TFloat32* pVertBoneWeights = reinterpret_cast<TFloat32*>(pVert + boneWeightsOffset);
				TUInt8* pVertBoneIndices = reinterpret_cast<TUInt8*>(pVert + boneIndicesOffset);

				// Add influence of this bone to the vertex data
				AddBoneInfluence( itBone->iFrame, itBoneWeight->fWeight,
				                  pVertBoneWeights, pVertBoneIndices );
				++itBoneWeight;
			}
			++itBone;
		}

		// Normalise vertex bone weights (ensure they add up to 1)
		TUInt8* pVert = pOutSubMesh->vertices;
		for (TUInt32 vert = 0; vert < pOutSubMesh->numVertices; ++vert)
		{
			TFloat32* pVertBoneWeights = reinterpret_cast<TFloat32*>(pVert + boneWeightsOffset);
			TUInt8* pVertBoneIndices = reinterpret_cast<TUInt8*>(pVert + boneIndicesOffset);

			TFloat32 sum = pVertBoneWeights[0] + pVertBoneWeights[1] +
			               pVertBoneWeights[2] + pVertBoneWeights[3];
			if (sum == 0.0f)
			{
				// Vertex with no weights - reference root bone only (model is probably not skinned)
				pVertBoneWeights[0] = 1.0f;
				pVertBoneIndices[0] = pOutSubMesh->node;
			}
			else
			{
				pVertBoneWeights[0] /= sum;
				pVertBoneWeights[1] /= sum;
				pVertBoneWeights[2] /= sum;
				pVertBoneWeights[3] /= sum;
			}
			pVert += pOutSubMesh->vertexSize;
		}
	}

	// Pre-size face array
	pOutSubMesh->numFaces = static_cast<TUInt32>(m_Meshes[iSubMesh].faces.size());
	pOutSubMesh->faces = new SMeshFace[pOutSubMesh->numFaces];

	// Get material from material map (all faces in sub-mesh have the same material at this point)
	pOutSubMesh->material = m_Meshes[iSubMesh].materialMap.front();

	// Loop through faces outputing to given sub-mesh
	TXFileFaces::const_iterator itFace = m_Meshes[iSubMesh].faces.begin();
	TXFileFaces::const_iterator itFaceEnd = m_Meshes[iSubMesh].faces.end();
	for (TUInt32 iFace = 0; iFace < pOutSubMesh->numFaces; ++iFace)
	{
		pOutSubMesh->faces[iFace].aiVertex[0] = itFace->aiVertex[0];
		pOutSubMesh->faces[iFace].aiVertex[1] = itFace->aiVertex[1];
		pOutSubMesh->faces[iFace].aiVertex[2] = itFace->aiVertex[2];
		++itFace;
	}

	return kSuccess;

	GEN_ENDGUARD;
}


// Get specification of a given material, returned through a pointer
// The render method of a material specifies how to draw geometry with this material
void CImportXFile::GetMaterial
(
	const TUInt32        iMaterial,
	SMeshMaterial* const pOutMaterial
) const
{
	GEN_GUARD;
	
	// Clear the output material contents to set default values
	memset( static_cast<void*>(pOutMaterial), 0, sizeof(SMeshMaterial) );

	// Unclutter code with a reference to the material 
	const SXFileMaterial& xFileMaterial = m_Materials[iMaterial];

	// Set constant colours
	pOutMaterial->diffuseColour.r = xFileMaterial.faceColour.fRed;
	pOutMaterial->diffuseColour.g = xFileMaterial.faceColour.fGreen; 
	pOutMaterial->diffuseColour.b = xFileMaterial.faceColour.fBlue;
	pOutMaterial->diffuseColour.a = xFileMaterial.faceColour.fAlpha;
	pOutMaterial->specularColour.r = xFileMaterial.specularColour.fRed;
	pOutMaterial->specularColour.g = xFileMaterial.specularColour.fGreen; 
	pOutMaterial->specularColour.b = xFileMaterial.specularColour.fBlue;
	pOutMaterial->specularColour.a = 1.0f;
	pOutMaterial->specularPower = xFileMaterial.fSpecularPower;

	// Select render method and number of textures from material and texture name
	pOutMaterial->renderMethod = RenderMethodFromMaterial( xFileMaterial.sName, xFileMaterial.sTextureName ); 
	pOutMaterial->numTextures = NumTexturesUsedByRenderMethod( pOutMaterial->renderMethod );

	// Set texture names. If a render method uses multiple textures, secondary texture names will be based on 
	// the main texture name. E.g. if a normal mapping method uses 3 textures and the main texture is "wall.jpg"
	// then the other textures must be named "wall1.jpg" and "wall2.jpg"
	if (pOutMaterial->numTextures > 0)
	{
		pOutMaterial->textureFileNames[0] = xFileMaterial.sTextureName;
		for (TUInt32 iExtraTex = 1; iExtraTex < pOutMaterial->numTextures; ++iExtraTex)
		{
			string::size_type lastDot = xFileMaterial.sTextureName.find_last_of( "." );
			if (lastDot == string::npos)
			{
				pOutMaterial->textureFileNames[iExtraTex] = xFileMaterial.sTextureName + char('0' + iExtraTex);
			}
			else
			{
				string filename = xFileMaterial.sTextureName.substr( 0, lastDot );
				string extension = xFileMaterial.sTextureName.substr( lastDot );
				pOutMaterial->textureFileNames[iExtraTex] = filename + char('0' + iExtraTex) + extension;
			}
		}
	}

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	X-File API support
-----------------------------------------------------------------------------------------*/

// Prepare and return an X-file object
// Possible return values:
//		kSuccess:			...
//		kOutOfSystemMemory:	...
//		kSystemFailure:		Problem registering X-file templates or other API error
EImportError CImportXFile::PrepareXFileObject
(
	ID3DXFile** ppXFile
)
{
	GEN_GUARD;

	HRESULT xFileError = D3DXFileCreate( ppXFile );
	if (xFileError != S_OK)
	{
		*ppXFile = 0;
		switch (xFileError)
		{
		case E_OUTOFMEMORY:
			return kOutOfSystemMemory;

		case E_POINTER:
			GEN_ERROR( "Invalid parameter to D3DXFileCreate" );

		default:
			return kSystemFailure;
		}
	}

    // Register templates for d3drm, skinning and patch extensions.
    xFileError = (*ppXFile)->RegisterTemplates( (void*)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES );
	if (xFileError != S_OK)
	{
        (*ppXFile)->Release();
		*ppXFile = 0;
		switch (xFileError)
		{
		case D3DXFERR_BADVALUE: 
			GEN_ERROR( "Invalid parameter to RegisterTemplates" );

		default:
			return kSystemFailure;
		}
    }

    xFileError = (*ppXFile)->RegisterTemplates( (void*)XSKINEXP_TEMPLATES, strlen(XSKINEXP_TEMPLATES) );
	if (xFileError != S_OK)
	{
        (*ppXFile)->Release();
		switch (xFileError)
		{
		case D3DXFERR_BADVALUE:
			GEN_ERROR( "Invalid parameter to RegisterTemplates" );

		default:
			return kSystemFailure;
		}
    }

    xFileError = (*ppXFile)->RegisterTemplates( (void*)XEXTENSIONS_TEMPLATES, strlen(XEXTENSIONS_TEMPLATES) );
	if (xFileError != S_OK)
	{
        (*ppXFile)->Release();
		*ppXFile = 0;
		switch (xFileError)
		{
		case D3DXFERR_BADVALUE:
			GEN_ERROR( "Invalid parameter to RegisterTemplates" );

		default:
			return kInvalidData;
		}
    }

	return kSuccess;

	GEN_ENDGUARD;
}


// Prepare and return an X-file enumerator given a filename and an X-file object
// Possible return values:
//		kSuccess:			...
//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
//		kSystemFailure:		X-file API error
EImportError CImportXFile::GetXFileEnumerator
(
	const string&         sFilename,
	ID3DXFile*            pXFile,
	ID3DXFileEnumObject** ppXFileEnumer
)
{
	GEN_GUARD;

	HRESULT xFileError =
		pXFile->CreateEnumObject( (LPVOID)sFilename.c_str(), DXFILELOAD_FROMFILE, ppXFileEnumer );
	if (xFileError != S_OK)
	{
		switch (xFileError)
		{
		case D3DXFERR_PARSEERROR:
			return kInvalidData;

		case D3DXFERR_BADVALUE:
			GEN_ERROR( "Invalid parameter to CreateEnumObject" );

		default:
			return kSystemFailure;
		}
	}

	return kSuccess;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	X-File parsing
-----------------------------------------------------------------------------------------*/

// Create a single root frame and parse the X-File to add all the bottom level frames and
// meshes. Any frames and meshes found will be children of this root frame, child frames are
// recursively parsed to create a frame hierarchy
// Possible return values:
//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
EImportError CImportXFile::ParseXFile
(
	ID3DXFileEnumObject* pXFileEnumer
)
{
	GEN_GUARD;

	// Create new root frame
	m_Frames.push_back( SXFileFrame() );

	// Set root frame values
	m_Frames[0].sName = "Root";
	m_Frames[0].iDepth = 0;
	m_Frames[0].iParentIndex = 0;
	m_Frames[0].iNumChildren = 0;
	m_Frames[0].defaultMatrix = CMatrix4x4::kIdentity;
	m_Frames[0].offsetMatrix = CMatrix4x4::kIdentity;

	// Get number of child objects for the current object
	TUInt32 iNumChildren;
	EImportError eError = GetXFileNumChildren( pXFileEnumer, &iNumChildren );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// For each child object
	for (TUInt32 iChild = 0; iChild < iNumChildren; ++iChild)
	{
		// Get child data and ID
		ID3DXFileData* pChildData;
		GUID childGUID;
		eError = GetXFileChild( pXFileEnumer, iChild, &pChildData, &childGUID );
		if (eError != kSuccess)
		{
			return kInvalidData;
		}
		
		// Found child frame
		if (childGUID == TID_D3DRMFrame)
		{
			++m_Frames[0].iNumChildren;
			eError = ParseXFileFrame( pChildData, 0 );
		}

		// Found child frame transformation matrix
		else if (childGUID == TID_D3DRMFrameTransformMatrix)
		{
			CMatrix4x4 transMat;
			TUInt32 iSize = 16 * sizeof(TFloat32);
			TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Frames[0].defaultMatrix.e00);
			eError = CopyXFileData( pChildData, pDest, &iSize );
		}

		// Found child mesh
		else if (childGUID == TID_D3DRMMesh)
		{
			eError = ParseXFileMesh( pChildData, 0 );
		}

		// Release current child data before moving to the next or quiting on error
		pChildData->Release();

		// Return any errors found
		if (eError != kSuccess)
		{
			return eError;
		}
	}		

	// Make a single global material list for all meshes
	MakeGlobalMaterialList();
	
	// Validate bones and match them to their frames
	eError = ProcessBones();
	if (eError != kSuccess)
	{
		return eError;
	}

	return kSuccess;

	GEN_ENDGUARD;
}


// Create a new frame and parse the X-File to add all the contained frames and meshes. Any
// frames and meshes found will become children of this new frame. Child frames are recursively
// parsed to create a frame hierarchy
// Possible return values:
//		kInvalidData:		The file could not be parsed correctly, or contains invalid data
EImportError CImportXFile::ParseXFileFrame
(
	ID3DXFileData* pXFileData,
	const TUInt32  iParentFrame
)
{
	GEN_GUARD;

	// Create new frame
	TUInt32 iCurrFrame = static_cast<TUInt32>(m_Frames.size());
	m_Frames.push_back( SXFileFrame() );

	// Get name for frame
	EImportError eError = GetXFileDataName( pXFileData, m_Frames[iCurrFrame].sName );
	if (eError != kSuccess)
	{
		return eError;
	}

	// Initialise other frame values
	m_Frames[iCurrFrame].iDepth = m_Frames[iParentFrame].iDepth + 1;
	m_Frames[iCurrFrame].iParentIndex = iParentFrame;
	m_Frames[iCurrFrame].iNumChildren = 0;
	m_Frames[iCurrFrame].defaultMatrix = CMatrix4x4::kIdentity;
	m_Frames[iCurrFrame].offsetMatrix = CMatrix4x4::kIdentity;

	// Get number of child objects for the current object
	TUInt32 iNumChildren;
	eError = GetXFileNumChildren( pXFileData, &iNumChildren );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// For each child object
	for (TUInt32 iChild = 0; iChild < iNumChildren; ++iChild)
	{
		// Get child data and ID
		ID3DXFileData* pChildData;
		GUID childGUID;
		eError = GetXFileChild( pXFileData, iChild, &pChildData, &childGUID );
		if (eError != kSuccess)
		{
			return kInvalidData;
		}
		
		// Found child frame
		if (childGUID == TID_D3DRMFrame)
		{
			++m_Frames[iCurrFrame].iNumChildren;
			eError = ParseXFileFrame( pChildData, iCurrFrame );
		}

		// Found child frame transformation matrix
		else if (childGUID == TID_D3DRMFrameTransformMatrix)
		{
			CMatrix4x4 transMat;
			TUInt32 iSize = 16 * sizeof(TFloat32);
			TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Frames[iCurrFrame].defaultMatrix.e00);
			eError = CopyXFileData( pChildData, pDest, &iSize );
		}

		// Found child mesh
		else if (childGUID == TID_D3DRMMesh)
		{
			eError = ParseXFileMesh( pChildData, iCurrFrame );
		}

		// Release current child data before moving to the next or quiting on error
		pChildData->Release();

		// Return any errors found
		if (eError != kSuccess)
		{
			return eError;
		}
	}		

	return kSuccess;

	GEN_ENDGUARD;
}


// Create a new mesh in the given frame and parse its data from the X-File
EImportError CImportXFile::ParseXFileMesh
(
	ID3DXFileData* pXFileData,
	const TUInt32  iCurrFrame
)
{
	GEN_GUARD;

	// Create new mesh
	TUInt32 iCurrMesh = static_cast<TUInt32>(m_Meshes.size());
	m_Meshes.push_back( SXFileMesh() );

	// Set owner frame
	m_Meshes[iCurrMesh].iParentFrame = iCurrFrame;
	m_Meshes[iCurrMesh].iNumUniqueVertices = 0;
	m_Meshes[iCurrMesh].iMaxBonesPerVertex = 0;
	m_Meshes[iCurrMesh].iMaxBonesPerFace = 0;

	// Read vertices and faces for the mesh
	EImportError eError = ReadMeshData( pXFileData, iCurrMesh );
	if (eError != kSuccess)
	{
		return eError;
	}

	// Counter for bones read from child data objects
	TUInt32 iCurrBone = 0; 

	// Get number of child objects for the current object
	TUInt32 iNumChildren;
	eError = GetXFileNumChildren( pXFileData, &iNumChildren );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// For each child object
	for (TUInt32 iChild = 0; iChild < iNumChildren; ++iChild)
	{
		// Get child data and ID
		ID3DXFileData* pChildData;
		GUID childGUID;
		eError = GetXFileChild( pXFileData, iChild, &pChildData, &childGUID );
		if (eError != kSuccess)
		{
			return kInvalidData;
		}

		// Found normal data
		if (childGUID == TID_D3DRMMeshNormals)
		{
			eError = ReadNormalData( pChildData, iCurrMesh );
		}

		// Found texture coordinate data
		else if (childGUID == TID_D3DRMMeshTextureCoords)
		{
			eError = ReadTextureUVData( pChildData, iCurrMesh );
		}

		// Found vertex colour data
		else if (childGUID == TID_D3DRMMeshVertexColors)
		{
			eError = ReadVertexColourData( pChildData, iCurrMesh );
		}

		// Found material list
		else if (childGUID == TID_D3DRMMeshMaterialList)
		{
			eError = ReadMaterialData( pChildData, iCurrMesh );
		}

		// Found vertex duplication list
		else if (childGUID == DXFILEOBJ_VertexDuplicationIndices)
		{
			eError = ReadDuplicationData( pChildData, iCurrMesh );
		}

		// Found face adjacency data
		else if (childGUID == DXFILEOBJ_FaceAdjacency)
		{
			eError = ReadAdjacencyData( pChildData, iCurrMesh );
		}

		// Found skinning definition
		else if (childGUID == DXFILEOBJ_XSkinMeshHeader)
		{
			eError = ReadSkinDefnData( pChildData, iCurrMesh );
		}

		// Found skin weights
		else if (childGUID == DXFILEOBJ_SkinWeights)
		{
			eError = ReadSkinWeightsData( pChildData, iCurrMesh, iCurrBone );
			++iCurrBone;
		}

		// Found unknown mesh data
		else
		{
			eError = kSuccess; // Won't flag this as failure though
		}

		if (eError != kSuccess)
		{
			return eError;
		}

		// Release current child data before moving to the next
		pChildData->Release();
	}

	// Check if not enough bones
	if (iCurrBone != m_Meshes[iCurrMesh].bones.size())
	{
		return kInvalidData;
	}

	// Match the face lists of vertices and normals, so there is exactly one normal per vertex
	MatchFaceLists( iCurrMesh );

	return kSuccess;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	X-File template parsing
-----------------------------------------------------------------------------------------*/

// Read vertex and face data from a mesh template
EImportError CImportXFile::ReadMeshData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Lock mesh data pointer
	TUInt32 iMeshDataSize = 0;
	const TUInt8* pMeshDataStart;
	EImportError eError = LockXFileData( pXFileData, &pMeshDataStart, &iMeshDataSize );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}
	const TUInt8* pMeshData = pMeshDataStart;

	// Get vertices
	TUInt32 iNumVertices;
	ReadXFileLockedUInt( pMeshData, &iNumVertices );
	m_Meshes[iMesh].vertices.resize( iNumVertices );
	for (TUInt32 iVertex = 0; iVertex < iNumVertices; ++iVertex)
	{
		TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].vertices[iVertex]);
		ReadXFileLockedData( pMeshData, pDest, sizeof(CVector3) );
	}

	// Read faces - they can be general polygons - convert them all to triangles
	TUInt32 iNumFaces;
	ReadXFileLockedUInt( pMeshData, &iNumFaces );
	m_Meshes[iMesh].origFaceEdges.resize( iNumFaces ); // See below
	for (TUInt32 iFace = 0; iFace < iNumFaces; ++iFace)
	{
		TUInt32 iNumEdges;
		ReadXFileLockedUInt( pMeshData, &iNumEdges );

		// Store original number of edges for normal face validation below
		m_Meshes[iMesh].origFaceEdges[iFace] = iNumEdges;

		// Read first index of polygon, then use successive pairs of indices to form triangles
		// with this first one
		TUInt32 iFirstIndex, iIndexA, iIndexB;
		ReadXFileLockedUInt( pMeshData, &iFirstIndex );
		ReadXFileLockedUInt( pMeshData, &iIndexA );
		for (TUInt32 iEdge = 2; iEdge < iNumEdges; ++iEdge)
		{
			ReadXFileLockedUInt( pMeshData, &iIndexB );
			SXFileFace face = { iFirstIndex, iIndexA, iIndexB };
			m_Meshes[iMesh].faces.push_back( face );
			iIndexA = iIndexB;
		}
	}

	// Unlock mesh data
	pXFileData->Unlock();

	// Validate amount of data read
	if (static_cast<TUInt32>(pMeshData - pMeshDataStart) != iMeshDataSize)
	{
		return kInvalidData;
	}

	return kSuccess;
	GEN_ENDGUARD;
}


// Read a normal data mesh template
EImportError CImportXFile::ReadNormalData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one vertex normal list in a mesh
	if (m_Meshes[iMesh].normals.size() > 0)
	{
		return kInvalidData;
	}

	// Get normal data
	const TUInt8* pNormalData;
	EImportError eError = LockXFileData( pXFileData, &pNormalData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read normals
	TUInt32 iNumNormals;
	ReadXFileLockedUInt( pNormalData, &iNumNormals );
	m_Meshes[iMesh].normals.resize( iNumNormals );
	for (TUInt32 iNormal = 0; iNormal < iNumNormals; ++iNormal)
	{
		TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].normals[iNormal]);
		ReadXFileLockedData( pNormalData, pDest, sizeof(CVector3) );
	}

	// Verify that normal face list matches face list
	TUInt32 iNumNormalFaces;
	ReadXFileLockedUInt( pNormalData, &iNumNormalFaces );
	if (iNumNormalFaces != m_Meshes[iMesh].origFaceEdges.size())
	{
		pXFileData->Unlock();
		return kInvalidData;
	}

	// Read normal faces - they can be general polygons - convert them all to triangles
	for (TUInt32 iFace = 0; iFace < iNumNormalFaces; ++iFace)
	{
		TUInt32 iNumEdges;
		ReadXFileLockedUInt( pNormalData, &iNumEdges );

		// Check number of edges against original face data
		if (iNumEdges != m_Meshes[iMesh].origFaceEdges[iFace])
		{
			pXFileData->Unlock();
			return kInvalidData;
		}

		// Read first index of polygon, then use successive pairs of indices to form triangles
		// with this first one
		TUInt32 iFirstIndex, iIndexA, iIndexB;
		ReadXFileLockedUInt( pNormalData, &iFirstIndex );
		ReadXFileLockedUInt( pNormalData, &iIndexA );
		for (TUInt32 iEdge = 2; iEdge < iNumEdges; ++iEdge)
		{
			ReadXFileLockedUInt( pNormalData, &iIndexB );
			SXFileFace face = { iFirstIndex, iIndexA, iIndexB };
			m_Meshes[iMesh].normalFaces.push_back( face );
			iIndexA = iIndexB;
		}
	}

	// Finished with normal data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}

// Read a texture coordinate mesh template
EImportError CImportXFile::ReadTextureUVData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one texture coordinate list in a mesh
	if (m_Meshes[iMesh].textureCoords.size() > 0)
	{
		return kInvalidData;
	}

	// Get texture coordinate data
	const TUInt8* pTexCoordData;
	EImportError eError = LockXFileData( pXFileData, &pTexCoordData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read texture coordinates
	TUInt32 iNumTextureCoords;
	ReadXFileLockedUInt( pTexCoordData, &iNumTextureCoords );
	if (iNumTextureCoords != m_Meshes[iMesh].vertices.size())
	{
		pXFileData->Unlock();
		return kInvalidData;
	}
	m_Meshes[iMesh].textureCoords.resize( iNumTextureCoords );
	for (TUInt32 iUV = 0; iUV < iNumTextureCoords; ++iUV)
	{
		TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].textureCoords[iUV]);
		ReadXFileLockedData( pTexCoordData, pDest, sizeof(SXFileUV) );
	}

	// Finished with texture coordinate data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}

// Read a vertex colour mesh template, any vertices not assigned a colour will get white
EImportError CImportXFile::ReadVertexColourData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one vertex colour list in a mesh
	if (m_Meshes[iMesh].vertexColours.size() > 0)
	{
		return kInvalidData;
	}

	// Get vertex colour data
	const TUInt8* pVertexColourData;
	EImportError eError = LockXFileData( pXFileData, &pVertexColourData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read vertex colours
	TUInt32 iNumVertexColours;
	ReadXFileLockedUInt( pVertexColourData, &iNumVertexColours );

	// All colours default to white if not assigned
	// TODO: Could split mesh into sections with and without vertex colours - not worth it?
	SXFileRGBAColour defaultColour = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_Meshes[iMesh].vertexColours.resize( iNumVertexColours, defaultColour );
	for (TUInt32 iColour = 0; iColour < iNumVertexColours; ++iColour)
	{
		TUInt32 iVertexIndex;
		ReadXFileLockedUInt( pVertexColourData, &iVertexIndex );
		TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].vertexColours[iVertexIndex]);
		ReadXFileLockedData( pVertexColourData, pDest, sizeof(SXFileRGBAColour) );
	}

	// Finished with vertex colour data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}

// Read a vertex colour mesh template
EImportError CImportXFile::ReadMaterialData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one material list in a mesh
	if (m_Meshes[iMesh].materials.size() > 0)
	{
		return kInvalidData;
	}

	// Get material data
	const TUInt8* pMaterialData;
	EImportError eError = LockXFileData( pXFileData, &pMaterialData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read number of materials and initialise material list
	TUInt32 iNumMaterials;
	ReadXFileLockedUInt( pMaterialData, &iNumMaterials );
	for (TUInt32 iMaterial = 0; iMaterial < iNumMaterials; ++iMaterial)
	{
		SXFileMaterial material = 
		{
			"",
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			20.0f, { 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f },
			""
		};
		m_Meshes[iMesh].materials.push_back( material );
	}

	// Read face materials - matching the original face list before it was split into triangles.
	// Will convert to match the new (triangle-only) face list
	TUInt32 iNumFaceMaterials;
	ReadXFileLockedUInt( pMaterialData, &iNumFaceMaterials );

	// Handle undocumented case with only one face material - all faces use same material
	if (iNumFaceMaterials == 1 && m_Meshes[iMesh].origFaceEdges.size() != 1)
	{
		// Read the single face material
		TUInt32 iFaceMaterial;
		ReadXFileLockedUInt( pMaterialData, &iFaceMaterial );

		// Create a full face material list from this value
		m_Meshes[iMesh].faceMaterials.resize( m_Meshes[iMesh].faces.size(), iFaceMaterial );
	}
	else // Read standard face materials - one material reference for each face
	{
		if (iNumFaceMaterials != m_Meshes[iMesh].origFaceEdges.size())
		{
			pXFileData->Unlock();
			return kInvalidData;
		}
		m_Meshes[iMesh].faceMaterials.resize( m_Meshes[iMesh].faces.size() );
		TUInt32 iFace = 0;
		for (TUInt32 iOrigFace = 0; iOrigFace < iNumFaceMaterials; ++iOrigFace)
		{
			TUInt32 iMaterial;
			ReadXFileLockedUInt( pMaterialData, &iMaterial );
			m_Meshes[iMesh].faceMaterials[iFace] = iMaterial;
			++iFace;
			for (TUInt32 iEdge = 3; iEdge < m_Meshes[iMesh].origFaceEdges[iOrigFace]; ++iEdge)
			{
				m_Meshes[iMesh].faceMaterials[iFace] = iMaterial;
				++iFace;
			}
		}
	}

	// Finished with material data
	pXFileData->Unlock();


	// Counter for materials read from optional data objects
	TUInt32 iMaterialsRead = 0;

	// Get number of child objects for the current object
	TUInt32 iNumMatListChildren;
	eError = GetXFileNumChildren( pXFileData, &iNumMatListChildren );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// For each child object
	for (TUInt32 iMatListChild = 0; iMatListChild < iNumMatListChildren; ++iMatListChild)
	{
		// Get child data and ID
		ID3DXFileData* pMatListChildData;
		GUID matListChildGUID;
		eError = GetXFileChild( pXFileData, iMatListChild, &pMatListChildData, &matListChildGUID );
		if (eError != kSuccess)
		{
			return kInvalidData;
		}

		// Found material in material list
		if (matListChildGUID == TID_D3DRMMaterial)
		{
			// Check if too many materials
			if (iMaterialsRead >= m_Meshes[iMesh].materials.size())
			{
				pMatListChildData->Release();
				return kInvalidData;
			}

			// Read material name
			EImportError eError =
				GetXFileDataName( pMatListChildData, m_Meshes[iMesh].materials[iMaterialsRead].sName );
			if (eError != kSuccess)
			{
				pMatListChildData->Release();
				return kInvalidData;
			}

			// Get material data (11 floats in material template up to optional data)
			// Fills in all of SXFileMaterial between name & texture name TODO a bit dodgy
			TUInt32 iSize = 11 * sizeof(TFloat32);
			TUInt8* pDest = 
				reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].materials[iMaterialsRead].faceColour);
			CopyXFileData( pMatListChildData, pDest, &iSize );


			// Get number of child objects for the current object
			TUInt32 iNumMatChildren;
			eError = GetXFileNumChildren( pMatListChildData, &iNumMatChildren );
			if (eError != kSuccess)
			{
				pMatListChildData->Release();
				return kInvalidData;
			}

			// For each child object
			for (TUInt32 iMatChild = 0; iMatChild < iNumMatChildren; ++iMatChild)
			{
				// Get child data and ID
				ID3DXFileData* pMatChildData;
				GUID matChildGUID;
				eError = GetXFileChild( pMatListChildData, iMatChild,
				                        &pMatChildData, &matChildGUID );
				if (eError != kSuccess)
				{
					pMatListChildData->Release();
					return kInvalidData;
				}

				// Found texture filename in material
				if (matChildGUID == TID_D3DRMTextureFilename)
				{
					const TUInt8* pFileNameData;
					EImportError eError = LockXFileData( pMatChildData, &pFileNameData );
					if (eError != kSuccess)
					{
						pMatChildData->Release();
						pMatListChildData->Release();
						return kInvalidData;
					}
					m_Meshes[iMesh].materials[iMaterialsRead].sTextureName = 
						reinterpret_cast<const char*>(pFileNameData);
					pMatChildData->Unlock();
				}

				// Found unknown material data
				else
				{
					// Ignore
				}

				// Release material child data before moving to the next
				pMatChildData->Release();
			}

			// Increase nubmer of materials that have been found and read
			++iMaterialsRead;
		}

		// Found unknown material list data
		else
		{
			// Ignore
		}

		// Release material list child data before moving to the next
		pMatListChildData->Release();
	}

	// Check if not enough materials
	if (iMaterialsRead != m_Meshes[iMesh].materials.size())
	{
		return kInvalidData;
	}

	return kSuccess;

	GEN_ENDGUARD;
}

// Read a vertex duplication mesh template
EImportError CImportXFile::ReadDuplicationData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one vertex duplication list in a mesh
	if (m_Meshes[iMesh].duplicateIndices.size() > 0)
	{
		return kInvalidData;
	}

	// Get vertex duplication data
	const TUInt8* pDuplicationData;
	EImportError eError = LockXFileData( pXFileData, &pDuplicationData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read duplicaton indices, also fetch number of unique vertices
	TUInt32 iNumDuplicationIndices;
	ReadXFileLockedUInt( pDuplicationData, &iNumDuplicationIndices );
	if (iNumDuplicationIndices != m_Meshes[iMesh].vertices.size())
	{
		pXFileData->Unlock();
		return kInvalidData;
	}
	ReadXFileLockedUInt( pDuplicationData, &m_Meshes[iMesh].iNumUniqueVertices );
	m_Meshes[iMesh].duplicateIndices.resize( iNumDuplicationIndices );
	for (TUInt32 iIndex = 0; iIndex < iNumDuplicationIndices; ++iIndex)
	{
		ReadXFileLockedUInt( pDuplicationData, &m_Meshes[iMesh].duplicateIndices[iIndex] );
	}

	// Finished with vertex duplication data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}

// Read a adjacancy data mesh template
// TODO: Unknown usage
EImportError CImportXFile::ReadAdjacencyData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one face adjacency list in a mesh
	if (m_Meshes[iMesh].adjacencyIndices.size() > 0)
	{
		return kInvalidData;
	}

	// Get face adjacency data
	const TUInt8* pAdjacencyData;
	EImportError eError = LockXFileData( pXFileData, &pAdjacencyData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read face adjacency list
	TUInt32 iNumAdjacencyIndices;
	ReadXFileLockedUInt( pAdjacencyData, &iNumAdjacencyIndices );
	m_Meshes[iMesh].adjacencyIndices.resize( iNumAdjacencyIndices );
	for (TUInt32 iIndex = 0; iIndex < iNumAdjacencyIndices; ++iIndex)
	{
		ReadXFileLockedUInt( pAdjacencyData, &m_Meshes[iMesh].adjacencyIndices[iIndex] );
	}

	// Finished with face adjacency data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}

// Read skinning header mesh template
EImportError CImportXFile::ReadSkinDefnData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Only allow one skining definition in a mesh
	if (m_Meshes[iMesh].bones.size() > 0)
	{
		return kInvalidData;
	}

	// Get skinning definition data
	const TUInt8* pSkinDefnData;
	EImportError eError = LockXFileData( pXFileData, &pSkinDefnData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read maximum weights info
	ReadXFileLockedUInt16( pSkinDefnData, &m_Meshes[iMesh].iMaxBonesPerVertex );
	ReadXFileLockedUInt16( pSkinDefnData, &m_Meshes[iMesh].iMaxBonesPerFace );

	// Get number of bones used and initialise bone structures
	TUInt16 iNumBones;
	ReadXFileLockedUInt16( pSkinDefnData, &iNumBones );
	for (TUInt32 iBone = 0; iBone < iNumBones; ++iBone)
	{
		SXFileBone bone;
		bone.iFrame = 0;
		bone.offsetMatrix = CMatrix4x4::kIdentity;
		m_Meshes[iMesh].bones.push_back( bone );
	}

	// Finished with skinning definition data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}

// Read a skinning weights mesh template
EImportError CImportXFile::ReadSkinWeightsData
(
	ID3DXFileData* pXFileData,
	const TUInt32  iMesh,
	const TUInt32  iBone
)
{
	GEN_GUARD;

	// Check if no skinning definition or too many bones
	if (m_Meshes[iMesh].bones.size() == 0 || iBone >= m_Meshes[iMesh].bones.size())
	{
		return kInvalidData;
	}

	// Get skin weights data
	const TUInt8* pSkinWeightData;
	EImportError eError = LockXFileData( pXFileData, &pSkinWeightData );
	if (eError != kSuccess)
	{
		return kInvalidData;
	}

	// Read name of bone
	m_Meshes[iMesh].bones[iBone].sFrameName = reinterpret_cast<const char*>(pSkinWeightData);
	pSkinWeightData += m_Meshes[iMesh].bones[iBone].sFrameName.length() + 1;

	// Read number of weights
	TUInt32 iNumWeights;
	ReadXFileLockedUInt( pSkinWeightData, &iNumWeights );
	m_Meshes[iMesh].bones[iBone].weights.resize( iNumWeights );

	// Read skinning indices, weights and offset matrix
	for (TUInt32 iIndex = 0; iIndex < iNumWeights; ++iIndex)
	{
		ReadXFileLockedUInt( pSkinWeightData,
		                     &m_Meshes[iMesh].bones[iBone].weights[iIndex].iVertexIndex );
	}

	for (TUInt32 iWeight = 0; iWeight < iNumWeights; ++iWeight)
	{
		TUInt8* pDest = 
			reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].bones[iBone].weights[iWeight].fWeight);
		ReadXFileLockedData( pSkinWeightData, pDest, sizeof(TFloat32) );
	}

	TUInt8* pDest = reinterpret_cast<TUInt8*>(&m_Meshes[iMesh].bones[iBone].offsetMatrix);
	ReadXFileLockedData( pSkinWeightData, pDest, 16 * sizeof(TFloat32) );

	// Finished with skin weight data
	pXFileData->Unlock();

	return kSuccess;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	X-File parsing
-----------------------------------------------------------------------------------------*/

EImportError CImportXFile::GetXFileNumChildren
(
	ID3DXFileEnumObject* pXFileEnumer, 
	TUInt32*             iNumChildren
)
{
	GEN_GUARD;

	HRESULT xFileError = pXFileEnumer->GetChildren( reinterpret_cast<DWORD*>(iNumChildren) );
	if (xFileError != S_OK)
	{
		return kInvalidData;
	}

	return kSuccess;

	GEN_ENDGUARD;
}

EImportError CImportXFile::GetXFileNumChildren
(
	ID3DXFileData* pXFileData,
	TUInt32*       iNumChildren
)
{
	GEN_GUARD;

	HRESULT xFileError = pXFileData->GetChildren( reinterpret_cast<DWORD*>(iNumChildren) );
	if (xFileError != S_OK)
	{
		return kInvalidData;
	}

	return kSuccess;

	GEN_ENDGUARD;
}


EImportError CImportXFile::GetXFileChild
(
	ID3DXFileEnumObject* pXFileEnumer,
	const TUInt32        iChild, 
	ID3DXFileData**      ppChildData, 
	GUID*                pChildGUID
)
{
	GEN_GUARD;

	HRESULT xFileError = pXFileEnumer->GetChild( iChild, ppChildData );
	if (xFileError != S_OK)
	{
		GEN_ASSERT( xFileError != D3DXFERR_NOMOREOBJECTS, "Invalid child ID" );
		return kInvalidData;
	}

	xFileError = (*ppChildData)->GetType( pChildGUID );
	GEN_ASSERT( xFileError == S_OK, "Failure getting X-File GUID" );

	return kSuccess;

	GEN_ENDGUARD;
}

EImportError CImportXFile::GetXFileChild
(
	ID3DXFileData*  pXFileData,
	const TUInt32   iChild, 
	ID3DXFileData** ppChildData, 
	GUID*           pChildGUID
)
{
	GEN_GUARD;

	HRESULT xFileError = pXFileData->GetChild( iChild, ppChildData );
	if (xFileError != S_OK)
	{
		GEN_ASSERT( xFileError != D3DXFERR_NOMOREOBJECTS, "Invalid child ID" );
		return kInvalidData;
	}

	xFileError = (*ppChildData)->GetType( pChildGUID );
	GEN_ASSERT( xFileError == S_OK, "Failure getting X-File GUID" );

	return kSuccess;

	GEN_ENDGUARD;
}


EImportError CImportXFile::GetXFileDataName
(
	ID3DXFileData* pXFileData,
	string&        sName
)
{
	GEN_GUARD;

	DWORD iDataSize;
	HRESULT xFileError = pXFileData->GetName( NULL, &iDataSize );
	if (!iDataSize)
	{
		sName = "";
	}
	else
	{
		char* szName = new char[iDataSize];
		if (!szName)
		{
			return kOutOfSystemMemory;
		}
		xFileError = pXFileData->GetName( szName, &iDataSize );
		GEN_ASSERT( xFileError == S_OK, "Failure getting X-File name" );
		sName = szName;
		delete[] szName;
	}

	return kSuccess;

	GEN_ENDGUARD;
}


EImportError CImportXFile::LockXFileData
(
	ID3DXFileData* pXFileData,
    const TUInt8** ppLockData,
    TUInt32*       pSize /*= 0*/
)
{
	GEN_GUARD;
 
	DWORD iActualSize;
	HRESULT xFileError = 
		pXFileData->Lock( &iActualSize, reinterpret_cast<const void**>(ppLockData) );
	GEN_ASSERT( xFileError == S_OK, "Unexpected failure in Lock" );
	if (pSize)
	{
		if (*pSize != 0 && *pSize != iActualSize)
		{
			pXFileData->Unlock();
			return kInvalidData;
		}
		*pSize = iActualSize;
	}

	return kSuccess;

	GEN_ENDGUARD;
}

void CImportXFile::UnlockXFileData
(
	ID3DXFileData* pXFileData
)
{
	GEN_GUARD;
 
	pXFileData->Unlock();

	GEN_ENDGUARD;
}


EImportError CImportXFile::CopyXFileData
(
	ID3DXFileData* pXFileData,
    TUInt8*        pDest,
    TUInt32*       pSize
)
{
	GEN_GUARD;
 
	const TUInt8* pLockData;
	EImportError eError = LockXFileData( pXFileData, &pLockData, pSize );
	if (eError != kSuccess)
	{
		return eError;
	}
	memcpy( pDest, pLockData, *pSize );
	UnlockXFileData( pXFileData );

	return kSuccess;

	GEN_ENDGUARD;
}

void CImportXFile::ReadXFileLockedData
(
	const TUInt8*& pMeshData,
	TUInt8*        pDest,
	const TUInt32  iSize
)
{
	GEN_GUARD;
 
	memcpy( pDest, pMeshData, iSize );
	pMeshData += iSize;

	GEN_ENDGUARD;
}

void CImportXFile::ReadXFileLockedUInt
(
	const TUInt8*& pMeshData,
	TUInt32*       piDest
)
{
	GEN_GUARD;
 
	memcpy( piDest, pMeshData, 4 );
	pMeshData += 4;

	GEN_ENDGUARD;
}

void CImportXFile::ReadXFileLockedUInt16
(
	const TUInt8*& pMeshData,
	TUInt16*       piDest
)
{
	GEN_GUARD;
 
	memcpy( piDest, pMeshData, 2 );
	pMeshData += 2;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	X-file type support
-----------------------------------------------------------------------------------------*/

// Equality operator for SXFileMaterial structure (needed for searching material lists)
bool operator==
(
	const CImportXFile::SXFileMaterial& cmp1,
	const CImportXFile::SXFileMaterial& cmp2
)
{
	return cmp1.faceColour.fRed == cmp2.faceColour.fRed &&
	       cmp1.faceColour.fGreen == cmp2.faceColour.fGreen &&
	       cmp1.faceColour.fBlue == cmp2.faceColour.fBlue &&
	       cmp1.faceColour.fAlpha == cmp2.faceColour.fAlpha &&
	       cmp1.fSpecularPower == cmp2.fSpecularPower &&
	       cmp1.specularColour.fRed == cmp2.specularColour.fRed &&
	       cmp1.specularColour.fGreen == cmp2.specularColour.fGreen &&
	       cmp1.specularColour.fBlue == cmp2.specularColour.fBlue &&
	       cmp1.emmisiveColour.fRed == cmp2.emmisiveColour.fRed &&
	       cmp1.emmisiveColour.fGreen == cmp2.emmisiveColour.fGreen &&
	       cmp1.emmisiveColour.fBlue == cmp2.emmisiveColour.fBlue &&
		   cmp1.sTextureName == cmp2.sTextureName;
}


/*-----------------------------------------------------------------------------------------
	Geometry processing
-----------------------------------------------------------------------------------------*/

// Match the face lists of vertices and normals, so there is exactly one normal per vertex
// See the comment to SXFileMesh::normalFaces in the header file
void CImportXFile::MatchFaceLists
(
	const TUInt32  iMesh
)
{
	GEN_GUARD;

	// Unclutter code with a reference to the mesh 
	SXFileMesh& mesh = m_Meshes[iMesh];

	if (!mesh.normals.empty())
	{
		// Maximum vertices (and normals) possible is the total number of indices in the original
		// face lists (if each index was different). Use std library accumulate from <numeric>
		TUInt32 iMaxVertices = accumulate( mesh.origFaceEdges.begin(), 
		                                   mesh.origFaceEdges.end(), 0 );

		// Create empty vertex and normal maps - use max vertex value as unused marker
		TXFileInts vertexMap( iMaxVertices, iMaxVertices );
		TXFileInts normalMap( iMaxVertices, iMaxVertices );

		// Table of vertex duplicates created by this process, each entry is next copy of vertex
		TXFileInts vertexDup( iMaxVertices, iMaxVertices );

		// May need to duplicate vertices, count from original number of vertices
		TUInt32 iNewNumVertices = static_cast<TUInt32>(mesh.vertices.size()); 

		// Create vertex and normal mapping tables
		for (TUInt32 iFace = 0; iFace != mesh.faces.size(); ++iFace)
		{
			// Unclutter code with references to current faces (vertex and normal)
			SXFileFace& vertexFace = mesh.faces[iFace];
			SXFileFace& normalFace = mesh.normalFaces[iFace];

			// For each face edge
			for (int i = 0; i < 3; ++i)
			{
				// Will try to use vertex face index for the normal face index, if unused then OK
				if (normalMap[vertexFace.aiVertex[i]] == iMaxVertices) 
				{
					// Use the vertex face index for both vertices and normals
					vertexMap[vertexFace.aiVertex[i]] = vertexFace.aiVertex[i];
					normalMap[vertexFace.aiVertex[i]] = normalFace.aiVertex[i];
				}
				// otherwise the vertex face index has already been used...
				else
				{
					// See if the normal face index is correct for this vertex or the duplicates
					// already created
					TUInt32 iVert = vertexFace.aiVertex[i];
					while (normalMap[iVert] != normalFace.aiVertex[i] &&
					       vertexDup[iVert] != iMaxVertices)
					{
						iVert = vertexDup[iVert];
					}
					// Can't find correct normal face index on any of this vertices copies, so
					// create another copy
					if (normalMap[iVert] != normalFace.aiVertex[i])
					{
						// Add new vertex index
						vertexMap[iNewNumVertices] = vertexFace.aiVertex[i];
						normalMap[iNewNumVertices] = normalFace.aiVertex[i];
						vertexDup[iVert] = iNewNumVertices;

						// Update the face indices to point at this new vertex
						vertexFace.aiVertex[i] = iNewNumVertices;
						++iNewNumVertices;
					}
					else
					{
						// Update the face indices to point at appropriate vertex duplicate
						vertexFace.aiVertex[i] = iVert;
					}
				}
			}
		}

		// Add any required duplicate vertex data (if necessary)
		TUInt32 iOldNumVertices = static_cast<TUInt32>(mesh.vertices.size());
		if (iNewNumVertices > iOldNumVertices)
		{
			// For every added vertex...
			for (TUInt32 iVertex = iOldNumVertices; iVertex < iNewNumVertices; ++iVertex)
			{
				// Use the vertex map to duplicate the various vertex data
				mesh.vertices.push_back( mesh.vertices[vertexMap[iVertex]] );
				if (!mesh.textureCoords.empty())
				{
					mesh.textureCoords.push_back( mesh.textureCoords[vertexMap[iVertex]] );
				}
				if (!mesh.vertexColours.empty())
				{
					mesh.vertexColours.push_back( mesh.vertexColours[vertexMap[iVertex]] );
				}
				if (!mesh.duplicateIndices.empty())
				{
					mesh.duplicateIndices.push_back( mesh.duplicateIndices[vertexMap[iVertex]] );
				}
			}
		}

		// Build full updated normal list and replace original normals
		TXFileVectors newNormals( iNewNumVertices );
		for (TUInt32 iNormal = 0; iNormal < iNewNumVertices; ++iNormal)
		{
			newNormals[iNormal] = mesh.normals[normalMap[iNormal]];
		}
		mesh.normals.swap( newNormals );
	}

	mesh.origFaceEdges.clear();
	mesh.normalFaces.clear();

	GEN_ENDGUARD;
}


// Create a global list of materials used by all the meshes - removing any duplicates. Also 
// create a list for each mesh mapping local material indices to global ones
void CImportXFile::MakeGlobalMaterialList()
{
	GEN_GUARD;

	// Process each mesh
	for (TUInt32 iMesh = 0; iMesh < m_Meshes.size(); ++iMesh)
	{
		// Unclutter code with a reference to the mesh 
		SXFileMesh& mesh = m_Meshes[iMesh];
		
		// Initialise material map for this mesh and look through each of its materials
		mesh.materialMap.resize( mesh.materials.size() );
		for (TUInt32 iMaterial = 0; iMaterial < mesh.materials.size(); ++iMaterial)
		{
			// See if this material is already in the global list
			TXFileMaterials::iterator duplicate = find( m_Materials.begin(), m_Materials.end(), 
														mesh.materials[iMaterial] );
			
			// If not found...
			if (duplicate == m_Materials.end())
			{
				// ...add new global material...
				mesh.materialMap[iMaterial] = static_cast<TUInt32>(m_Materials.size());
				m_Materials.push_back( mesh.materials[iMaterial] );
			}
			else
			{
				// ...otherwise refer to existing material
				mesh.materialMap[iMaterial] =
					static_cast<TUInt32>(duplicate - m_Materials.begin());
			}
		}
	}

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Bone support functions
-----------------------------------------------------------------------------------------*/

// Add new bone weight/index to a vertex - maximum of 4, removes least signficant if necessary
void CImportXFile::AddBoneInfluence( TUInt32 bone, TFloat32 weight,
                                     TFloat32* vertWeights, TUInt8* vertBones )
{
	GEN_GUARD;

	// Store weights (& indices) in decreasing order - find position for new weight
	if (weight > vertWeights[0])
	{
		vertWeights[3] = vertWeights[2];
		vertWeights[2] = vertWeights[1];
		vertWeights[1] = vertWeights[0];
		vertWeights[0] = weight;
		vertBones[3] = vertBones[2];
		vertBones[2] = vertBones[1];
		vertBones[1] = vertBones[0];
		vertBones[0] = bone;
	}
	else if (weight > vertWeights[1])
	{
		vertWeights[3]	= vertWeights[2];
		vertWeights[2]	= vertWeights[1];
		vertWeights[1] = weight;
		vertBones[3] = vertBones[2];
		vertBones[2] = vertBones[1];
		vertBones[1] = bone;
	}
	else if (weight > vertWeights[2])
	{
		vertWeights[3]	= vertWeights[2];
		vertWeights[2] = weight;
		vertBones[3] = vertBones[2];
		vertBones[2] = bone;
	}
	else if (weight > vertWeights[3])
	{
		vertWeights[3] = weight;
		vertBones[3] = bone;
	}
	// else weight ignored

	GEN_ENDGUARD;
}


// Match the bones in each mesh to their frames
// Possible return values:
//		kInvalidData:		Could not find a frame matching one of the bones
EImportError CImportXFile::ProcessBones()
{
	GEN_GUARD;

	for (TUInt32 iMesh = 0; iMesh < m_Meshes.size(); ++iMesh)
	{
		for (TUInt32 iBone = 0; iBone < m_Meshes[iMesh].bones.size(); ++iBone)
		{
			bool bFoundFrame = false;
			for (TUInt32 iFrame = 0; iFrame < m_Frames.size(); ++iFrame)
			{
				if (m_Meshes[iMesh].bones[iBone].sFrameName == m_Frames[iFrame].sName)
				{
					m_Meshes[iMesh].bones[iBone].iFrame = iFrame;
					m_Frames[iFrame].offsetMatrix = m_Meshes[iMesh].bones[iBone].offsetMatrix;
					bFoundFrame = true;
					break;
				}
			}
			if (!bFoundFrame)
			{
				return kInvalidData;
			}
		}
	}

	return kSuccess;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Mesh processing
-----------------------------------------------------------------------------------------*/

// Split each mesh into a set of meshes - each of which contains only a single material
void CImportXFile::SplitMeshes()
{
	GEN_GUARD;

	TUInt32 iNumOrigMeshes = static_cast<TUInt32>(m_Meshes.size());
	for (TUInt32 iMesh = 0; iMesh < iNumOrigMeshes; ++iMesh)
	{
		TUInt32 iMaxVertices = static_cast<TUInt32>(m_Meshes[iMesh].vertices.size());

		for (TUInt32 iMaterial = 0; iMaterial < m_Meshes[iMesh].materials.size(); ++iMaterial)
		{
			SXFileMesh newMesh;
			newMesh.iParentFrame = m_Meshes[iMesh].iParentFrame;
			newMesh.materials.push_back( m_Meshes[iMesh].materials[iMaterial] );
			newMesh.materialMap.push_back( m_Meshes[iMesh].materialMap[iMaterial] );

			TXFileInts vertexMap( iMaxVertices, iMaxVertices );

			for (TUInt32 iFace = 0; iFace < m_Meshes[iMesh].faceMaterials.size(); ++iFace)
			{
				if (m_Meshes[iMesh].faceMaterials[iFace] == iMaterial)
				{
					newMesh.faceMaterials.push_back( 0 );
					SXFileFace newFace;
					for (TUInt32 iIndex = 0; iIndex < 3; ++iIndex)
					{
						TUInt32 iVert = m_Meshes[iMesh].faces[iFace].aiVertex[iIndex];
						if (vertexMap[iVert] == iMaxVertices)
						{
							vertexMap[iVert] = static_cast<TUInt32>(newMesh.vertices.size());
							newMesh.vertices.push_back( m_Meshes[iMesh].vertices[iVert] );
							if (m_Meshes[iMesh].normals.size() > 0)
							{
								newMesh.normals.push_back( m_Meshes[iMesh].normals[iVert] );
							}
							if (m_Meshes[iMesh].textureCoords.size() > 0)
							{
								newMesh.textureCoords.push_back( m_Meshes[iMesh].textureCoords[iVert] );
							}
							if (m_Meshes[iMesh].vertexColours.size() > 0)
							{
								newMesh.vertexColours.push_back( m_Meshes[iMesh].vertexColours[iVert] );
							}
						}
						newFace.aiVertex[iIndex] = vertexMap[iVert];
					}
					newMesh.faces.push_back( newFace );
				}
			}
			if (newMesh.vertices.size() > 0)
			{
				m_Meshes.push_back( newMesh );
			}
		}
	}
	m_Meshes.erase( m_Meshes.begin(), m_Meshes.begin() + iNumOrigMeshes );

	GEN_ENDGUARD;
}


// Create a list of tangent vectors for the given mesh. The tangent vector is the direction of
// a vertex's texture U axis in model-space. Returns true on success
bool CImportXFile::CalculateTangents
(
	TUInt32 iMesh,
	TXFileVectors* pTangents
) const
{
	// Normals and UVs are required for tangent calculation
	if (!m_Meshes[iMesh].normals.size() || !m_Meshes[iMesh].textureCoords.size())
	{
		return false;
	}

	pTangents->clear();
	pTangents->resize( m_Meshes[iMesh].vertices.size(), CVector3::kOrigin );

	// Step through faces
	for (TUInt32 iFace = 0; iFace < m_Meshes[iMesh].faces.size(); ++iFace)
	{
		int i1 = m_Meshes[iMesh].faces[iFace].aiVertex[0];
		int i2 = m_Meshes[iMesh].faces[iFace].aiVertex[1];
		int i3 = m_Meshes[iMesh].faces[iFace].aiVertex[2];

		CVector3 v1 = m_Meshes[iMesh].vertices[i1];
		CVector3 v2 = m_Meshes[iMesh].vertices[i2];
		CVector3 v3 = m_Meshes[iMesh].vertices[i3];

		SXFileUV uv1 = m_Meshes[iMesh].textureCoords[i1];
		SXFileUV uv2 = m_Meshes[iMesh].textureCoords[i2];
		SXFileUV uv3 = m_Meshes[iMesh].textureCoords[i3];

		CVector3 edge1 = v2 - v1;
		CVector3 edge2 = v3 - v1;

		TFloat32 s1 = uv2.fU - uv1.fU;
		TFloat32 s2 = uv3.fU - uv1.fU;
		TFloat32 t1 = uv2.fV - uv1.fV;
		TFloat32 t2 = uv3.fV - uv1.fV;

		CVector3 tangent;
		TFloat32 denom = s1 * t2 - s2 * t1;
		if (!gen::IsZero(denom))
		{
			tangent = (t2 * edge1 - t1 * edge2) / (s1 * t2 - s2 * t1);
		}
		else
		{
			tangent = CVector3::kXAxis;
		}

		(*pTangents)[i1] += tangent;
		(*pTangents)[i2] += tangent;
		(*pTangents)[i3] += tangent;
	}

	// Orthogonalise normals and tangents
	for (TUInt32 iVert = 0; iVert < m_Meshes[iMesh].vertices.size(); ++iVert)
	{
		// Gram-Schmidt orthogonalize
		TFloat32 dot = Dot( m_Meshes[iMesh].normals[iVert], (*pTangents)[iVert] );
		(*pTangents)[iVert] -= dot * m_Meshes[iMesh].normals[iVert];
		(*pTangents)[iVert].Normalise();
	}

	return true;
}


} // namespace gen
