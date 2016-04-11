/**************************************************************************************************
	Module:       CMatrix3x3.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Implementation of the concrete class CMatrix3x3, a 3x3 matrix of 32-bit floats. Designed for
	transformation matrices for 3D graphics or affine transformations for 2D graphics

	See notes at top of header for some background to this class. Also see "Essential Mathematics
	for Games & Interactive Applications" (Van Verth & Bishop) for the mathematics behind the more
	complex code (e.g rotation conversions)

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "CMatrix3x3.h"

#include "Error.h"
#include "CMatrix2x2.h"
#include "CQuaternion.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/

// Construct by value
CMatrix3x3::CMatrix3x3
(
	const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02,
	const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12,
	const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22
)
{
	e00 = elt00;
	e01 = elt01;
	e02 = elt02;

	e10 = elt10;
	e11 = elt11;
	e12 = elt12;

	e20 = elt20;
	e21 = elt21;
	e22 = elt22;
}

// Construct through pointer to 9 floats, may specify row/column order of data
CMatrix3x3::CMatrix3x3
(
	const TFloat32* pfElts,
	const bool      bRows /*= true*/
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( pfElts, "Invalid parameter" );

	if (bRows)
	{
		e00 = pfElts[0];
		e01 = pfElts[1];
		e02 = pfElts[2];

		e10 = pfElts[3];
		e11 = pfElts[4];
		e12 = pfElts[5];

		e20 = pfElts[6];
		e21 = pfElts[7];
		e22 = pfElts[8];
	}
	else
	{
		e00 = pfElts[0];
		e10 = pfElts[1];
		e20 = pfElts[2];

		e01 = pfElts[3];
		e11 = pfElts[4];
		e21 = pfElts[5];

		e02 = pfElts[6];
		e12 = pfElts[7];
		e22 = pfElts[8];
	}

	GEN_ENDGUARD_OPT;
}

// Construct by row or column using CVector3's, may specify if setting rows or columns
CMatrix3x3::CMatrix3x3
(
	const CVector3& v0,
	const CVector3& v1,
	const CVector3& v2,
	const bool      bRows /*= true*/
)
{
	if (bRows)
	{
		e00 = v0.x;
		e01 = v0.y;
		e02 = v0.z;

		e10 = v1.x;
		e11 = v1.y;
		e12 = v1.z;

		e20 = v2.x;
		e21 = v2.y;
		e22 = v2.z;
	}
	else
	{
		e00 = v0.x;
		e10 = v0.y;
		e20 = v0.z;

		e01 = v1.x;
		e11 = v1.y;
		e21 = v1.z;

		e02 = v2.x;
		e12 = v2.y;
		e22 = v2.z;
	}
}

// Construct by row or column using CVector2's, remaining elements taken from identity matrix
// May specify if setting rows or columns
CMatrix3x3::CMatrix3x3
(
	const CVector2& v0,
	const CVector2& v1,
	const CVector2& v2,
	const bool      bRows /*= true*/
)
{
	if (bRows)
	{
		e00 = v0.x;
		e01 = v0.y;
		e02 = 0.0f;

		e10 = v1.x;
		e11 = v1.y;
		e12 = 0.0f;

		e20 = v2.x;
		e21 = v2.y;
		e22 = 0.0f;
	}
	else
	{
		e00 = v0.x;
		e10 = v0.y;

		e01 = v1.x;
		e11 = v1.y;

		e02 = v2.x;
		e12 = v2.y;

		e20 = 0.0f;
		e21 = 0.0f;
		e22 = 1.0f;
	}
}
 

// Construct matrix transformation from euler angles and optional scaling. Matrix is effectively
// built in this order: M = Scale*Rotation
CMatrix3x3::CMatrix3x3
(
	const CVector3&      angles,
	const ERotationOrder eRotOrder /*= kZXY*/,
	const CVector3&      scale /*= CVector3::kOne*/
)
{
	// First build rotation matrix
	MakeRotation( angles, eRotOrder );

	// Scale matrix
	e00 *= scale.x;
	e01 *= scale.x;
	e02 *= scale.x;

	e10 *= scale.y;
	e11 *= scale.y;
	e12 *= scale.y;

	e20 *= scale.z;
	e21 *= scale.z;
	e22 *= scale.z;
}

// Construct matrix transformation from quaternion and optional scaling. Matrix is effectively
// built in this order: M = Scale*Rotation
CMatrix3x3::CMatrix3x3
(
	const CQuaternion& quat,
	const CVector3&    scale /*= CVector3::kOne*/
)
{
	// Efficiently precalculate some values from the quaternion
	TFloat32 xx = 2*quat.x;
	TFloat32 yy = 2*quat.y;
	TFloat32 zz = 2*quat.z;
	TFloat32 xy = xx*quat.y;
	TFloat32 yz = yy*quat.z;
	TFloat32 zx = zz*quat.x;
	TFloat32 wx = quat.w*xx;
	TFloat32 wy = quat.w*yy;
	TFloat32 wz = quat.w*zz;
	xx *= quat.x;
	yy *= quat.y;
	zz *= quat.z;

	// Fill upper 3x3 matrix, combining scaling with rotation values from the quaternion
	e00 = scale.x * (1 - yy - zz);
	e01 = scale.x * (xy + wz);
	e02 = scale.x * (zx - wy);

	e10 = scale.y * (xy - wz);
	e11 = scale.y * (1 - xx - zz);
	e12 = scale.y * (yz + wx);

	e20 = scale.z * (zx + wy);
	e21 = scale.z * (yz - wx);
	e22 = scale.z * (1 - xx - yy);
}

// Construct matrix transformation from axis/angle of rotation and optional scaling. Matrix is
// effectively built in this order: M = Scale*Rotation
CMatrix3x3::CMatrix3x3
(
	const CVector3& axis,
	const TFloat32  fAngle,
	const CVector3& scale /*= CVector3::kOne*/
)
{
	// First build rotation matrix
	MakeRotation( axis, fAngle );

	// Scale matrix
	e00 *= scale.x;
	e01 *= scale.x;
	e02 *= scale.x;

	e10 *= scale.y;
	e11 *= scale.y;
	e12 *= scale.y;

	e20 *= scale.z;
	e21 *= scale.z;
	e22 *= scale.z;
}


// Construct 2D affine transformation from position (translation) only
CMatrix3x3::CMatrix3x3( const CVector2& position )
{
	// Take most elements from identity
	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;

	// Put position (translation) in bottom row
	e20 = position.x;
	e21 = position.y;
	e22 = 1.0f;
}

// Construct 2D affine transformation from position, rotation angle and optional scaling, with 
// remaining elements taken from the identity matrix. Matrix is effectively built in this
// order: M = Scale*Rotation*Translation
CMatrix3x3::CMatrix3x3
(
	const CVector2& position,
	const TFloat32  fAngle,
	const CVector2& scale /*= CVector2::kOne*/
)
{
	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	e00 = c * scale.x;
	e01 = s * scale.x;
	e02 = 0.0f;

	e10 = -s * scale.y;
	e11 = c * scale.y;
	e12 = 0.0f;

	e20 = position.x;
	e21 = position.y;
	e22 = 1.0f;
}


// Construct from a CMatrix2x2 and optional 2D position, with remaining elements taken from
// the identity matrix
CMatrix3x3::CMatrix3x3
(
	const CMatrix2x2& m,
	const CVector2&   position /*= CVector2::kOrigin*/
)
{
	// Copy 2x2 elements, adding extra identity elements
	e00 = m.e00;
	e01 = m.e01;
	e02 = 0.0f;

	e10 = m.e10;
	e11 = m.e11;
	e12 = 0.0f;

	// Put position (translation) in bottom row
	e20 = position.x;
	e21 = position.y;
	e22 = 1.0f;
}


// Copy constructor
CMatrix3x3::CMatrix3x3( const CMatrix3x3& m )
{
	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;
}

// Assignment operator
CMatrix3x3& CMatrix3x3::operator=( const CMatrix3x3& m )
{
	if ( this != &m )
	{
		e00 = m.e00;
		e01 = m.e01;
		e02 = m.e02;

		e10 = m.e10;
		e11 = m.e11;
		e12 = m.e12;

		e20 = m.e20;
		e21 = m.e21;
		e22 = m.e22;
	}
	return *this;
}


/*-----------------------------------------------------------------------------------------
	Setters
-----------------------------------------------------------------------------------------*/

// Set by value
void CMatrix3x3::Set
(
	const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02,
	const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12,
	const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22
)
{
	e00 = elt00;
	e01 = elt01;
	e02 = elt02;

	e10 = elt10;
	e11 = elt11;
	e12 = elt12;

	e20 = elt20;
	e21 = elt21;
	e22 = elt22;
}

// Set through pointer to 9 floats, may specify column/row order of data
void CMatrix3x3::Set
(
	const TFloat32* pfElts,
	const bool      bRows /*= true*/
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( pfElts, "Invalid parameter" );

	if (bRows)
	{
		e00 = pfElts[0];
		e01 = pfElts[1];
		e02 = pfElts[2];

		e10 = pfElts[3];
		e11 = pfElts[4];
		e12 = pfElts[5];

		e20 = pfElts[6];
		e21 = pfElts[7];
		e22 = pfElts[8];
	}
	else
	{
		e00 = pfElts[0];
		e10 = pfElts[1];
		e20 = pfElts[2];

		e01 = pfElts[3];
		e11 = pfElts[4];
		e21 = pfElts[5];

		e02 = pfElts[6];
		e12 = pfElts[7];
		e22 = pfElts[8];
	}

	GEN_ENDGUARD_OPT;
}


/*-----------------------------------------------------------------------------------------
	Row/column getters & setters
-----------------------------------------------------------------------------------------*/

// Get a single row (range 0-2) of the matrix
CVector3 CMatrix3x3::GetRow( const TUInt32 iRow ) const
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 3, "Invalid parameter" );

	return CVector3( &e00 + iRow * 3 );

	GEN_ENDGUARD_OPT;
}

// Get a single column (range 0-2) of the matrix
CVector3 CMatrix3x3::GetColumn( const TUInt32 iCol ) const
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 3, "Invalid parameter" );

	const TFloat32* pfElts = &e00 + iCol;
	return CVector3( pfElts[0], pfElts[3], pfElts[6] );

	GEN_ENDGUARD_OPT;
}


// Set a single row (range 0-2) of the matrix
void CMatrix3x3::SetRow
(
	const TUInt32   iRow,
	const CVector3& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 3, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iRow * 3;
	pfElts[0] = v.x;
	pfElts[1] = v.y;
	pfElts[2] = v.z;

	GEN_ENDGUARD_OPT;
}

// Set a single row (range 0-2) of the matrix using a CVector2. Third element left unchanged
void CMatrix3x3::SetRow
(
	const TUInt32   iRow,
	const CVector2& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 3, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iRow * 3;
	pfElts[0] = v.x;
	pfElts[1] = v.y;

	GEN_ENDGUARD_OPT;
}


