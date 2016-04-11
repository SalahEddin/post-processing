/*******************************************
	
	Light.cpp

	Light class implementation

********************************************/

#include "Light.h"

namespace gen
{

// Constructor / Destructor
CLight::CLight( const CVector3& pos, const SColourRGBA& colour, TFloat32 bright /*= 100.0f*/ )
{
	SetPosition( pos );
	SetColour( colour );
	SetBrightness( bright );
}


// Control the light with keys
void CLight::Control( EKeyCode moveForward, EKeyCode moveBackward,
                      EKeyCode moveLeft, EKeyCode moveRight,
                      EKeyCode moveUp, EKeyCode moveDown,
					  TFloat32 MoveSpeed )
{
	if (KeyHeld( moveRight ))
	{
		m_Position.x += MoveSpeed;
	}
	if (KeyHeld( moveLeft ))
	{
		m_Position.x -= MoveSpeed;
	}
	if (KeyHeld( moveUp ))
	{
		m_Position.y += MoveSpeed;
	}
	if (KeyHeld( moveDown ))
	{
		m_Position.y -= MoveSpeed;
	}
	if (KeyHeld( moveForward ))
	{
		m_Position.z += MoveSpeed;
	}
	if (KeyHeld( moveBackward ))
	{
		m_Position.z -= MoveSpeed;
	}
}


} // namespace gen