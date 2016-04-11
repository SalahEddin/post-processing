/**************************************************************************************************
	Module:       CVector2.cpp
	Author:       Laurent Noel
	Date created: 12/06/06

	Implementation of the concrete class CVector2, two 32-bit floats representing a vector/point
	with x & y components - or a column/row of a 2x2 matrix

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "CVector2.h"
#include "CVector3.h"
#include "CVector4.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/

// Construct from a CVector3, discarding z value
CVector2::CVector2( const CVector3& v )
{
	x = v.x;
	y = v.y;
}

// Construct from a CVector4, discarding z & w values
CVector2::CVector2( const CVector4& v )
{
	x = v.x;
	y = v.y;
}

/*-----------------------------------------------------------------------------------------
	Length operations
-----------------------------------------------------------------------------------------*/

// Reduce vector to unit length - member function
void CVector2::Normalise()
{
	TFloat32 lengthSq = x*x + y*y;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( gen::IsZero( lengthSq ) )
	{
		x = y = 0.0f;
	}
	else
	{
		TFloat32 invLength = InvSqrt( lengthSq );
		x *= invLength;
		y *= invLength;
	}
}


// Return unit length vector in the same direction as given one
CVector2 Normalise( const CVector2& v )
{
	TFloat32 lengthSq = v.x*v.x + v.y*v.y;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( gen::IsZero( lengthSq ) )
	{
		return CVector2(0.0f, 0.0f);
	}
	else
	{
		TFloat32 invLength = InvSqrt( lengthSq );
		return CVector2(v.x * invLength, v.y * invLength);
	}
}


/*-----------------------------------------------------------------------------------------
	Point related functions
-----------------------------------------------------------------------------------------*/

// Return distance from this point to another - member function
TFloat32 CVector2::DistanceTo( const CVector2& p )
{
	TFloat32 distX = p.x - x;
	TFloat32 distY = p.y - y;
	return Sqrt( distX*distX + distY*distY );
}


// Return squared distance from this point to another - member function
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceToSquared(...) ) to calculate 1 / distance more efficiently
TFloat32 CVector2::DistanceToSquared( const CVector2& p )
{
	TFloat32 distX = p.x - x;
	TFloat32 distY = p.y - y;
	return distX*distX + distY*distY;
}


// Return distance from one point to another - non-member version
TFloat32 Distance
(
	const CVector2& p1,
	const CVector2& p2
)
{
	TFloat32 distX = p1.x - p2.x;
	TFloat32 distY = p1.y - p2.y;
	return Sqrt( distX*distX + distY*distY );
}

// Return squared distance from one point to another - non-member version
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceSquared(...) ) to calculate 1 / distance more efficiently
TFloat32 DistanceSquared
(
	const CVector2& p1,
	const CVector2& p2
)
{
	TFloat32 distX = p1.x - p2.x;
	TFloat32 distY = p1.y - p2.y;
	return distX*distX + distY*distY;
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard vectors
const CVector2 CVector2::kZero(0.0f, 0.0f);
const CVector2 CVector2::kOne(1.0f, 1.0f);
const CVector2 CVector2::kOrigin(0.0f, 0.0f);
const CVector2 CVector2::kXAxis(1.0f, 0.0f);
const CVector2 CVector2::kYAxis(0.0f, 1.0f);


} // namespace gen
