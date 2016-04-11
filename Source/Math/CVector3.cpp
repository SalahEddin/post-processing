/**************************************************************************************************
	Module:       CVector3.cpp
	Author:       Laurent Noel
	Date created: 12/06/06

	Implementation of the concrete class CVector3, three 32-bit floats representing a vector/point
	with x, y & z components - or a column/row of a 3x3 matrix

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "CVector3.h"
#include "CVector4.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/

// Construct from a CVector4, discarding w value
CVector3::CVector3( const CVector4& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
}


/*-----------------------------------------------------------------------------------------
	Length operations
-----------------------------------------------------------------------------------------*/

// Reduce vector to unit length - member function
void CVector3::Normalise()
{
	TFloat32 lengthSq = x*x + y*y + z*z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( gen::IsZero( lengthSq ) )
	{
		x = y = z = 0.0f;
	}
	else
	{
		TFloat32 invLength = InvSqrt( lengthSq );
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
}


// Return unit length vector in the same direction as given one
CVector3 Normalise( const CVector3& v )
{
	TFloat32 lengthSq = v.x*v.x + v.y*v.y + v.z*v.z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( gen::IsZero( lengthSq ) )
	{
		return CVector3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		TFloat32 invLength = InvSqrt( lengthSq );
		return CVector3(v.x * invLength, v.y * invLength, v.z * invLength);
	}
}


/*-----------------------------------------------------------------------------------------
	Point related functions
-----------------------------------------------------------------------------------------*/

// Return distance from this point to another - member function
TFloat32 CVector3::DistanceTo( const CVector3& p )
{
	TFloat32 distX = p.x - x;
	TFloat32 distY = p.y - y;
	TFloat32 distZ = p.z - z;
	return Sqrt( distX*distX + distY*distY + distZ*distZ );
}


// Return squared distance from this point to another - member function
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceToSquared(...) ) to calculate 1 / distance more efficiently
TFloat32 CVector3::DistanceToSquared( const CVector3& p )
{
	TFloat32 distX = p.x - x;
	TFloat32 distY = p.y - y;
	TFloat32 distZ = p.z - z;
	return distX*distX + distY*distY + distZ*distZ;
}


// Return distance from one point to another - non-member version
TFloat32 Distance
(
	const CVector3& p1,
	const CVector3& p2
)
{
	TFloat32 distX = p1.x - p2.x;
	TFloat32 distY = p1.y - p2.y;
	TFloat32 distZ = p1.z - p2.z;
	return Sqrt( distX*distX + distY*distY + distZ*distZ );
}

// Return squared distance from one point to another - non-member version
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceSquared(...) ) to calculate 1 / distance more efficiently
TFloat32 DistanceSquared
(
	const CVector3& p1,
	const CVector3& p2
)
{
	TFloat32 distX = p1.x - p2.x;
	TFloat32 distY = p1.y - p2.y;
	TFloat32 distZ = p1.z - p2.z;
	return distX*distX + distY*distY + distZ*distZ;
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard vectors
const CVector3 CVector3::kZero(0.0f, 0.0f, 0.0f);
const CVector3 CVector3::kOne(1.0f, 1.0f, 1.0f);
const CVector3 CVector3::kOrigin(0.0f, 0.0f, 0.0f);
const CVector3 CVector3::kXAxis(1.0f, 0.0f, 0.0f);
const CVector3 CVector3::kYAxis(0.0f, 1.0f, 0.0f);
const CVector3 CVector3::kZAxis(0.0f, 0.0f, 1.0f);


} // namespace gen
