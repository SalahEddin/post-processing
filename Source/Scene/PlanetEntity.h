/*******************************************
	PlanetEntity.h

	Planet entity class	definition
********************************************/

#pragma once

#include <string>
using namespace std;

#include "Defines.h"
#include "Entity.h"

namespace gen
{

/////////////////////////////////////////////////////////////////////////
// No need for a specialised planet template class, only requires base
// functionality (mesh management)
/////////////////////////////////////////////////////////////////////////


/*-----------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------
	Planet Entity Class
-------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------*/

// A planet inherits the ID/positioning/rendering support of the base class and adds
// instance status (spin speed only). It also provides an update function to perform spin
class CPlanetEntity : public CEntity
{
/////////////////////////////////////
//	Constructors/Destructors
public:
	// Planet constructor intialises car-specific data and passes parameters to base class constructor
	CPlanetEntity
	(
		CEntityTemplate* planetTemplate,
		TEntityUID       UID,
		const string&    name = "",
		TFloat32         spinSpeed = kfPi,
		const CVector3&  position = CVector3::kOrigin, 
		const CVector3&  rotation = CVector3( 0.0f, 0.0f, 0.0f ),
		const CVector3&  scale = CVector3( 1.0f, 1.0f, 1.0f )
	);

	// No destructor needed

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	CPlanetEntity( const CPlanetEntity& );
	CPlanetEntity& operator=( const CPlanetEntity& );


/////////////////////////////////////
//	Public interface
public:

	/////////////////////////////////////
	// Getters / Setters

	const TFloat32 GetSpinSpeed()
	{
		return m_SpinSpeed;
	}

	void SetSpinSpeed( const TFloat32 speed )
	{
		m_SpinSpeed = speed;
	}


	/////////////////////////////////////
	// Update

	// Update the car - performs car message processing and behaviour
	// Return false if the entity is to be destroyed
	// Keep as a virtual function in case of further derivation
	virtual bool Update( TFloat32 updateTime );
	

/////////////////////////////////////
//	Private interface
private:

	/////////////////////////////////////
	// Data

	// Entity status
	TFloat32 m_SpinSpeed;  // Current spin speed of this planet (Y rotation)
};


} // namespace gen
