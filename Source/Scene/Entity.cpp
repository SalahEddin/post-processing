/*******************************************
	Entity.cpp

	Entity class implementation
********************************************/

#include "Entity.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------
	Base Entity Class
-------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------*/

// Base entity constructor, needs pointer to common template data and UID, may also pass 
// name, initial position, rotation and scaling. Set up positional matrices for the entity
CEntity::CEntity
(
	CEntityTemplate* entityTemplate,
	TEntityUID       UID,
	const string&    name /*=""*/,
	const CVector3&  position /*= CVector3::kOrigin*/, 
	const CVector3&  rotation /*= CVector3( 0.0f, 0.0f, 0.0f )*/,
	const CVector3&  scale /*= CVector3( 1.0f, 1.0f, 1.0f )*/
)
{
	m_Template = entityTemplate;
	m_UID = UID;
	m_Name = name;

	// Allocate space for matrices
	TUInt32 numNodes = m_Template->Mesh()->GetNumNodes();
	m_RelMatrices = new CMatrix4x4[numNodes];
	m_Matrices = new CMatrix4x4[numNodes];

	// Set initial matrices from mesh defaults
	for (TUInt32 node = 0; node < numNodes; ++node)
	{
		m_RelMatrices[node] = m_Template->Mesh()->GetNode( node ).positionMatrix;
	}

	// Override root matrix with constructor parameters
	m_RelMatrices[0] = CMatrix4x4( position, rotation, kZXY, scale );
}


// Render the model from the given camera
// May request to render either normal or post-processed materials in the entity (defaults to normal)
void CEntity::Render( CCamera* camera, bool postProcess /*= false*/ )
{
	// Get pointer to mesh to simplify code
	CMesh* Mesh = m_Template->Mesh();

	// Calculate absolute matrices from relative node matrices & node heirarchy
	m_Matrices[0] = m_RelMatrices[0];
	TUInt32 numNodes = Mesh->GetNumNodes();
	for (TUInt32 node = 1; node < numNodes; ++node)
	{
		m_Matrices[node] = m_RelMatrices[node] * m_Matrices[Mesh->GetNode( node ).parent];
	}
	// Incorporate any bone<->mesh offsets (only relevant for skinning)
	// Don't need this step for this exercise

	// Render with absolute matrices
	Mesh->Render( m_Matrices, camera, postProcess );
}


} // namespace gen