// Set a single column (range 0-2) of the matrix
void CMatrix3x3::SetColumn
(
	const TUInt32   iCol,
	const CVector3& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 3, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iCol;
	pfElts[0] = v.x;
	pfElts[3] = v.y;
	pfElts[6] = v.z;

	GEN_ENDGUARD_OPT;
}

// Set a single column (range 0-2) of the matrix using a CVector2. Third element left unchanged
void CMatrix3x3::SetColumn
(
	const TUInt32   iCol,
	const CVector2& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 3, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iCol;
	pfElts[0] = v.x;
	pfElts[3] = v.y;

	GEN_ENDGUARD_OPT;
}


// Set all rows of the matrix at once
void CMatrix3x3::SetRows
(
	const CVector3& v0,
	const CVector3& v1,
	const CVector3& v2
)
{
	e00 = v0.x;
	e01 = v0.y;
	e02 = v0.z;

	e10 = v1.x;
	e11 = v1.y;
	e12 = v1.z;

	e20 = v2.x;
	e21 = v2.y;
	e22 = v2.z;
}

// Set all rows of the matrix at once using CVector2, with remaining elements taken from the
// identity matrix
void CMatrix3x3::SetRows
(
	const CVector2& v0,
	const CVector2& v1,
	const CVector2& v2
)
{
	e00 = v0.x;
	e01 = v0.y;
	e02 = 0.0f;

	e10 = v1.x;
	e11 = v1.y;
	e12 = 0.0f;

	e20 = v2.x;
	e21 = v2.y;
	e22 = 1.0f;
}


// Set all columns of the matrix at once
void CMatrix3x3::SetColumns
(
	const CVector3& v0,
	const CVector3& v1,
	const CVector3& v2
)
{
	e00 = v0.x;
	e10 = v0.y;
	e20 = v0.z;

	e01 = v1.x;
	e11 = v1.y;
	e21 = v1.z;

	e02 = v2.x;
	e12 = v2.y;
	e22 = v2.z;
}

// Set all columns of the matrix at once using CVector2, with remaining elements taken from the
// identity matrix
void CMatrix3x3::SetColumns
(
	const CVector2& v0,
	const CVector2& v1,
	const CVector2& v2
)
{
	e00 = v0.x;
	e10 = v0.y;

	e01 = v1.x;
	e11 = v1.y;

	e02 = v2.x;
	e12 = v2.y;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}


/*-----------------------------------------------------------------------------------------
	Creation and Decomposition of Affine Matrices
-----------------------------------------------------------------------------------------*/

// Make matrix a transformation using Euler angles & optional scaling, with remaining elements
// taken from the identity matrix. May specify order to apply rotations. Matrix is built in
// this order: M = Scale*Rotation
void CMatrix3x3::MakeTransformEuler
(
	const CVector3&      angles,
	const ERotationOrder eRotOrder /*= kZXY*/,
	const CVector3&      scale /*= CVector3::kOne*/
)
{
	// First build rotation matrix
	MakeRotation( angles, eRotOrder );

	// Scale matrix
	e00 *= scale.x;
	e01 *= scale.x;
	e02 *= scale.x;

	e10 *= scale.y;
	e11 *= scale.y;
	e12 *= scale.y;

	e20 *= scale.z;
	e21 *= scale.z;
	e22 *= scale.z;
}

// Make matrix a transformation using quaternion & optional scaling, with remaining elements
// taken from the identity matrix. Matrix is built in this order: M = Scale*Rotation
void CMatrix3x3::MakeTransformQuaternion
(
	const CQuaternion& quat,
	const CVector3&    scale /*= CVector3::kOne*/
)
{
	// Efficiently precalculate some values from the quaternion
	TFloat32 xx = 2*quat.x;
	TFloat32 yy = 2*quat.y;
	TFloat32 zz = 2*quat.z;
	TFloat32 xy = xx*quat.y;
	TFloat32 yz = yy*quat.z;
	TFloat32 zx = zz*quat.x;
	TFloat32 wx = quat.w*xx;
	TFloat32 wy = quat.w*yy;
	TFloat32 wz = quat.w*zz;
	xx *= quat.x;
	yy *= quat.y;
	zz *= quat.z;

	// Fill upper 3x3 matrix, combining scaling with rotation values from the quaternion
	e00  = scale.x * (1 - yy - zz);
	e01  = scale.x * (xy + wz);
	e02  = scale.x * (zx - wy);

	e10  = scale.y * (xy - wz);
	e11  = scale.y * (1 - xx - zz);
	e12  = scale.y * (yz + wx);

	e20  = scale.z * (zx + wy);
	e21  = scale.z * (yz - wx);
	e22 = scale.z * (1 - xx - yy);
}

// Make matrix a transformation using angle/axis of rotation and optional scaling, with
// remaining elements taken from the identity matrix
// Matrix is built in this order: M = Scale*Rotation*Translation
void CMatrix3x3::MakeTransformAxisAngle
(
	const CVector3& axis,
	const TFloat32  fAngle,
	const CVector3& scale /*= CVector3::kOne*/
)
{
	// First build rotation matrix
	MakeRotation( axis, fAngle );

	// Scale matrix
	e00 *= scale.x;
	e01 *= scale.x;
	e02 *= scale.x;

	e10 *= scale.y;
	e11 *= scale.y;
	e12 *= scale.y;

	e20 *= scale.z;
	e21 *= scale.z;
	e22 *= scale.z;
}


// Decompose transformation into Euler angles of rotation (X, Y & Z angles of rotation around
// each axis) and scale. Optionally pass order of rotations. Pass NULL for any unneeded
// parameters. Assumes matrix is built in this order: M = Scale*Rotation
void CMatrix3x3::DecomposeTransformEuler
(
	CVector3*            pAngles,
	CVector3*            pScale,
	const ERotationOrder eRotOrder /*= kZXY*/
) const
{
	GEN_GUARD;

	// Calculate matrix scaling
	TFloat32 scaleX = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	TFloat32 scaleY = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	TFloat32 scaleZ = Sqrt( e20*e20 + e21*e21 + e22*e22 );

	// Get rotations if required
	if (pAngles)
	{
		// Calculate inverse scaling to extract rotational values only
		GEN_ASSERT( !IsZero(scaleX) && !IsZero(scaleY) && !IsZero(scaleZ), "Singular matrix" );
		TFloat32 invScaleX = 1.0f / scaleX;
		TFloat32 invScaleY = 1.0f / scaleY;
		TFloat32 invScaleZ = 1.0f / scaleZ;

		TFloat32 sX, cX, sY, cY, sZ, cZ;
		switch (eRotOrder)
		{
			case kZYX:
			{
				sY = e20 * invScaleX;
				cY = Sqrt( 1.0f - sY*sY );

				// If no gimbal lock...
				if (!IsZero( cY ))
				{
					TFloat32 invCY = 1.0f / cY;
					sZ = -e10 * invCY * invScaleY;
					cZ =  e00 * invCY * invScaleX;
					sX = -e21 * invCY * invScaleZ;
					cX =  e22 * invCY * invScaleZ;
				}
				else
				{
					// Gimbal lock - force Z angle to 0
					sZ = 0.0f;
					cZ = 1.0f;
					sX = e12 * invScaleY;
					cX = e11 * invScaleY;
				}
				break;
			}

			case kYZX:
			{
				sZ = -e10 * invScaleY;
				cZ = Sqrt( 1.0f - sZ*sZ );

				// If no gimbal lock...
				if (!IsZero( cZ ))
				{
					TFloat32 invCZ = 1.0f / cZ;
					sY = e20 * invCZ * invScaleZ;
					cY = e00 * invCZ * invScaleX;
					sX = e12 * invCZ * invScaleY;
					cX = e11 * invCZ * invScaleY;
				}
				else
				{
					// Gimbal lock - force Y angle to 0
					sY = 0.0f;
					cY = 1.0f;
					sX = -e21 * invScaleZ;
					cX =  e22 * invScaleZ;
				}
				break;
			}

			case kXZY:
			{
				sZ = e01 * invScaleX;
				cZ = Sqrt( 1.0f - sZ*sZ );

				// If no gimbal lock...
				if (!IsZero( cZ ))
				{
					TFloat32 invCZ = 1.0f / cZ;
					sX = -e21 * invCZ * invScaleZ;
					cX =  e11 * invCZ * invScaleY;
					sY = -e02 * invCZ * invScaleX;
					cY =  e00 * invCZ * invScaleX;
				}
				else
				{
					// Gimbal lock - force X angle to 0
					sX = 0.0f;
					cX = 1.0f;
					sY = e20 * invScaleZ;
					cY = e22 * invScaleZ;
				}
				break;
			}

			case kZXY:
			{
				sX = -e21 * invScaleZ;
				cX = Sqrt( 1.0f - sX*sX );

				// If no gimbal lock...
				if (!IsZero( cX ))
				{
					TFloat32 invCX = 1.0f / cX;
					sZ = e01 * invCX * invScaleX;
					cZ = e11 * invCX * invScaleY;
					sY = e20 * invCX * invScaleZ;
					cY = e22 * invCX * invScaleZ;
				}
				else
				{
					// Gimbal lock - force Z angle to 0
					sZ = 0.0f;
					cZ = 1.0f;
					sY = -e02 * invScaleX;
					cY =  e00 * invScaleX;
				}
				break;
			}

			case kYXZ:
			{
				sX = e12 * invScaleY;
				cX = Sqrt( 1.0f - sX*sX );

				// If no gimbal lock...
				if (!IsZero( cX ))
				{
					TFloat32 invCX = 1.0f / cX;
					sY = -e02 * invCX * invScaleX;
					cY =  e22 * invCX * invScaleZ;
					sZ = -e10 * invCX * invScaleY;
					cZ =  e11 * invCX * invScaleY;
				}
				else
				{
					// Gimbal lock - force Y angle to 0
					sY = 0.0f;
					cY = 1.0f;
					sZ = e01 * invScaleX;
					cZ = e00 * invScaleX;
				}
				break;
			}

			case kXYZ:
			{
				sY = -e02 * invScaleX;
				cY = Sqrt( 1.0f - sY*sY );

				// If no gimbal lock...
				if (!IsZero( cY ))
				{
					TFloat32 invCY = 1.0f / cY;
					sX = e12 * invCY * invScaleY;
					cX = e22 * invCY * invScaleZ;
					sZ = e01 * invCY * invScaleX;
					cZ = e00 * invCY * invScaleX;
				}
				else
				{
					// Gimbal lock - force X angle to 0
					sX = 0.0f;
					cX = 1.0f;
					sZ = -e10 * invScaleY;
					cZ =  e11 * invScaleY;
				}
				break;
			}

			default:
				GEN_ERROR( "Invalid parameter" );
		}

		pAngles->x = ATan( sX, cX );
		pAngles->y = ATan( sY, cY );
		pAngles->z = ATan( sZ, cZ );
	}

	// Return scale if required
	if (pScale)
	{
		pScale->x = scaleX;
		pScale->y = scaleY;
		pScale->z = scaleZ;
	}

	GEN_ENDGUARD;
}

