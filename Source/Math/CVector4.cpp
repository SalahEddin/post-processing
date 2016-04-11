/**************************************************************************************************
	Module:       CVector4.cpp
	Author:       Laurent Noel
	Date created: 12/06/06

	Implementation of the concrete class CVector4, four 32-bit floats representing a vector/point
	with x, y, z & w components - or a row or column of a 4x4 matrix

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "CVector4.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Length operations
-----------------------------------------------------------------------------------------*/

// Reduce vector to unit length - member function
void CVector4::Normalise()
{
	TFloat32 lengthSq = x*x + y*y + z*z + w*w;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( gen::IsZero( lengthSq ) )
	{
		x = y = z = w = 0.0f;
	}
	else
	{
		TFloat32 invLength = InvSqrt( lengthSq );
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}
}


// Return unit length vector in the same direction as given one - non-member function
CVector4 Normalise( const CVector4& v )
{
	TFloat32 lengthSq = v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( gen::IsZero( lengthSq ) )
	{
		return CVector4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		TFloat32 invLength = InvSqrt( lengthSq );
		return CVector4(v.x * invLength, v.y * invLength, v.z * invLength, v.w * invLength);
	}
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard vectors
const CVector4 CVector4::kZero(0.0f, 0.0f, 0.0f, 0.0f);
const CVector4 CVector4::kOne(1.0f, 1.0f, 1.0f, 1.0f);
const CVector4 CVector4::kOrigin(0.0f, 0.0f, 0.0f, 0.0f);
const CVector4 CVector4::kXAxis(1.0f, 0.0f, 0.0f, 0.0f);
const CVector4 CVector4::kYAxis(0.0f, 1.0f, 0.0f, 0.0f);
const CVector4 CVector4::kZAxis(0.0f, 0.0f, 1.0f, 0.0f);
const CVector4 CVector4::kWAxis(0.0f, 0.0f, 0.0f ,1.0f);


} // namespace gen
