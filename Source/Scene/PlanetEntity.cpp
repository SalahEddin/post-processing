/*******************************************
	PlanetEntity.cpp

	Planet entity class	definition
********************************************/

#include "PlanetEntity.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Ship Entity Class
-----------------------------------------------------------------------------------------*/

	// Planet constructor intialises car-specific data and passes parameters to base class constructor
CPlanetEntity::CPlanetEntity
(
	CEntityTemplate* planetTemplate,
	TEntityUID       UID,
	const string&    name /*= ""*/,
	TFloat32         spinSpeed /*= kfPi*/,
	const CVector3&  position /*= CVector3::kOrigin*/, 
	const CVector3&  rotation /*= CVector3( 0.0f, 0.0f, 0.0f )*/,
	const CVector3&  scale /*= CVector3( 1.0f, 1.0f, 1.0f )*/
) : CEntity( planetTemplate, UID, name, position, rotation, scale )
{
	m_SpinSpeed = spinSpeed;
}


// Update the planet
// Return false if the entity is to be destroyed
bool CPlanetEntity::Update( TFloat32 updateTime )
{
	Matrix().RotateLocalY( m_SpinSpeed * updateTime );
	return true;
}


} // namespace gen