// Decompose transformation into quaternion of rotation and scale. Pass NULL for any unneeded
// parameters. Assumes built in this order: M = Scale*Rotation
void CMatrix3x3::DecomposeTransformQuaternion
(
	CQuaternion* pQuat,
	CVector3*    pScale
) const
{
	// Calculate matrix scaling
	TFloat32 scaleX = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	TFloat32 scaleY = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	TFloat32 scaleZ = Sqrt( e20*e20 + e21*e21 + e22*e22 );

	// Return quaternion if required - similar to angle-axis process below
	if (pQuat)
	{
		// Calculate inverse scaling to extract rotational values only
		GEN_ASSERT( !IsZero(scaleX) && !IsZero(scaleY) && !IsZero(scaleZ), "Singular matrix" );
		TFloat32 invScaleX = 1.0f / scaleX;
		TFloat32 invScaleY = 1.0f / scaleY;
		TFloat32 invScaleZ = 1.0f / scaleZ;

		// Calculate trace of matrix (the sum of diagonal elements)
		TFloat32 diagX = e00 * invScaleX; // Remove scaling
		TFloat32 diagY = e11 * invScaleY;
		TFloat32 diagZ = e22 * invScaleZ;
		TFloat32 trace = diagX + diagY + diagZ;

		// Simple method if trace is positive
		if (trace > 0.0f)
		{
			// Derive quaternion from remaining elements
			TFloat32 cosAng = Sqrt( trace + 1.0f );
			pQuat->w = cosAng * 0.5f;
			TFloat32 invCosAng = 0.5f / cosAng;
			pQuat->x = (e12*invScaleY - e21*invScaleZ) * invCosAng;
			pQuat->y = (e20*invScaleZ - e02*invScaleX) * invCosAng;
			pQuat->z = (e01*invScaleX - e10*invScaleY) * invCosAng;
		}
		else
		{
			// Find largest x,y or z axis component by manipulating diagonal elts
			TFloat32 maxAxis, invMaxAxis;
			if (diagX > diagY)
			{
				if (diagX > diagZ)
				{
					maxAxis = Sqrt( diagX - diagY - diagZ + 1.0f );
					pQuat->x = 0.5f * maxAxis;
					invMaxAxis = 0.5f / maxAxis;
					pQuat->y = (e01*invScaleX + e10*invScaleY) * invMaxAxis;
					pQuat->z = (e20*invScaleZ + e02*invScaleX) * invMaxAxis;
					pQuat->w = (e12*invScaleY - e21*invScaleZ) * invMaxAxis;
				}
				else
				{
					maxAxis = Sqrt( diagZ - diagX - diagY + 1.0f );
					pQuat->z = 0.5f * maxAxis;
					invMaxAxis = 0.5f / maxAxis;
					pQuat->x = (e20*invScaleZ + e02*invScaleX) * invMaxAxis;
					pQuat->y = (e12*invScaleY + e21*invScaleZ) * invMaxAxis;
					pQuat->w = (e01*invScaleX - e10*invScaleY) * invMaxAxis;
				}
			}
			else if (diagY > diagZ)
			{
				maxAxis = Sqrt( diagY - diagZ - diagX + 1.0f );
				pQuat->y = 0.5f * maxAxis;
				invMaxAxis = 0.5f / maxAxis;
				pQuat->z = (e12*invScaleY + e21*invScaleZ) * invMaxAxis;
				pQuat->x = (e01*invScaleX + e10*invScaleY) * invMaxAxis;
				pQuat->w = (e20*invScaleZ - e02*invScaleX) * invMaxAxis;
			}
			else
			{
				maxAxis = Sqrt( diagZ - diagX - diagY + 1.0f );
				pQuat->z = 0.5f * maxAxis;
				invMaxAxis = 0.5f / maxAxis;
				pQuat->x = (e20*invScaleZ + e02*invScaleX) * invMaxAxis;
				pQuat->y = (e12*invScaleY + e21*invScaleZ) * invMaxAxis;
				pQuat->w = (e01*invScaleX - e10*invScaleY) * invMaxAxis;
			}
		}
	}

	// Return scale if required
	if (pScale)
	{
		pScale->x = scaleX;
		pScale->y = scaleY;
		pScale->z = scaleZ;
	}
}

// Decompose transformation into angle/axis of rotation (an axis and amount of rotation around
// that axis) and scale. Pass NULL for any unneeded parameters. Assumes matrix is built in this
// order: M = Scale*Rotation
void CMatrix3x3::DecomposeTransformAxisAngle
(
	CVector3* pAxis,
	TFloat32* pfAngle,
	CVector3* pScale
) const
{
	// Calculate matrix scaling
	TFloat32 scaleX = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	TFloat32 scaleY = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	TFloat32 scaleZ = Sqrt( e20*e20 + e21*e21 + e22*e22 );

	// Return angle/axis if required
	if (pAxis || pfAngle)
	{
		// Calculate inverse scaling to extract rotational values only
		GEN_ASSERT( !IsZero(scaleX) && !IsZero(scaleY) && !IsZero(scaleZ), "Singular matrix" );
		TFloat32 invScaleX = 1.0f / scaleX;
		TFloat32 invScaleY = 1.0f / scaleY;
		TFloat32 invScaleZ = 1.0f / scaleZ;

		// Calculate rotation using trace of matrix (the sum of diagonal elements) 
		TFloat32 diagX = e00 * invScaleX; // Remove scaling
		TFloat32 diagY = e11 * invScaleY;
		TFloat32 diagZ = e22 * invScaleZ;
		TFloat32 cosAngle = (diagX + diagY + diagZ - 1.0f) * 0.5f;
		TFloat32 angle = ACos( cosAngle );

		// Return angle if required
		if (pfAngle)
		{
			*pfAngle = angle;
		}

		// Get axis if required
		if (pAxis)
		{
			// Rotation of 0 gives free choice of axis
			if (IsZero( angle ))
			{
				*pAxis = CVector3::kZAxis;
			}

			// Otherwise if rotation is not 180 degrees...
			else if (!IsZero( angle - kfPi ))
			{
				pAxis->Set( e12 *invScaleY - e21 * invScaleZ, 
				            e20 *invScaleZ - e02 * invScaleX,
							e01 *invScaleX - e10 * invScaleY );
				pAxis->Normalise();
			}

			// Or if rotation is 180 degrees
			else
			{
				// Find largest x,y or z axis component by manipulating diagonal elts
				TFloat32 maxAxis, invMaxAxis;
				if (diagX > diagY)
				{
					if (diagX > diagZ)
					{
						maxAxis = Sqrt(diagX - diagY - diagZ + 1.0f);
						pAxis->x = 0.5f * maxAxis;
						// Use this component to calculate others using other matrix elts
						invMaxAxis = 1.0f / maxAxis;
						pAxis->y = e01 * invMaxAxis;
						pAxis->z = e02 * invMaxAxis;
					}
					else
					{
						maxAxis = Sqrt(diagZ - diagX - diagY + 1.0f);
						pAxis->z = 0.5f * maxAxis;
						invMaxAxis = 1.0f / maxAxis;
						pAxis->x = e02 * invMaxAxis;
						pAxis->y = e12 * invMaxAxis;
					}
				}
				else if (diagY > diagZ)
				{
					maxAxis = Sqrt(diagY - diagZ - diagX + 1.0f);
					pAxis->y = 0.5f * maxAxis;
					invMaxAxis = 1.0f / maxAxis;
					pAxis->x = e01 * invMaxAxis;
					pAxis->z = e12 * invMaxAxis;
				}
				else
				{
					maxAxis = Sqrt(diagZ - diagX - diagY + 1.0f);
					pAxis->z = 0.5f * maxAxis;
					invMaxAxis = 1.0f / maxAxis;
					pAxis->x = e02 * invMaxAxis;
					pAxis->y = e12 * invMaxAxis;
				}
			}
		}
	}

	// Get scale if required
	if (pScale)
	{
		pScale->x = scaleX;
		pScale->y = scaleY;
		pScale->z = scaleZ;
	}
}


/*-----------------------------------------------------------------------------------------
	Creation and Decomposition of 2D Affine Matrices
-----------------------------------------------------------------------------------------*/

// Make 2D affine transformation from position, and optional rotation angle & scaling, with 
// remaining elements taken from the identity matrix. Matrix is effectively built in this
// order: M = Scale*Rotation*Translation
void CMatrix3x3::MakeAffine2D
(
	const CVector2& position,
	const TFloat32  fAngle /*= 0.0f*/,
	const CVector2& scale /*= CVector2::kOne*/
)
{
	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	e00 = c * scale.x;
	e01 = s * scale.x;
	e02 = 0.0f;

	e10 = -s * scale.y;
	e11 = c * scale.y;
	e12 = 0.0f;

	e20 = position.x;
	e21 = position.y;
	e22 = 1.0f;
}

// Decompose 2D affine transformation into position, angle of rotation & scale. Pass NULL for
// unneeded parameters. Assumes matrix is built in this order: M = Scale*Rotation*Translation
void CMatrix3x3::DecomposeAffine2D
(
	CVector2* pPosition,
	TFloat32* pfAngle,
	CVector2* pScale
) const
{
	GEN_GUARD;

	// Get position if required
	if (pPosition)
	{
		pPosition->x = e20;
		pPosition->y = e21;
	}

	// Calculate matrix scaling
	TFloat32 scaleX = Sqrt( e00*e00 + e01*e01 );
	TFloat32 scaleY = Sqrt( e10*e10 + e11*e11 );

	// Get rotation angle if required
	if (pfAngle)
	{
		// Calculate inverse scaling to extract rotational values only
		GEN_ASSERT( !IsZero(scaleX) && !IsZero(scaleY), "Singular matrix" );
		TFloat32 invScaleX = 1.0f / scaleX;

		*pfAngle = ATan( e01 * invScaleX, e00 * invScaleX );
	}

	// Return scale if required
	if (pScale)
	{
		pScale->x = scaleX;
		pScale->y = scaleY;
	}

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Manipulation of Transformation Matrices
-----------------------------------------------------------------------------------------*/

// Get the X, Y & Z scaling of a transformation matrix
CVector3 CMatrix3x3::GetScale() const
{
	CVector3 scale;
	scale.x = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	scale.y = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	scale.z = Sqrt( e20*e20 + e21*e21 + e22*e22 );
	return scale;
}


// Set the X scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (e.g. rotation)
void CMatrix3x3::SetScaleX( const TFloat32 x )
{
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = x * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
		e02 *= rescale;
	}
}

