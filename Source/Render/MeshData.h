///////////////////////////////////////////////////////////
//  MeshData.h
//  Mesh related definitions
//  Created on:      28-Jul-2005 17:10:10
//  Original author: LN
///////////////////////////////////////////////////////////

#ifndef GEN_MESH_H_INCLUDED
#define GEN_MESH_H_INCLUDED

#include <vector>
#include <string>
using namespace std;

#include "Defines.h"
#include "Colour.h"
#include "CMatrix4x4.h"
#include "RenderMethod.h"

namespace gen
{

/////////////////////////////////////
// Mesh definitions

// A single node in the hierarchy of a mesh. The hierarchy is flattened (depth-first) into a list
struct SMeshNode
{ 
	string     name;           // Name for the node
	TUInt32    depth;          // Depth in hierachy of this node
	TUInt32    parent;         // Index in hierarchy list of parent node
	TUInt32    numChildren;    // Number of children of this node - the next node in the list will
	                           // be the first child
	CMatrix4x4 positionMatrix; // Default matrix of this node in parent space
	CMatrix4x4 invMeshOffset;  // Inverse of the matrix of this node in mesh's root space
};


// A single face in a mesh - all faces are triangles
struct SMeshFace
{
	TUInt16 aiVertex[3];
};
typedef vector<SMeshFace> TMeshFaces;

// A sub-mesh is a single block of geometry that uses the same material. It contains a set of faces
// and vertices and is controlled by a single node. The vertices are pointed to as raw bytes,
// because of the flexibility of vertex data
struct SSubMesh
{
	TUInt32    node;        // Node in heirarchy controlling this submesh
	TUInt32    material;    // Index of material used by this submesh
	TUInt32    numVertices;
	TUInt8*    vertices;    // Pointer to raw vertex data as a byte stream
	TUInt32    vertexSize;  // Size in bytes of a single vertex
	bool       hasSkinningData, hasNormals, hasTangents, // Components of each vertex
	           hasTextureCoords, hasVertexColours;       // (Vertex coordinate assumed)
	TUInt32    numFaces;
	SMeshFace* faces;
};



const TUInt32 kiMaxTextures = 4;

// A material indicating how to render a sub-mesh - each sub-mesh uses a single material
struct SMeshMaterial
{
	ERenderMethod renderMethod;

	SColourRGBA   diffuseColour;
	SColourRGBA   specularColour;
	TFloat32      specularPower;

	TUInt32       numTextures;
	string        textureFileNames[kiMaxTextures];
};


} // namespace gen

#endif // GEN_MESH_H_INCLUDED
