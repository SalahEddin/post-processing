/*******************************************
	
	Light.h

	Light class declarations

********************************************/

#pragma once

#include "Defines.h"
#include "CVector3.h"
#include "Colour.h"
#include "Input.h"

namespace gen
{

class CLight
{
public:
	// Constructor / Destructor
	CLight( const CVector3& pos, const SColourRGBA& colour, TFloat32 bright = 100.0f );


	// Getters
	CVector3 GetPosition()
	{
		return m_Position;
	}
	SColourRGBA GetColour()
	{
		return m_Colour;
	}
	TFloat32 GetBrightness()
	{
		return m_Bright;
	}

	// Setters
	void SetPosition( const CVector3& pos )
	{
		m_Position = pos;
	}
	void SetColour( const SColourRGBA& colour )
	{
		m_Colour = colour;
	}
	void SetBrightness( float bright )
	{
		m_Bright = bright;
	}


	// Control the light with keys
	void Control( EKeyCode moveForward, EKeyCode moveBackward,
	              EKeyCode moveLeft, EKeyCode moveRight,
	              EKeyCode moveUp, EKeyCode moveDown,
				  TFloat32 MoveSpeed );


private:
	// Light settings
	CVector3    m_Position;
	SColourRGBA m_Colour;
	TFloat32    m_Bright;
};


} // namespace gen