// Set the Y scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (e.g. rotation)
void CMatrix3x3::SetScaleY( const TFloat32 y )
{
	TFloat32 scaleSq = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = y * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
		e12 *= rescale;
	}
}

// Set the Z scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (e.g. rotation)
void CMatrix3x3::SetScaleZ( const TFloat32 z )
{
	TFloat32 scaleSq = Sqrt( e20*e20 + e21*e21 + e22*e22 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = z * InvSqrt( scaleSq );
		e20 *= rescale;
		e21 *= rescale;
		e22 *= rescale;
	}
}

// Set the X, Y & Z scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (e.g. rotation)
void CMatrix3x3::SetScale( const CVector3& scale )
{
	TFloat32 rescale;
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = scale.x * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
		e02 *= rescale;
	}
	
	scaleSq = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = scale.y * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
		e12 *= rescale;
	}

	scaleSq = Sqrt( e20*e20 + e21*e21 + e22*e22 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = scale.z * InvSqrt( scaleSq );
		e20 *= rescale;
		e21 *= rescale;
		e22 *= rescale;
	}
}

// Set a uniform scaling for a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (rotation)
void CMatrix3x3::SetScale( const TFloat32 fScale )
{
	TFloat32 rescale;
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = fScale * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
		e02 *= rescale;
	}
	
	scaleSq = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = fScale * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
		e12 *= rescale;
	}

	scaleSq = Sqrt( e20*e20 + e21*e21 + e22*e22 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = fScale * InvSqrt( scaleSq );
		e20 *= rescale;
		e21 *= rescale;
		e22 *= rescale;
	}
}


/*-----------------------------------------------------------------------------------------
	Manipulation of 2D Affine Matrices
-----------------------------------------------------------------------------------------*/

// Get the X & Y scaling of a 2D affine transformation matrix
CVector2 CMatrix3x3::GetScale2D() const
{
	CVector2 scale;
	scale.x = Sqrt( e00*e00 + e01*e01 );
	scale.y = Sqrt( e10*e10 + e11*e11 );
	return scale;
}


// Set the X scaling of a 2D affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix3x3::SetScaleX2D( const TFloat32 x )
{
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = x * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
	}
}

// Set the Y scaling of a 2D affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix3x3::SetScaleY2D( const TFloat32 y )
{
	TFloat32 scaleSq = Sqrt( e10*e10 + e11*e11 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = y * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
	}
}

// Set the X & Y scaling of a 2D affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix3x3::SetScale2D( const CVector2& scale )
{
	TFloat32 rescale;
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = scale.x * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
	}
	
	scaleSq = Sqrt( e10*e10 + e11*e11 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = scale.y * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
	}
}

// Set a uniform scaling for a 2D affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix3x3::SetScale2D( const TFloat32 fScale )
{
	TFloat32 rescale;
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = fScale * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
	}
	
	scaleSq = Sqrt( e10*e10 + e11*e11 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		rescale = fScale * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
	}
}


/*-----------------------------------------------------------------------------------------
	Comparisons
-----------------------------------------------------------------------------------------*/

// Test if matrix is the identity
// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
bool CMatrix3x3::IsIdentity() const
{
	return IsZero( e00 - 1.0f ) && IsZero( e11 - 1.0f ) && IsZero( e22 - 1.0f ) && 
	       IsZero( e01 ) && IsZero( e02 ) && 
	       IsZero( e10 ) && IsZero( e12 ) && 
	       IsZero( e20 ) && IsZero( e21 );
}


/*-----------------------------------------------------------------------------------------
	Othogonality
-----------------------------------------------------------------------------------------*/

// See extensive notes in header file

// Test if matrix has orthogonal rows, i.e. if the three rows are vectors at right angles to
// each other. Will also be orthonormal if it contains no scaling
bool CMatrix3x3::IsOrthogonal() const
{
	// Check dot product of each pair of rows is zero
	return IsZero( e00*e10 + e01*e11 + e02*e12 ) &&
	       IsZero( e10*e20 + e11*e21 + e12*e22 ) &&
	       IsZero( e20*e00 + e21*e01 + e22*e02 );
}

// Test if matrix has orthonormal rows, i.e. if the three rows are *normals* at right angles to
// each other. Scaled matrices cannot be orthonormal (can be orthogonal)
bool CMatrix3x3::IsOrthonormal() const
{
	// Check each row is length 1 and dot product of each pair of rows is zero
	return IsZero( e00*e00 + e01*e01 + e02*e02 - 1.0f ) &&
	       IsZero( e10*e10 + e11*e11 + e12*e12 - 1.0f ) &&
	       IsZero( e20*e20 + e21*e21 + e22*e22 - 1.0f ) &&
	       IsZero( e00*e10 + e01*e11 + e02*e12 ) &&
	       IsZero( e10*e20 + e11*e21 + e12*e22 ) &&
	       IsZero( e20*e00 + e21*e01 + e22*e02 );
}

// Orthogonalise the rows/columns of the matrix. Generally used to "correct" matrices that
// become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
void CMatrix3x3::Orthogonalise( const CVector3& scale /*= CVector3::kOne*/ )
{
	GEN_GUARD;

	// Normalise first vector (x-axis)
	TFloat32 origScaleX = e00*e00 + e01*e01 + e02*e02;
	GEN_ASSERT( !IsZero(origScaleX), "Singular matrix" );
	TFloat32 invOrigScaleX = InvSqrt( origScaleX );
	e00 *= invOrigScaleX;
	e01 *= invOrigScaleX;
	e02 *= invOrigScaleX;

	// Orthogonalise second vector (y-axis) with first
	TFloat32 proj10 = e10*e00 + e11*e01 + e12*e02;
	e10 -= proj10 * e00;
	e11 -= proj10 * e01;
	e12 -= proj10 * e02;

	// Normalise second vector
	TFloat32 origScaleY = e10*e10 + e11*e11 + e12*e12;
	GEN_ASSERT( !IsZero(origScaleY), "Singular matrix" );
	TFloat32 invOrigScaleY = InvSqrt( origScaleY );
	e10 *= invOrigScaleY;
	e11 *= invOrigScaleY;
	e12 *= invOrigScaleY;

	// Get third vector from cross product of first two (result is already normalised)
	e20 = e01*e12 - e02*e11;
	e21 = e02*e10 - e00*e12;
	e22 = e00*e11 - e01*e10;
	
	// Rescale each vector
	e00 *= scale.x;
	e01 *= scale.x;
	e02 *= scale.x;

	e10 *= scale.y;
	e11 *= scale.y;
	e12 *= scale.y;

	e20 *= scale.z;
	e21 *= scale.z;
	e22 *= scale.z;

	GEN_ENDGUARD;
}


// Return a copy of given matrix with orthogonalised rows/columns. Generally used to "correct"
// matrices that become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
CMatrix3x3 Orthogonalise
(
	const CMatrix3x3& m,
	const CVector3& scale /*= CVector3::kOne*/
)
{
	GEN_GUARD;

	CMatrix3x3 mOut;

	// Normalise first vector (x-axis)
	TFloat32 origScaleX = m.e00*m.e00 + m.e01*m.e01 + m.e02*m.e02;
	GEN_ASSERT( !IsZero(origScaleX), "Singular matrix" );
	TFloat32 invOrigScaleX = InvSqrt( origScaleX );
	mOut.e00 = m.e00 * invOrigScaleX;
	mOut.e01 = m.e01 * invOrigScaleX;
	mOut.e02 = m.e02 * invOrigScaleX;

	// Orthogonalise second vector (y-axis) with first
	TFloat32 proj10 = m.e10*mOut.e00 + m.e11*mOut.e01 + m.e12*mOut.e02;
	mOut.e10 = m.e10 - proj10 * mOut.e00;
	mOut.e11 = m.e11 - proj10 * mOut.e01;
	mOut.e12 = m.e12 - proj10 * mOut.e02;

	// Normalise second vector
	TFloat32 origScaleY = mOut.e10*mOut.e10 + mOut.e11*mOut.e11 + mOut.e12*mOut.e12;
	GEN_ASSERT( !IsZero(origScaleY), "Singular matrix" );
	TFloat32 invOrigScaleY = InvSqrt( origScaleY );
	mOut.e10 *= invOrigScaleY;
	mOut.e11 *= invOrigScaleY;
	mOut.e12 *= invOrigScaleY;

	// Get third vector from cross product of first two (result is already normalised)
	mOut.e20 = mOut.e01*mOut.e12 - mOut.e02*mOut.e11;
	mOut.e21 = mOut.e02*mOut.e10 - mOut.e00*mOut.e12;
	mOut.e22 = mOut.e00*mOut.e11 - mOut.e01*mOut.e10;
	
	// Rescale each vector
	mOut.e00 *= scale.x;
	mOut.e01 *= scale.x;
	mOut.e02 *= scale.x;

	mOut.e10 *= scale.y;
	mOut.e11 *= scale.y;
	mOut.e12 *= scale.y;

	mOut.e20 *= scale.z;
	mOut.e21 *= scale.z;
	mOut.e22 *= scale.z;

	return mOut;

	GEN_ENDGUARD;
}


// Test if upper-left 2x2 matrix has orthogonal rows, i.e. if the two rows are vectors
// at right angles to each other. Will also be orthonormal if it contains no scaling
bool CMatrix3x3::IsOrthogonal2x2() const
{
	// Check dot product of each pair of rows is zero
	return IsZero( e00*e10 + e01*e11) && IsZero( e10*e20 + e11*e21);
}

// Test if upper-left 2x2 matrix has orthonormal rows, i.e. if the two rows are *normals*
// at right angles to each other. Scaled matrices cannot be orthonormal (can be orthogonal)
bool CMatrix3x3::IsOrthonormal2x2() const
{
	// Check each row is length 1 and dot product of each pair of rows is zero
	return IsZero( e00*e00 + e01*e01 - 1.0f ) && IsZero( e10*e10 + e11*e11 - 1.0f ) &&
	       IsZero( e00*e10 + e01*e11 ) && IsZero( e10*e20 + e11*e21 );
}

// Orthogonalise the rows/columns of the upper-left 2x2 matrix. Generally used to "correct"
// matrices that become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
void CMatrix3x3::Orthogonalise2x2(	const CVector2& scale /*= CVector2::kOne*/ )
{
	GEN_GUARD;

	// Normalise first vector
	TFloat32 origScaleX = e00*e00 + e01*e01;
	GEN_ASSERT( !IsZero(origScaleX), "Singular matrix" );
	TFloat32 invOrigScaleX = InvSqrt( origScaleX );
	e00 *= invOrigScaleX;
	e01 *= invOrigScaleX;

	// Second vector is simply perpendicular to first, only need to select cw or ccw direction
	TFloat32 ccwPerpDot = e00*e11 - e01*e10;
	if (ccwPerpDot >= 0.0f)
	{
		e11 = e00;
		e10 = -e01;
	}
	else
	{
		e11 = -e00;
		e10 = e01;
	}

	// Rescale each vector
	e00 *= scale.x;
	e01 *= scale.x;

	e10 *= scale.y;
	e11 *= scale.y;

	GEN_ENDGUARD;
}

// Return a copy of given matrix with orthogonalised rows/columns in the upper-left 2x2 matrix.
// Generally used to "correct" matrices that become non-orthogonal after repeated calculations /
// floating point rounding. May pass scaling for resultant rows - default is 1, leading to
// "orthonormalisation"
CMatrix3x3 Orthogonalise2x2
(
	const CMatrix3x3& m,
	const CVector2&   scale /*= CVector2::kOne*/
)
{
	GEN_GUARD;

	CMatrix3x3 mOut;

	// Normalise first vector
	TFloat32 origScaleX = m.e00*m.e00 + m.e01*m.e01;
	GEN_ASSERT( !IsZero(origScaleX), "Singular matrix" );
	TFloat32 invOrigScaleX = InvSqrt( origScaleX );
	mOut.e00 = m.e00 * invOrigScaleX;
	mOut.e01 = m.e01 * invOrigScaleX;

	// Second vector is simply perpendicular to first, only need to select cw or ccw direction
	TFloat32 ccwPerpDot = mOut.e00*m.e11 - mOut.e01*m.e10;
	if (ccwPerpDot >= 0.0f)
	{
		mOut.e11 = mOut.e00;
		mOut.e10 = -mOut.e01;
	}
	else
	{
		mOut.e11 = -mOut.e00;
		mOut.e10 = mOut.e01;
	}

	// Rescale each vector
	mOut.e00 *= scale.x;
	mOut.e01 *= scale.x;

	mOut.e10 *= scale.y;
	mOut.e11 *= scale.y;

	return mOut;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Inverse related
-----------------------------------------------------------------------------------------*/

// Set this matrix to its transpose (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
void CMatrix3x3::Transpose()
{
	TFloat32 t;

	t   = e01;
	e01 = e10;
	e10 = t;

	t   = e02;
	e02 = e20;
	e20 = t;

	t   = e12;
	e12 = e21;
	e21 = t;
}

// Return the transpose of given matrix (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
CMatrix3x3 Transpose( const CMatrix3x3& m )
{
	CMatrix3x3 transMat;

	transMat.e00 = m.e00;
	transMat.e01 = m.e10;
	transMat.e02 = m.e20;

	transMat.e10 = m.e01;
	transMat.e11 = m.e11;
	transMat.e12 = m.e21;

	transMat.e20 = m.e02;
	transMat.e21 = m.e12;
	transMat.e22 = m.e22;

	return transMat;
}


// Set this matrix to its inverse assuming it has orthogonal rows, i.e. it is a transformation
// matrix with no shear. Most efficient inverse for transformations with rotation & scale only
void CMatrix3x3::InvertRotScale()
{
	GEN_GUARD;

	// Get X, Y & Z scaling (squared)
	TFloat32 scaleSqX = e00*e00 + e01*e01 + e02*e02;
	TFloat32 scaleSqY = e10*e10 + e11*e11 + e12*e12;
	TFloat32 scaleSqZ = e20*e20 + e21*e21 + e22*e22;
	
	// Calculate scaling inverse
	GEN_ASSERT( !IsZero(scaleSqX) && !IsZero(scaleSqY) && !IsZero(scaleSqZ), "Singular matrix" );
	TFloat32 invScaleX = 1.0f / scaleSqX;
	TFloat32 invScaleY = 1.0f / scaleSqY;
	TFloat32 invScaleZ = 1.0f / scaleSqZ;

	// Inverse is just the transpose with scaling inverse factored in
	TFloat32 t;
	e00 *= invScaleX;
	e11 *= invScaleY;
	e22 *= invScaleZ;

	t   = e01;
	e01 = e10 * invScaleY;
	e10 = t * invScaleX;

	t   = e02;
	e02 = e20 * invScaleZ;
	e20 = t  * invScaleX;

	t   = e12;
	e12 = e21 * invScaleZ;
	e21 = t * invScaleY;

	GEN_ENDGUARD;
}

// Return the inverse of given matrix assuming it has orthogonal rows, i.e. it is a transformation
// matrix with no shear. Most efficient inverse for transformations with rotation & scale only
CMatrix3x3 InverseRotScale( const CMatrix3x3& m )
{
	GEN_GUARD;

	CMatrix3x3 mOut;

	// Get X, Y & Z scaling (squared)
	TFloat32 scaleSqX = m.e00*m.e00 + m.e01*m.e01 + m.e02*m.e02;
	TFloat32 scaleSqY = m.e10*m.e10 + m.e11*m.e11 + m.e12*m.e12;
	TFloat32 scaleSqZ = m.e20*m.e20 + m.e21*m.e21 + m.e22*m.e22;
	
	// Calculate scaling inverse
	GEN_ASSERT( !IsZero(scaleSqX) && !IsZero(scaleSqY) && !IsZero(scaleSqZ), "Singular matrix" );
	TFloat32 invScaleX = 1.0f / scaleSqX;
	TFloat32 invScaleY = 1.0f / scaleSqY;
	TFloat32 invScaleZ = 1.0f / scaleSqZ;

	// Inverse is just the transpose with scaling inverse factored in
	mOut.e00 = m.e00 * invScaleX;
	mOut.e01 = m.e10 * invScaleY;
	mOut.e02 = m.e20 * invScaleZ;
	
	mOut.e10 = m.e01 * invScaleX;
	mOut.e11 = m.e11 * invScaleY;
	mOut.e12 = m.e21 * invScaleZ;

	mOut.e20 = m.e02 * invScaleX;
	mOut.e21 = m.e12 * invScaleY;
	mOut.e22 = m.e22 * invScaleZ;

	return mOut;

	GEN_ENDGUARD;
}


// Set this matrix to its inverse assuming that it is a 2D affine matrix
void CMatrix3x3::InvertAffine2D()
{
	GEN_GUARD;

	// Calculate determinant of upper left 2x2
	TFloat32 det = e00*e11 - e01*e10;
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Calculate inverse of upper left 2x2
	TFloat32 invDet = 1.0f / det;
	TFloat32 t;
	t   = invDet * e00;
	e00 = invDet * e11;
	e11 = t;

	e01 *= invDet;
	e10 *= invDet;

	// Transform negative translation by inverted 3x3 to get inverse
	e20 = -e20*e00 - e21*e10;
	e21 = -e20*e01 - e21*e11;

	GEN_ENDGUARD;
}

// Return the inverse of given matrix assuming only that it is a 2D affine matrix
CMatrix3x3 InverseAffine2D( const CMatrix3x3& m )
{
	GEN_GUARD;

	CMatrix3x3 mOut;

	// Calculate determinant of upper left 2x2
	TFloat32 det = m.e00*m.e11 - m.e01*m.e10;
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Calculate inverse of upper left 2x2
	TFloat32 invDet = 1.0f / det;
	mOut.e00 = invDet * m.e11;
	mOut.e01 = invDet * m.e01;
	mOut.e02 = 0.0f;

	mOut.e10 = invDet * m.e10;
	mOut.e11 = invDet * m.e00;
	mOut.e12 = 0.0f;

	// Transform negative translation by inverted 3x3 to get inverse
	mOut.e20 = -m.e20*mOut.e00 - m.e21*mOut.e10;
	mOut.e21 = -m.e20*mOut.e01 - m.e21*mOut.e11;
	mOut.e22 = 1.0f;

	return mOut;

	GEN_ENDGUARD;
}


// Set this matrix to its inverse. Most general, least efficient inverse function
void CMatrix3x3::Invert()
{
	*this = Inverse( *this ); // TODO: Just use non-member version
}

// Return the inverse of given matrix. Most general, least efficient inverse function
CMatrix3x3 Inverse( const CMatrix3x3& m )
{
	GEN_GUARD;

	CMatrix3x3 mOut;

	// Calculate determinant of upper left 3x3
	TFloat32 det0 = m.e11*m.e22 - m.e21*m.e12;
	TFloat32 det1 = m.e12*m.e20 - m.e22*m.e10;
	TFloat32 det2 = m.e10*m.e21 - m.e20*m.e11;
	TFloat32 det = m.e00*det0 +m.e01*det1 + m.e02*det2;
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Calculate inverse of upper left 3x3
	TFloat32 invDet = 1.0f / det;
	mOut.e00 = invDet * det0;
	mOut.e10 = invDet * det1;
	mOut.e20 = invDet * det2;
		  
	mOut.e01 = invDet * (m.e21*m.e02 - m.e01*m.e22);
	mOut.e11 = invDet * (m.e22*m.e00 - m.e02*m.e20);
	mOut.e21 = invDet * (m.e20*m.e01 - m.e00*m.e21);
		  
	mOut.e02 = invDet * (m.e01*m.e12 - m.e11*m.e02);
	mOut.e12 = invDet * (m.e02*m.e10 - m.e12*m.e00);
	mOut.e22 = invDet * (m.e00*m.e11 - m.e10*m.e01);

	return mOut;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Transformation Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix the identity matrix
void CMatrix3x3::MakeIdentity()
{
	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f; 

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}


// Make this matrix an X-axis rotation by the given angle (radians)
void CMatrix3x3::MakeRotationX( const TFloat32 x )
{
	TFloat32 sX, cX;
	SinCos( x, &sX, &cX );

	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = cX;
	e12 = sX;

	e20 = 0.0f;
	e21 = -sX;
	e22 = cX;
}

// Make this matrix a Y-axis rotation by the given angle (radians)
void CMatrix3x3::MakeRotationY( const TFloat32 y )
{
	TFloat32 sY, cY;
	SinCos( y, &sY, &cY );

	e00 = cY;
	e01 = 0.0f;
	e02 = -sY;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;

	e20 = sY;
	e21 = 0.0f;
	e22 = cY;
}

// Make this matrix a Z-axis rotation by the given angle (radians)
void CMatrix3x3::MakeRotationZ( const TFloat32 z )
{
	TFloat32 sZ, cZ;
	SinCos( z, &sZ, &cZ );

	e00 = cZ;
	e01 = sZ;
	e02 = 0.0f;

	e10 = -sZ;
	e11 = cZ;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}

// Make a matrix that is a combined rotation around the X, Y & Z axes by the given angles
// (radians), applied in the order specified
void CMatrix3x3::MakeRotation
( 
	const CVector3       angles,
	const ERotationOrder eRotOrder /*= kZXY*/
)
{
	GEN_GUARD;

	TFloat32 sX, cX, sY, cY, sZ, cZ;
	SinCos( angles.x, &sX, &cX );
	SinCos( angles.y, &sY, &cY );
	SinCos( angles.z, &sZ, &cZ );

	switch (eRotOrder)
	{
		case kZYX:
		{
			e00 = cZ * cY;
			e01 = sZ * cX + cZ * sY * sX;
			e02 = sZ * sX + -cZ * sY * cX;

			e10 = -sZ * cY;
			e11 = cZ * cX + -sZ * sY * sX;
			e12 = cZ * sX + sZ * sY * cX;

			e20 = sY;
			e21 = -cY * sX;
			e22 = cY * cX;
			break;
		}
		case kYZX:
		{
			e00 = cY * cZ;
			e01 = cY * sZ * cX + sY * sX;
			e02 = cY * sZ * sX + -sY * cX;

			e10 = -sZ;
			e11 = cZ * cX;
			e12 = cZ * sX;

			e20 = sY * cZ;
			e21 = sY * sZ * cX + -cY * sX;
			e22 = sY * sZ * sX + cY * cX;
			break;
		}
		case kXZY:
		{
			e00 = cZ * cY;
			e01 = sZ;
			e02 = -cZ * sY;

			e10 = -cX * sZ * cY + sX * sY;
			e11 = cX * cZ;
			e12 = cX * sZ * sY + sX * cY;

			e20 = sX * sZ * cY + cX * sY;
			e21 = -sX * cZ;
			e22 = -sX * sZ * sY + cX * cY;
			break;
		}
		case kZXY:
		{
			e00 = cZ * cY + sZ * sX * sY;
			e01 = sZ * cX;
			e02 = -cZ * sY + sZ * sX * cY;

			e10 = -sZ * cY + cZ * sX * sY;
			e11 = cZ * cX;
			e12 = sZ * sY + cZ * sX * cY;

			e20 = cX * sY;
			e21 = -sX;
			e22 = cX * cY;
			break;
		}
		case kYXZ:
		{
			e00 = cY * cZ + -sY * sX * sZ;
			e01 = cY * sZ + sY * sX * cZ;
			e02 = -sY * cX;

			e10 = -cX * sZ;
			e11 = cX * cZ;
			e12 = sX;

			e20 = sY * cZ + cY * sX * sZ;
			e21 = sY * sZ + -cY * sX * cZ;
			e22 = cY * cX;
			break;
		}
		case kXYZ:
		{
			e00 = cY * cZ;
			e01 = cY * sZ;
			e02 = -sY;

			e10 = sX * sY * cZ + -cX * sZ;
			e11 = sX * sY * sZ + cX * cZ;
			e12 = sX * cY;

			e20 = cX * sY * cZ + sX * sZ;
			e21 = cX * sY * sZ + -sX * cZ;
			e22 = cX * cY;
			break;
		}

		default:
			GEN_ERROR( "Invalid parameter" );
	}

	GEN_ENDGUARD;
}

// Make this matrix a rotation around the given axis by the given angle (radians)
void CMatrix3x3::MakeRotation
(
	const CVector3& axis,
	const TFloat32  fAngle
)
{
	GEN_GUARD;

	TFloat32 s, c;
	SinCos( fAngle, &s, &c );
	TFloat32 t = 1.0f - c;

	CVector3 axisNorm = Normalise( axis );
	GEN_ASSERT( !axisNorm.IsZero(), "Zero length axis" );

	TFloat32 sx = s * axisNorm.x;
	TFloat32 sy = s * axisNorm.y;
	TFloat32 sz = s * axisNorm.z;
	TFloat32 tx = t * axisNorm.x;  
	TFloat32 ty = t * axisNorm.y;
	TFloat32 tz = t * axisNorm.z;
	TFloat32 txy = ty * axisNorm.x;
	TFloat32 tyz = tz * axisNorm.y;
	TFloat32 tzx = tx * axisNorm.z;

	e00 = tx * axisNorm.x + c;
	e01 = txy + sz;
	e02 = tzx - sy;

	e10 = txy - sz; 
	e11 = ty * axisNorm.y + c;
	e12 = tyz + sx;

	e20 = tzx + sy;
	e21 = tyz - sx;
	e22 = tz * axisNorm.z + c;

	GEN_ENDGUARD;
}

// Make this matrix a scaling in X, Y and Z by the values provided in the given vector
void CMatrix3x3::MakeScaling( const CVector3& scale )
{
	e00 = scale.x;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = scale.y;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = scale.z;
}

// Make this matrix a uniform scaling by the given amount
void CMatrix3x3::MakeScaling( const TFloat32 fScale )
{
	e00 = fScale;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = fScale;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = fScale;
}


/*-----------------------------------------------------------------------------------------
	Transformation Matrices - non-member functions
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value). Can be used
// as temporaries in calculations, e.g.
//     CMatrix3x3 m = MatrixScaling( 3.0f ) * MatrixRotationX( ToRadians(45.0f) );

// Return an identity matrix - non-member function
CMatrix3x3 Matrix3x3Identity()
{
	CMatrix3x3 m;

	m.e00 = 1.0f;
	m.e01 = 0.0f;
	m.e02 = 0.0f; 

	m.e10 = 0.0f;
	m.e11 = 1.0f;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;

	return m;
}


// Return an X-axis rotation matrix of the given angle - non-member function
CMatrix3x3 Matrix3x3RotationX( const TFloat32 x )
{
	CMatrix3x3 m;

	TFloat32 sX, cX;
	SinCos( x, &sX, &cX );

	m.e00 = 1.0f;
	m.e01 = 0.0f;
	m.e02 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = cX;
	m.e12 = sX;

	m.e20 = 0.0f;
	m.e21 = -sX;
	m.e22 = cX;

	return m;
}

// Return a Y-axis rotation matrix of the given angle - non-member function
CMatrix3x3 Matrix3x3RotationY( const TFloat32 y )
{
	CMatrix3x3 m;

	TFloat32 sY, cY;
	SinCos( y, &sY, &cY );

	m.e00 = cY;
	m.e01 = 0.0f;
	m.e02 = -sY;

	m.e10 = 0.0f;
	m.e11 = 1.0f;
	m.e12 = 0.0f;

	m.e20 = sY;
	m.e21 = 0.0f;
	m.e22 = cY;

	return m;
}

// Return a Z-axis rotation matrix of the given angle - non-member function
CMatrix3x3 Matrix3x3RotationZ( const TFloat32 z )
{
	CMatrix3x3 m;

	TFloat32 sZ, cZ;
	SinCos( z, &sZ, &cZ );

	m.e00 = cZ;
	m.e01 = sZ;
	m.e02 = 0.0f;

	m.e10 = -sZ;
	m.e11 = cZ;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;

	return m;
}

// Make a matrix that is a combined rotation around each axis of the given angles (radians),
// applied in the order specified - non-member function
CMatrix3x3 Matrix3x3Rotation
( 
	const CVector3       angles,
	const ERotationOrder eRotOrder /*= kZXY*/
)
{
	GEN_GUARD;

	CMatrix3x3 m;

	TFloat32 sX, cX, sY, cY, sZ, cZ;
	SinCos( angles.x, &sX, &cX );
	SinCos( angles.y, &sY, &cY );
	SinCos( angles.z, &sZ, &cZ );

	switch (eRotOrder)
	{
		case kZYX:
		{
			m.e00 = cZ * cY;
			m.e01 = sZ * cX + cZ * sY * sX;
			m.e02 = sZ * sX + -cZ * sY * cX;

			m.e10 = -sZ * cY;
			m.e11 = cZ * cX + -sZ * sY * sX;
			m.e12 = cZ * sX + sZ * sY * cX;

			m.e20 = sY;
			m.e21 = -cY * sX;
			m.e22 = cY * cX;
			break;
		}
		case kYZX:
		{
			m.e00 = cY * cZ;
			m.e01 = cY * sZ * cX + sY * sX;
			m.e02 = cY * sZ * sX + -sY * cX;

			m.e10 = -sZ;
			m.e11 = cZ * cX;
			m.e12 = cZ * sX;

			m.e20 = sY * cZ;
			m.e21 = sY * sZ * cX + -cY * sX;
			m.e22 = sY * sZ * sX + cY * cX;
			break;
		}
		case kXZY:
		{
			m.e00 = cZ * cY;
			m.e01 = sZ;
			m.e02 = -cZ * sY;

			m.e10 = -cX * sZ * cY + sX * sY;
			m.e11 = cX * cZ;
			m.e12 = cX * sZ * sY + sX * cY;

			m.e20 = sX * sZ * cY + cX * sY;
			m.e21 = -sX * cZ;
			m.e22 = -sX * sZ * sY + cX * cY;
			break;
		}
		case kZXY:
		{
			m.e00 = cZ * cY + sZ * sX * sY;
			m.e01 = sZ * cX;
			m.e02 = -cZ * sY + sZ * sX * cY;

			m.e10 = -sZ * cY + cZ * sX * sY;
			m.e11 = cZ * cX;
			m.e12 = sZ * sY + cZ * sX * cY;

			m.e20 = cX * sY;
			m.e21 = -sX;
			m.e22 = cX * cY;
			break;
		}
		case kYXZ:
		{
			m.e00 = cY * cZ + -sY * sX * sZ;
			m.e01 = cY * sZ + sY * sX * cZ;
			m.e02 = -sY * cX;

			m.e10 = -cX * sZ;
			m.e11 = cX * cZ;
			m.e12 = sX;

			m.e20 = sY * cZ + cY * sX * sZ;
			m.e21 = sY * sZ + -cY * sX * cZ;
			m.e22 = cY * cX;
			break;
		}
		case kXYZ:
		{
			m.e00 = cY * cZ;
			m.e01 = cY * sZ;
			m.e02 = -sY;

			m.e10 = sX * sY * cZ + -cX * sZ;
			m.e11 = sX * sY * sZ + cX * cZ;
			m.e12 = sX * cY;

			m.e20 = cX * sY * cZ + sX * sZ;
			m.e21 = cX * sY * sZ + -sX * cZ;
			m.e22 = cX * cY;
			break;
		}

		default:
			GEN_ERROR( "Invalid parameter" );
	}

	return m;

	GEN_ENDGUARD;
}

// Return a matrix that is a rotation around the given axis of the given angle (radians)
// Non-member function
CMatrix3x3 Matrix3x3Rotation
(
	const CVector3& axis,
	const TFloat32  fAngle
)
{
	GEN_GUARD;

	CMatrix3x3 m;

	TFloat32 s, c;
	SinCos( fAngle, &s, &c );
	TFloat32 t = 1.0f - c;

	CVector3 axisNorm = Normalise( axis );
	GEN_ASSERT( !axisNorm.IsZero(), "Zero length axis" );

	TFloat32 sx = s * axisNorm.x;
	TFloat32 sy = s * axisNorm.y;
	TFloat32 sz = s * axisNorm.z;
	TFloat32 tx = t * axisNorm.x;  
	TFloat32 ty = t * axisNorm.y;
	TFloat32 tz = t * axisNorm.z;
	TFloat32 txy = ty * axisNorm.x;
	TFloat32 tyz = tz * axisNorm.y;
	TFloat32 tzx = tx * axisNorm.z;

	m.e00 = tx * axisNorm.x + c;
	m.e01 = txy + sz;
	m.e02 = tzx - sy;

	m.e10 = txy - sz; 
	m.e11 = ty * axisNorm.y + c;
	m.e12 = tyz + sx;

	m.e20 = tzx + sy;
	m.e21 = tyz - sx;
	m.e22 = tz * axisNorm.z + c;

	return m;

	GEN_ENDGUARD;
}


// Return a matrix that is a scaling in X,Y and Z of the values provided in the given vector
// Non-member function
CMatrix3x3 Matrix3x3Scaling( const CVector3& scale )
{
	CMatrix3x3 m;

	m.e00 = scale.x;
	m.e01 = 0.0f;
	m.e02 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = scale.y;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = scale.z;

	return m;
}

// Return a matrix that is a uniform scaling of the given amount - non-member function
CMatrix3x3 Matrix3x3Scaling( const TFloat32 fScale )
{
	CMatrix3x3 m;

	m.e00 = fScale;
	m.e01 = 0.0f;
	m.e02 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = fScale;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = fScale;

	return m;
}


/*-----------------------------------------------------------------------------------------
	2D Affine Transformation Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix a 2D affine translation by the given vector
void CMatrix3x3::MakeTranslation2D( const CVector2& translate )
{
	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;

	e20 = translate.x;
	e21 = translate.y;
	e22 = 1.0f;
}


// Make this matrix an 2D affine rotation by the given angle (radians)
void CMatrix3x3::MakeRotation2D( const TFloat32 fAngle )
{
	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	e00 = c;
	e01 = s;
	e02 = 0.0f;

	e10 = -s;
	e11 = c;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}


// Make this matrix a 2D affine scaling in X and Y by the values provided in the given vector
void CMatrix3x3::MakeScaling2D( const CVector2& scale )
{
	e00 = scale.x;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = scale.y;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}

// Make this matrix a 2D affine uniform scaling of the given amount
void CMatrix3x3::MakeScaling2D( const TFloat32 fScale )
{
	e00 = fScale;
	e01 = 0.0f;
	e02 = 0.0f;

	e10 = 0.0f;
	e11 = fScale;
	e12 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
}


/*-----------------------------------------------------------------------------------------
	2D Affine Transformation Matrices - non-member functions
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value) - see above

// Return a matrix that is a 2D affine translation of the given vector
CMatrix3x3 MatrixTranslation2D( const CVector2& translate )
{
	CMatrix3x3 m;

	m.e00 = 1.0f;
	m.e01 = 0.0f;
	m.e02 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = 1.0f;
	m.e12 = 0.0f;

	m.e20 = translate.x;
	m.e21 = translate.y;
	m.e22 = 1.0f;

	return m;
}


// Return a matrix that is a 2D affine rotation of the given angle (radians)
CMatrix3x3 MatrixRotation2D( const TFloat32 fAngle )
{
	CMatrix3x3 m;

	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	m.e00 = c;
	m.e01 = s;
	m.e02 = 0.0f;

	m.e10 = -s;
	m.e11 = c;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;

	return m;
}


// Return a matrix that is a 2D affine scaling in X and Y by the values provided in the given vector
CMatrix3x3 MatrixScaling2D( const CVector2& scale )
{
	CMatrix3x3 m;

	m.e00 = scale.x;
	m.e01 = 0.0f;
	m.e02 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = scale.y;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;

	return m;
}

// Return a matrix that is a 2D affine uniform scaling of the given amount
CMatrix3x3 MatrixScaling2D( const TFloat32 fScale )
{
	CMatrix3x3 m;

	m.e00 = fScale;
	m.e01 = 0.0f;
	m.e02 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = fScale;
	m.e12 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;

	return m;
}


/*-----------------------------------------------------------------------------------------
	Facing Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix a rotation matrix that faces in given direction (Z axis). Can pass up
// vector for the constructed matrix and specify handedness (right-handed Z axis will face
// away from direction)
void CMatrix3x3::FaceDirection
(
	const CVector3& direction,
	const CVector3& up /*= CVector3::kYAxis*/,
	const bool      bLH /*= true*/
)
{
	CVector3 axisX, axisY, axisZ;
	if (bLH)
	{
		axisZ = Normalise( direction );
		if (axisZ.IsZero()) return;
		axisX = Normalise( Cross( up, axisZ ) );
		if (axisX.IsZero()) return;
		axisY = Cross( axisZ, axisX ); // Will already be normalised
	}
	else
	{
		axisZ = Normalise( -direction );
		if (axisZ.IsZero()) return;
		axisX = Normalise( Cross( axisZ, up ) );
		if (axisX.IsZero()) return;
		axisY = Cross( axisX, axisZ ); // Will already be normalised
	}
	SetRow( 0, axisX );
	SetRow( 1, axisY );
	SetRow( 2, axisZ );
}

// Return rotation matrix that faces in given direction (Z axis). Can pass up vector for the
// constructed matrix and specify handedness (right-handed Z axis will face away from direction)
CMatrix3x3 MatrixFaceDirection
(
	const CVector3& direction,
	const CVector3& up /*= CVector3::kYAxis*/,
	const bool      bLH /*= true*/
)
{
	CVector3 axisX, axisY, axisZ;
	if (bLH)
	{
		axisZ = Normalise( direction );
		if (axisZ.IsZero()) return CMatrix3x3::kIdentity;
		axisX = Normalise( Cross( up, axisZ ) );
		if (axisX.IsZero()) return CMatrix3x3::kIdentity;
		axisY = Cross( axisZ, axisX ); // Will already be normalised
	}
	else
	{
		axisZ = Normalise( -direction );
		if (axisZ.IsZero()) return CMatrix3x3::kIdentity;
		axisX = Normalise( Cross( axisZ, up ) );
		if (axisX.IsZero()) return CMatrix3x3::kIdentity;
		axisY = Cross( axisX, axisZ ); // Will already be normalised
	}
	
	return CMatrix3x3( axisX, axisY, axisZ );
}

// Make this matrix an affine 2D transformation matrix to point from current position to given
// target. Can specify whether to use X or Y axis and handedness (defaults: Y, left)
// Will retain the matrix's current scaling
void CMatrix3x3::FaceTarget2D
(
	const CVector2& target,
	const bool      bUseYAxis /*= true*/,
	const bool      bLH /*= true*/
)
{
	CVector2 scale = GetScale2D();
	CVector2 axis = Normalise(target - GetPosition2D());
	if (axis.IsZero()) return;
	if (bUseYAxis)
	{
		e10 = axis.x * scale.y;
		e11 = axis.y * scale.y;
		if (bLH)
		{
			e00 = e11 * scale.x;
			e01 = -e10 * scale.x;
		}
		else
		{
			e00 = -e11 * scale.x;
			e01 = e10 * scale.x;
		}
	}
	else
	{
		e00 = axis.x * scale.x;
		e01 = axis.y * scale.x;
		if (bLH)
		{
			e11 = -e00 * scale.y;
			e10 = e01 * scale.y;
		}
		else
		{
			e11 = e00 * scale.y;
			e10 = -e01 * scale.y;
		}
	}
}

// Return affine 3D transformation matrix that points from given position to given target. Can
// specify whether to use X or Y axis and handedness (defaults: Y, left)
CMatrix3x3 MatrixFaceTarget2D
(
	const CVector2& position,
	const CVector2& target,
	const bool      bUseYAxis /*= true*/,
	const bool      bLH /*= true*/
)
{
	CMatrix3x3 m;

	CVector2 axis = Normalise(target - position);
	if (axis.IsZero()) return CMatrix3x3( position );
	if (bUseYAxis)
	{
		m.e10 = axis.x;
		m.e11 = axis.y;
		m.e12 = 0.0f;
		if (bLH)
		{
			m.e00 = m.e11;
			m.e01 = -m.e10;
		}
		else
		{
			m.e00 = -m.e11;
			m.e01 = m.e10;
		}
		m.e02 = 0.0f;
	}
	else
	{
		m.e00 = axis.x;
		m.e01 = axis.y;
		m.e02 = 0.0f;
		if (bLH)
		{
			m.e11 = -m.e00;
			m.e10 = m.e01;
		}
		else
		{
			m.e11 = m.e00;
			m.e10 = -m.e01;
		}
		m.e12 = 0.0f;
	}

	m.e20 = position.x;
	m.e21 = position.y;
	m.e22 = 1.0f;

	return m;
}


// Make this matrix an affine 2D transformation matrix that faces in given direction from the
// current position. Can specify whether to use X or Y axis and handedness (defaults: Y, left)
void CMatrix3x3::FaceDirection2D
(
	const CVector2& direction,
	const bool      bUseYAxis /*= true*/,
	const bool      bLH /*= true*/
)
{
	CVector2 scale = GetScale2D();
	CVector2 axis = Normalise( direction );
	if (axis.IsZero()) return;
	if (bUseYAxis)
	{
		e10 = axis.x * scale.y;
		e11 = axis.y * scale.y;
		if (bLH)
		{
			e00 = e11 * scale.x;
			e01 = -e10 * scale.x;
		}
		else
		{
			e00 = -e11 * scale.x;
			e01 = e10 * scale.x;
		}
	}
	else
	{
		e00 = axis.x * scale.x;
		e01 = axis.y * scale.x;
		if (bLH)
		{
			e11 = -e00 * scale.y;
			e10 = e01 * scale.y;
		}
		else
		{
			e11 = e00 * scale.y;
			e10 = -e01 * scale.y;
		}
	}
}

// Return affine 3D transformation matrix that faces in given direction from the given position.
// Can specify whether to use X or Y axis and handedness (defaults: Y, left)
CMatrix3x3 MatrixFaceDirection2D
(
	const CVector2& position,
	const CVector2& direction,
	const bool      bUseYAxis /*= true*/,
	const bool      bLH /*= true*/
)
{
	CMatrix3x3 m;

	CVector2 axis = Normalise( direction );
	if (axis.IsZero()) return CMatrix3x3( position );
	if (bUseYAxis)
	{
		m.e10 = axis.x;
		m.e11 = axis.y;
		m.e12 = 0.0f;
		if (bLH)
		{
			m.e00 = m.e11;
			m.e01 = -m.e10;
		}
		else
		{
			m.e00 = -m.e11;
			m.e01 = m.e10;
		}
		m.e02 = 0.0f;
	}
	else
	{
		m.e00 = axis.x;
		m.e01 = axis.y;
		m.e02 = 0.0f;
		if (bLH)
		{
			m.e11 = -m.e00;
			m.e10 = m.e01;
		}
		else
		{
			m.e11 = m.e00;
			m.e10 = -m.e01;
		}
		m.e12 = 0.0f;
	}

	m.e20 = position.x;
	m.e21 = position.y;
	m.e22 = 1.0f;

	return m;
}


/*-----------------------------------------------------------------------------------------
	Matrix Operators
-----------------------------------------------------------------------------------------*/

///////////////////////////////
// Comparison

// Matrix equality - non-member function
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator==
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
)
{
	return AreEqual(m1.e00, m2.e00) && AreEqual(m1.e01, m2.e01) && AreEqual(m1.e02, m2.e02) &&
		   AreEqual(m1.e10, m2.e10) && AreEqual(m1.e11, m2.e11) && AreEqual(m1.e12, m2.e12) &&
	       AreEqual(m1.e20, m2.e20) && AreEqual(m1.e21, m2.e21) && AreEqual(m1.e22, m2.e22);
}

// Matrix inequality - non-member function
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator!=
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
)
{
	return !AreEqual(m1.e00, m2.e00) || !AreEqual(m1.e01, m2.e01) || !AreEqual(m1.e02, m2.e02) ||
		   !AreEqual(m1.e10, m2.e10) || !AreEqual(m1.e11, m2.e11) || !AreEqual(m1.e12, m2.e12) ||
	       !AreEqual(m1.e20, m2.e20) || !AreEqual(m1.e21, m2.e21) || !AreEqual(m1.e22, m2.e22);
}


//////////////////////////////////
// Scalar multiplication/division

// Scalar multiplication
CMatrix3x3& CMatrix3x3::operator*=( const TFloat32 s )
{
	e00 *= s;
	e01 *= s;
	e02 *= s;
	e10 *= s;
	e11 *= s;
	e12 *= s;
	e20 *= s;
	e21 *= s;
	e22 *= s;

	return *this;
}

// Matrix-scalar multiplication - non-member version
CMatrix3x3 operator*
(
	const CMatrix3x3& m,
	const TFloat32    s
)
{
	CMatrix3x3 mOut;
    mOut.e00 = m.e00 * s;
    mOut.e01 = m.e01 * s;
    mOut.e02 = m.e02 * s;
    mOut.e10 = m.e10 * s;
    mOut.e11 = m.e11 * s;
    mOut.e12 = m.e12 * s;
    mOut.e20 = m.e20 * s;
    mOut.e21 = m.e21 * s;
    mOut.e22 = m.e22 * s;

	return mOut;
}

// Scalar-matrix multiplication - non-member version
CMatrix3x3 operator*
(
	const TFloat32    s,
	const CMatrix3x3& m
)
{
	CMatrix3x3 mOut;
    mOut.e00 = m.e00 * s;
    mOut.e01 = m.e01 * s;
    mOut.e02 = m.e02 * s;
    mOut.e10 = m.e10 * s;
    mOut.e11 = m.e11 * s;
    mOut.e12 = m.e12 * s;
    mOut.e20 = m.e20 * s;
    mOut.e21 = m.e21 * s;
    mOut.e22 = m.e22 * s;

	return mOut;
}


// Scalar division
CMatrix3x3& CMatrix3x3::operator/=(	const TFloat32 s )
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	e00 /= s;
	e01 /= s;
	e02 /= s;
	e10 /= s;
	e11 /= s;
	e12 /= s;
	e20 /= s;
	e21 /= s;
	e22 /= s;

	return *this;

	GEN_ENDGUARD_OPT;
}

// Matrix-scalar division - non-member version
CMatrix3x3 operator/
(
	const CMatrix3x3& m,
	const TFloat32    s
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	CMatrix3x3 mOut;
    mOut.e00 = m.e00 / s;
    mOut.e01 = m.e01 / s;
    mOut.e02 = m.e02 / s;
    mOut.e10 = m.e10 / s;
    mOut.e11 = m.e11 / s;
    mOut.e12 = m.e12 / s;
    mOut.e20 = m.e20 / s;
    mOut.e21 = m.e21 / s;
    mOut.e22 = m.e22 / s;

	return mOut;

	GEN_ENDGUARD_OPT;
}


///////////////////////////////
// Vector multiplication

// Vector-matrix multiplication (order is important - this is usual order for transformation
// for matrices stored as row vectors - see notes at top of header)
CVector3 operator*
(
	const CVector3&   v,
	const CMatrix3x3& m
)
{
    CVector3 vOut;
    vOut.x = v.x*m.e00 + v.y*m.e10 + v.z*m.e20;
    vOut.y = v.x*m.e01 + v.y*m.e11 + v.z*m.e21;
    vOut.z = v.x*m.e02 + v.y*m.e12 + v.z*m.e22;

    return vOut;
}

// Matrix-vector multiplication (order is important - this is an unusual order for matrices
// stored as row vectors - see notes at top of header)
CVector3 operator*
(
	const CMatrix3x3& m,
	const CVector3&   v
)
{
    CVector3 vOut;
    vOut.x = m.e00*v.x + m.e01*v.y + m.e02*v.z;
    vOut.y = m.e10*v.x + m.e11*v.y + m.e12*v.z;
    vOut.z = m.e20*v.x + m.e21*v.y + m.e22*v.z;

    return vOut;
}


// Return the given vector transformed by this matrix (pre-multiplication: V' = V*M)
CVector3 CMatrix3x3::Transform(	const CVector3& v ) const
{
	CVector3 vOut;
	vOut.x = v.x*e00 + v.y*e10 + v.z*e20;
	vOut.y = v.x*e01 + v.y*e11 + v.z*e21;
	vOut.z = v.x*e02 + v.y*e12 + v.z*e22;

	return vOut;
}

// Return the given CVector2 transformed by this matrix (pre-multiplication: V' = V*M)
// Assuming it is a vector rather then a point, i.e. assume the vector's 3th element is 0
CVector2 CMatrix3x3::TransformVector2D( const CVector2& v ) const
{
	CVector2 vOut;
	vOut.x = v.x*e00 + v.y*e10;
	vOut.y = v.x*e01 + v.y*e11;

	return vOut;
}

// Return the given CVector2 transformed by this matrix (pre-multiplication: V' = V*M)
// Assuming it is a point rather then a vector, i.e. assume the vector's 3th element is 1
CVector2 CMatrix3x3::TransformPoint2D( const CVector2& p ) const
{
	CVector2 pOut;
	pOut.x = p.x*e00 + p.y*e10 + e20;
	pOut.y = p.x*e01 + p.y*e11 + e21;

	return pOut;
}


///////////////////////////////
// Matrix multiplication

// Post-multiply this matrix by the given one
CMatrix3x3& CMatrix3x3::operator*=( const CMatrix3x3& m )
{
	if ( this == &m )
	{
		// Special case of multiplying by self - no copy optimisations so use binary version
		*this = m * m;
	}
	else
	{
		TFloat32 t0, t1;

		t0  = e00*m.e00 + e01*m.e10 + e02*m.e20;
		t1  = e00*m.e01 + e01*m.e11 + e02*m.e21;
		e02 = e00*m.e02 + e01*m.e12 + e02*m.e22;
		e00 = t0;
		e01 = t1;

		t0  = e10*m.e00 + e11*m.e10 + e12*m.e20;
		t1  = e10*m.e01 + e11*m.e11 + e12*m.e21;
		e12 = e10*m.e02 + e11*m.e12 + e12*m.e22;
		e10 = t0;
		e11 = t1;

		t0  = e20*m.e00 + e21*m.e10 + e22*m.e20;
		t1  = e20*m.e01 + e21*m.e11 + e22*m.e21;
		e22 = e20*m.e02 + e21*m.e12 + e22*m.e22;
		e20 = t0;
		e21 = t1;
	}
	return *this;
}


// Binary matrix-matrix multiplication - non-member version
CMatrix3x3 operator*
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
)
{
	CMatrix3x3 mOut;

	mOut.e00 = m1.e00*m2.e00 + m1.e01*m2.e10 + m1.e02*m2.e20;
	mOut.e01 = m1.e00*m2.e01 + m1.e01*m2.e11 + m1.e02*m2.e21;
	mOut.e02 = m1.e00*m2.e02 + m1.e01*m2.e12 + m1.e02*m2.e22;

	mOut.e10 = m1.e10*m2.e00 + m1.e11*m2.e10 + m1.e12*m2.e20;
	mOut.e11 = m1.e10*m2.e01 + m1.e11*m2.e11 + m1.e12*m2.e21;
	mOut.e12 = m1.e10*m2.e02 + m1.e11*m2.e12 + m1.e12*m2.e22;

	mOut.e20 = m1.e20*m2.e00 + m1.e21*m2.e10 + m1.e22*m2.e20;
	mOut.e21 = m1.e20*m2.e01 + m1.e21*m2.e11 + m1.e22*m2.e21;
	mOut.e22 = m1.e20*m2.e02 + m1.e21*m2.e12 + m1.e22*m2.e22;

	return mOut;
}


// Post-multiply this matrix by the given one assuming they are both affine
CMatrix3x3& CMatrix3x3::MultiplyAffine2D( const CMatrix3x3& m )
{
	if ( this == &m )
	{
		// Special case of multiplying by self - no copy optimisations so use binary version
		*this = gen::MultiplyAffine2D( m, m );
	}
	else
	{
		TFloat32 t;

		t   = e00*m.e00 + e01*m.e10;
		e01 = e00*m.e01 + e01*m.e11;
		e00 = t;

		t   = e10*m.e00 + e11*m.e10;
		e11 = e10*m.e01 + e11*m.e11;
		e10 = t;

		t   = e20*m.e00 + e21*m.e10 + m.e20;
		e21 = e20*m.e01 + e21*m.e11 + m.e21;
		e20 = t;
	}

	return *this;
}

// Binary matrix-matrix multiplication assuming both matrices are affine - non-member version
CMatrix3x3 MultiplyAffine2D
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
)
{
	CMatrix3x3 mOut;

	mOut.e00 = m1.e00*m2.e00 + m1.e01*m2.e10;
	mOut.e01 = m1.e00*m2.e01 + m1.e01*m2.e11;
	mOut.e02 = 0.0f;

	mOut.e10 = m1.e10*m2.e00 + m1.e11*m2.e10;
	mOut.e11 = m1.e10*m2.e01 + m1.e11*m2.e11;
	mOut.e12 = 0.0f;

	mOut.e20 = m1.e20*m2.e00 + m1.e21*m2.e10 + m2.e20;
	mOut.e21 = m1.e20*m2.e01 + m1.e21*m2.e11 + m2.e21;
	mOut.e22 = 1.0f;

	return mOut;
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard matrices
const CMatrix3x3 CMatrix3x3::kIdentity(1.0f, 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f);


} // namespace gen
