/**************************************************************************************************
	Module:       CMatrix4x4.cpp
	Author:       Laurent Noel
	Date created: 12/06/06

	Implementation of the concrete class CMatrix4x4, a 4x4 matrix of 32-bit floats. Supports
	general 4x4 matrices, but primarily designed for affine transformation matrices for 3D graphics

	See notes at top of header for some background to this class. Also see "Essential Mathematics
	for Games & Interactive Applications" (Van Verth & Bishop) for the mathematics behind the more
	complex code (e.g rotation conversions)

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "CMatrix4x4.h"

#include "Error.h"
#include "CVector4.h"
#include "CMatrix2x2.h"
#include "CMatrix3x3.h"
#include "CQuaternion.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/

// Construct by value
CMatrix4x4::CMatrix4x4
(
	const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02, const TFloat32 elt03,
	const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12, const TFloat32 elt13,
	const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22, const TFloat32 elt23,
	const TFloat32 elt30, const TFloat32 elt31, const TFloat32 elt32, const TFloat32 elt33
)
{
	e00 = elt00;
	e01 = elt01;
	e02 = elt02;
	e03 = elt03;

	e10 = elt10;
	e11 = elt11;
	e12 = elt12;
	e13 = elt13;

	e20 = elt20;
	e21 = elt21;
	e22 = elt22;
	e23 = elt23;

	e30 = elt30;
	e31 = elt31;
	e32 = elt32;
	e33 = elt33;
}

// Construct through pointer to 16 floats, may specify row/column order of data
CMatrix4x4::CMatrix4x4
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
		e03 = pfElts[3];

		e10 = pfElts[4];
		e11 = pfElts[5];
		e12 = pfElts[6];
		e13 = pfElts[7];

		e20 = pfElts[8];
		e21 = pfElts[9];
		e22 = pfElts[10];
		e23 = pfElts[11];

		e30 = pfElts[12];
		e31 = pfElts[13];
		e32 = pfElts[14];
		e33 = pfElts[15];
	}
	else
	{
		e00 = pfElts[0];
		e10 = pfElts[1];
		e20 = pfElts[2];
		e30 = pfElts[3];

		e01 = pfElts[4];
		e11 = pfElts[5];
		e21 = pfElts[6];
		e31 = pfElts[7];

		e02 = pfElts[8];
		e12 = pfElts[9];
		e22 = pfElts[10];
		e32 = pfElts[11];

		e03 = pfElts[12];
		e13 = pfElts[13];
		e23 = pfElts[14];
		e33 = pfElts[15];
	}

	GEN_ENDGUARD_OPT;
}

// Construct by row or column using CVector4's, may specify if setting rows or columns
CMatrix4x4::CMatrix4x4
(
	const CVector4& v0,
	const CVector4& v1,
	const CVector4& v2,
	const CVector4& v3,
	const bool      bRows /*= true*/
)
{
	if (bRows)
	{
		e00 = v0.x;
		e01 = v0.y;
		e02 = v0.z;
		e03 = v0.w;

		e10 = v1.x;
		e11 = v1.y;
		e12 = v1.z;
		e13 = v1.w;

		e20 = v2.x;
		e21 = v2.y;
		e22 = v2.z;
		e23 = v2.w;

		e30 = v3.x;
		e31 = v3.y;
		e32 = v3.z;
		e33 = v3.w;
	}
	else
	{
		e00 = v0.x;
		e10 = v0.y;
		e20 = v0.z;
		e30 = v0.w;

		e01 = v1.x;
		e11 = v1.y;
		e21 = v1.z;
		e31 = v1.w;

		e02 = v2.x;
		e12 = v2.y;
		e22 = v2.z;
		e32 = v2.w;

		e03 = v3.x;
		e13 = v3.y;
		e23 = v3.z;
		e33 = v3.w;
	}
}

// Construct by row or column using CVector3's, remaining elements taken from identity matrix
// May specify if setting rows or columns
CMatrix4x4::CMatrix4x4
(
	const CVector3& v0,
	const CVector3& v1,
	const CVector3& v2,
	const CVector3& v3,
	const bool      bRows /*= true*/
)
{
	if (bRows)
	{
		e00 = v0.x;
		e01 = v0.y;
		e02 = v0.z;
		e03 = 0.0f;

		e10 = v1.x;
		e11 = v1.y;
		e12 = v1.z;
		e13 = 0.0f;

		e20 = v2.x;
		e21 = v2.y;
		e22 = v2.z;
		e23 = 0.0f;

		e30 = v3.x;
		e31 = v3.y;
		e32 = v3.z;
		e33 = 1.0f;
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

		e03 = v3.x;
		e13 = v3.y;
		e23 = v3.z;

		e30 = 0.0f;
		e31 = 0.0f;
		e32 = 0.0f;
		e33 = 1.0f;
	}
}
 
// Construct affine transformation from position (translation) only
CMatrix4x4::CMatrix4x4( const CVector3& position )
{
	// Take most elements from identity
	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
	e23 = 0.0f;

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
	e33 = 1.0f;
}

// Construct affine transformation from position, Euler angles and optional scaling, with 
// remaining elements taken from the identity matrix. May specify order to apply rotations
// Matrix is effectively built in this order: M = Scale*Rotation*Translation
CMatrix4x4::CMatrix4x4
(
	const CVector3&      position,
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

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
}

// Construct affine transformation from quaternion and optional position & scaling, with 
// remaining elements taken from the identity matrix
// Matrix is effectively built in this order: M = Scale*Rotation*Translation
CMatrix4x4::CMatrix4x4
(
	const CQuaternion& quat,
	const CVector3&    position /*= CVector3::kOrigin*/,
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
	e03  = 0.0f; // Add 0's in fourth column

	e10  = scale.y * (xy - wz);
	e11  = scale.y * (1 - xx - zz);
	e12  = scale.y * (yz + wx);
	e13  = 0.0f;

	e20  = scale.z * (zx + wy);
	e21  = scale.z * (yz - wx);
	e22 = scale.z * (1 - xx - yy);
	e23 = 0.0f;

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
	e33 = 1.0f;
}

// Construct affine transformation from axis/angle of rotation and optional position & scaling,
// with remaining elements taken from the identity matrix
// Matrix is effectively built in this order: M = Scale*Rotation*Translation
CMatrix4x4::CMatrix4x4
(
	const CVector3& axis,
	const TFloat32  fAngle,
	const CVector3& position /*= CVector3::kOrigin*/,
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

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
}


// Construct from a CMatrix2x2 and optional 2D position, with remaining elements taken from
// the identity matrix
CMatrix4x4::CMatrix4x4
(
	const CMatrix2x2& m,
	const CVector2&   position /*= CVector2::kOrigin*/
)
{
	// Copy 2x2 elements, adding extra identity elements
	e00 = m.e00;
	e01 = m.e01;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = m.e10;
	e11 = m.e11;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
	e23 = 0.0f;

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = 0.0f;
	e33 = 1.0f;
}

// Construct from a CMatrix3x3 and optional 3D position, with remaining elements from the
// identity matrix
CMatrix4x4::CMatrix4x4
(
	const CMatrix3x3& m,
	const CVector3&   position /*= CVector2::kOrigin*/
)
{
	// Copy 3x3 elements, adding extra identity elements
	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;
	e03 = 0.0f;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;
	e13 = 0.0f;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;
	e23 = 0.0f;

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
	e33 = 1.0f;
}


// Copy constructor
CMatrix4x4::CMatrix4x4( const CMatrix4x4& m )
{
	e00 = m.e00;
	e01 = m.e01;
	e02 = m.e02;
	e03 = m.e03;

	e10 = m.e10;
	e11 = m.e11;
	e12 = m.e12;
	e13 = m.e13;

	e20 = m.e20;
	e21 = m.e21;
	e22 = m.e22;
	e23 = m.e23;

	e30 = m.e30;
	e31 = m.e31;
	e32 = m.e32;
	e33 = m.e33;
}

// Assignment operator
CMatrix4x4& CMatrix4x4::operator=( const CMatrix4x4& m )
{
	if ( this != &m )
	{
		e00 = m.e00;
		e01 = m.e01;
		e02 = m.e02;
		e03 = m.e03;

		e10 = m.e10;
		e11 = m.e11;
		e12 = m.e12;
		e13 = m.e13;

		e20 = m.e20;
		e21 = m.e21;
		e22 = m.e22;
		e23 = m.e23;

		e30 = m.e30;
		e31 = m.e31;
		e32 = m.e32;
		e33 = m.e33;
	}
	return *this;
}


/*-----------------------------------------------------------------------------------------
	Setters
-----------------------------------------------------------------------------------------*/

// Set by value
void CMatrix4x4::Set
(
	const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02, const TFloat32 elt03,
	const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12, const TFloat32 elt13,
	const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22, const TFloat32 elt23,
	const TFloat32 elt30, const TFloat32 elt31, const TFloat32 elt32, const TFloat32 elt33
)
{
	e00 = elt00;
	e01 = elt01;
	e02 = elt02;
	e03 = elt03;

	e10 = elt10;
	e11 = elt11;
	e12 = elt12;
	e13 = elt13;

	e20 = elt20;
	e21 = elt21;
	e22 = elt22;
	e23 = elt23;

	e30 = elt30;
	e31 = elt31;
	e32 = elt32;
	e33 = elt33;
}

// Set through pointer to 16 floats, may specify column/row order of data
void CMatrix4x4::Set
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
		e03 = pfElts[3];

		e10 = pfElts[4];
		e11 = pfElts[5];
		e12 = pfElts[6];
		e13 = pfElts[7];

		e20 = pfElts[8];
		e21 = pfElts[9];
		e22 = pfElts[10];
		e23 = pfElts[11];

		e30 = pfElts[12];
		e31 = pfElts[13];
		e32 = pfElts[14];
		e33 = pfElts[15];
	}
	else
	{
		e00 = pfElts[0];
		e10 = pfElts[1];
		e20 = pfElts[2];
		e30 = pfElts[3];

		e01 = pfElts[4];
		e11 = pfElts[5];
		e21 = pfElts[6];
		e31 = pfElts[7];

		e02 = pfElts[8];
		e12 = pfElts[9];
		e22 = pfElts[10];
		e32 = pfElts[11];

		e03 = pfElts[12];
		e13 = pfElts[13];
		e23 = pfElts[14];
		e33 = pfElts[15];
	}

	GEN_ENDGUARD_OPT;
}


/*-----------------------------------------------------------------------------------------
	Row/column getters & setters
-----------------------------------------------------------------------------------------*/

// Get a single row (range 0-3) of the matrix
CVector4 CMatrix4x4::GetRow( const TUInt32 iRow ) const
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 4, "Invalid parameter" );

	return CVector4( &e00 + iRow * 4 );

	GEN_ENDGUARD_OPT;
}

// Get a single column (range 0-3) of the matrix
CVector4 CMatrix4x4::GetColumn( const TUInt32 iCol ) const
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 4, "Invalid parameter" );

	const TFloat32* pfElts = &e00 + iCol;
	return CVector4( pfElts[0], pfElts[4], pfElts[8], pfElts[12] );

	GEN_ENDGUARD_OPT;
}


// Set a single row (range 0-3) of the matrix
void CMatrix4x4::SetRow
(
	const TUInt32   iRow,
	const CVector4& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 4, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iRow * 4;
	pfElts[0] = v.x;
	pfElts[1] = v.y;
	pfElts[2] = v.z;
	pfElts[3] = v.w;

	GEN_ENDGUARD_OPT;
}

// Set a single row (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
void CMatrix4x4::SetRow
(
	const TUInt32   iRow,
	const CVector3& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 4, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iRow * 4;
	pfElts[0] = v.x;
	pfElts[1] = v.y;
	pfElts[2] = v.z;

	GEN_ENDGUARD_OPT;
}


// Set a single column (range 0-3) of the matrix
void CMatrix4x4::SetColumn
(
	const TUInt32   iCol,
	const CVector4& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 4, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iCol;
	pfElts[0] = v.x;
	pfElts[4] = v.y;
	pfElts[8] = v.z;
	pfElts[12] = v.w;

	GEN_ENDGUARD_OPT;
}

// Set a single column (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
void CMatrix4x4::SetColumn
(
	const TUInt32   iCol,
	const CVector3& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 4, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iCol;
	pfElts[0] = v.x;
	pfElts[4] = v.y;
	pfElts[8] = v.z;

	GEN_ENDGUARD_OPT;
}


// Set all rows of the matrix at once
void CMatrix4x4::SetRows
(
	const CVector4& v0,
	const CVector4& v1,
	const CVector4& v2,
	const CVector4& v3
)
{
	e00 = v0.x;
	e01 = v0.y;
	e02 = v0.z;
	e03 = v0.w;

	e10 = v1.x;
	e11 = v1.y;
	e12 = v1.z;
	e13 = v1.w;

	e20 = v2.x;
	e21 = v2.y;
	e22 = v2.z;
	e23 = v2.w;

	e30 = v3.x;
	e31 = v3.y;
	e32 = v3.z;
	e33 = v3.w;
}

// Set all rows of the matrix at once using CVector3, with remaining elements taken from the
// identity matrix
void CMatrix4x4::SetRows
(
	const CVector3& v0,
	const CVector3& v1,
	const CVector3& v2,
	const CVector3& v3
)
{
	e00 = v0.x;
	e01 = v0.y;
	e02 = v0.z;
	e03 = 0.0f;

	e10 = v1.x;
	e11 = v1.y;
	e12 = v1.z;
	e13 = 0.0f;

	e20 = v2.x;
	e21 = v2.y;
	e22 = v2.z;
	e23 = 0.0f;

	e30 = v3.x;
	e31 = v3.y;
	e32 = v3.z;
	e33 = 1.0f;
}


// Set all columns of the matrix at once
void CMatrix4x4::SetColumns
(
	const CVector4& v0,
	const CVector4& v1,
	const CVector4& v2,
	const CVector4& v3
)
{
	e00 = v0.x;
	e10 = v0.y;
	e20 = v0.z;
	e30 = v0.w;

	e01 = v1.x;
	e11 = v1.y;
	e21 = v1.z;
	e31 = v1.w;

	e02 = v2.x;
	e12 = v2.y;
	e22 = v2.z;
	e32 = v2.w;

	e03 = v3.x;
	e13 = v3.y;
	e23 = v3.z;
	e33 = v3.w;
}

// Set all columns of the matrix at once using CVector3, with remaining elements taken from the
// identity matrix
void CMatrix4x4::SetColumns
(
	const CVector3& v0,
	const CVector3& v1,
	const CVector3& v2,
	const CVector3& v3
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

	e03 = v3.x;
	e13 = v3.y;
	e23 = v3.z;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}


/*-----------------------------------------------------------------------------------------
	Creation and Decomposition of Affine Matrices
-----------------------------------------------------------------------------------------*/

// Make matrix an affine transformation given position & optional Euler angles & scaling, with
// remaining elements taken from the identity matrix. May specify order to apply rotations.
// Matrix is built in this order: M = Scale*Rotation*Translation
void CMatrix4x4::MakeAffineEuler
(
	const CVector3&      position,
	const CVector3&      angles /*= CVector3::kZero*/,
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

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
}

// Make matrix an affine transformation given quaternion and optional position & scaling, with 
// remaining elements taken from the identity matrix.
// Matrix is built in this order: M = Scale*Rotation*Translation
void CMatrix4x4::MakeAffineQuaternion
(
	const CQuaternion& quat,
	const CVector3&    position /*= CVector3::kOrigin*/,
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
	e03  = 0.0f; // Add 0's in fourth column

	e10  = scale.y * (xy - wz);
	e11  = scale.y * (1 - xx - zz);
	e12  = scale.y * (yz + wx);
	e13  = 0.0f;

	e20  = scale.z * (zx + wy);
	e21  = scale.z * (yz - wx);
	e22 = scale.z * (1 - xx - yy);
	e23 = 0.0f;

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
	e33 = 1.0f;
}

// Make matrix an affine transformation given angle/axis of rotation and optional position &
// scaling, with remaining elements taken from the identity matrix
// Matrix is built in this order: M = Scale*Rotation*Translation
void CMatrix4x4::MakeAffineAxisAngle
(
	const CVector3& axis,
	const TFloat32  fAngle,
	const CVector3& position /*= CVector3::kOrigin*/,
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

	// Put position (translation) in bottom row
	e30 = position.x;
	e31 = position.y;
	e32 = position.z;
}


// Decompose affine transformation into position, Euler angles of rotation (X, Y & Z angles of
// rotation around each axis) and scale. Optionally pass order of rotations. Pass NULL for any
// unneeded parameters. Assumes matrix is built in this order: M = Scale*Rotation*Translation
void CMatrix4x4::DecomposeAffineEuler
(
	CVector3*            pPosition,
	CVector3*            pAngles,
	CVector3*            pScale,
	const ERotationOrder eRotOrder /*= kZXY*/
) const
{
	GEN_GUARD;

	// Get position if required
	if (pPosition)
	{
		pPosition->x = e30;
		pPosition->y = e31;
		pPosition->z = e32;
	}

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

// Decompose affine transformation into position, quaternion of rotation and scale. Pass NULL
// for any unneeded parameters. Assumes built in this order: M = Scale*Rotation*Translation
void CMatrix4x4::DecomposeAffineQuaternion
(
	CVector3*    pPosition,
	CQuaternion* pQuat,
	CVector3*    pScale
) const
{
	// Get position if required
	if (pPosition)
	{
		pPosition->x = e30;
		pPosition->y = e31;
		pPosition->z = e32;
	}

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

// Decompose affine transformation into position, angle/axis of rotation (an axis and amount of
// rotation around that axis) and scale. Pass NULL for any unneeded parameters. Assumes matrix
// is built in this order: M = Scale*Rotation*Translation
void CMatrix4x4::DecomposeAffineAxisAngle
(
	CVector3* pPosition,
	CVector3* pAxis,
	TFloat32* pfAngle,
	CVector3* pScale
) const
{
	// Get position if required
	if (pPosition)
	{
		pPosition->x = e30;
		pPosition->y = e31;
		pPosition->z = e32;
	}

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
				pAxis->Set( e12 * invScaleY - e21 * invScaleZ, 
				            e20 * invScaleZ - e02 * invScaleX,
							e01 * invScaleX - e10 * invScaleY );
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
	Manipulation of Affine Matrices
-----------------------------------------------------------------------------------------*/

// Get the X, Y & Z scaling of an affine transformation matrix
CVector3 CMatrix4x4::GetScale() const
{
	CVector3 scale;
	scale.x = Sqrt( e00*e00 + e01*e01 + e02*e02 );
	scale.y = Sqrt( e10*e10 + e11*e11 + e12*e12 );
	scale.z = Sqrt( e20*e20 + e21*e21 + e22*e22 );
	return scale;
}


// Set the X scaling of an affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix4x4::SetScaleX( const TFloat32 x )
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

// Set the Y scaling of an affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix4x4::SetScaleY( const TFloat32 y )
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

// Set the Z scaling of an affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix4x4::SetScaleZ( const TFloat32 z )
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

// Set the X, Y & Z scaling of an affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix4x4::SetScale( const CVector3& scale )
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

// Set a uniform scaling for an affine transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (position, rotation etc.)
void CMatrix4x4::SetScale( const TFloat32 fScale )
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
	Comparisons
-----------------------------------------------------------------------------------------*/

// Test if matrix is the identity
// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
bool CMatrix4x4::IsIdentity() const
{
	return IsZero( e00 - 1.0f ) && IsZero( e11 - 1.0f ) && 
	       IsZero( e22 - 1.0f ) && IsZero( e33 - 1.0f ) && 
	       IsZero( e01 ) && IsZero( e02 ) && IsZero( e03 ) && 
	       IsZero( e10 ) && IsZero( e12 ) && IsZero( e13 ) && 
	       IsZero( e20 ) && IsZero( e21 ) && IsZero( e23 ) && 
	       IsZero( e30 ) && IsZero( e31 ) && IsZero( e32 );
}


/*-----------------------------------------------------------------------------------------
	Othogonality
-----------------------------------------------------------------------------------------*/

// See extensive notes in header file

// Test if upper-left 3x3 matrix has orthogonal rows, i.e. if the three rows are vectors
// at right angles to each other. Will also be orthonormal if it contains no scaling
bool CMatrix4x4::IsOrthogonal3x3() const
{
	// Check dot product of each pair of rows is zero
	return IsZero( e00*e10 + e01*e11 + e02*e12 ) &&
	       IsZero( e10*e20 + e11*e21 + e12*e22 ) &&
	       IsZero( e20*e00 + e21*e01 + e22*e02 );
}

// Test if upper-left 3x3 matrix has orthonormal rows, i.e. if the three rows are *normals*
// at right angles to each other. Scaled matrices cannot be orthonormal (can be orthogonal)
bool CMatrix4x4::IsOrthonormal3x3() const
{
	// Check each row is length 1 and dot product of each pair of rows is zero
	return IsZero( e00*e00 + e01*e01 + e02*e02 - 1.0f ) &&
	       IsZero( e10*e10 + e11*e11 + e12*e12 - 1.0f ) &&
	       IsZero( e20*e20 + e21*e21 + e22*e22 - 1.0f ) &&
	       IsZero( e00*e10 + e01*e11 + e02*e12 ) &&
	       IsZero( e10*e20 + e11*e21 + e12*e22 ) &&
	       IsZero( e20*e00 + e21*e01 + e22*e02 );
}

// Orthogonalise the rows/columns of the upper-left 3x3 matrix. Generally used to "correct"
// matrices that become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
void CMatrix4x4::Orthogonalise3x3( const CVector3& scale /*= CVector3::kOne*/ )
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

// Return a copy of given matrix with orthogonalised rows/columns in the upper-left 3x3 matrix.
// Generally used to "correct" matrices that become non-orthogonal after repeated calculations /
// floating point rounding. May pass scaling for resultant rows - default is 1, leading to
// "orthonormalisation"
CMatrix4x4 Orthogonalise3x3
(
	const CMatrix4x4& m,
	const CVector3&   scale /*= CVector3::kOne*/
)
{
	GEN_GUARD;

	CMatrix4x4 mOut;

	// Normalise first vector (x-axis)
	TFloat32 origScaleX = m.e00*m.e00 + m.e01*m.e01 + m.e02*m.e02;
	GEN_ASSERT( !IsZero(origScaleX), "Singular matrix" );
	TFloat32 invOrigScaleX = InvSqrt( origScaleX );
	mOut.e00 = m.e00 * invOrigScaleX;
	mOut.e01 = m.e01 * invOrigScaleX;
	mOut.e02 = m.e02 * invOrigScaleX;
	mOut.e03 = m.e03;

	// Orthogonalise second vector (y-axis) with first
	TFloat32 proj10 = m.e10*mOut.e00 + m.e11*mOut.e01 + m.e12*mOut.e02;
	mOut.e10 = m.e10 - proj10 * mOut.e00;
	mOut.e11 = m.e11 - proj10 * mOut.e01;
	mOut.e12 = m.e12 - proj10 * mOut.e02;
	mOut.e13 = m.e13;

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
	mOut.e23 = m.e23;
	
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

	// Copy fourth row
	mOut.e30 = m.e30;
	mOut.e31 = m.e31;
	mOut.e32 = m.e32;
	mOut.e33 = m.e33;

	return mOut;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Inverse related
-----------------------------------------------------------------------------------------*/

// Set this matrix to its transpose (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
void CMatrix4x4::Transpose()
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

	t   = e03;
	e03 = e30;
	e30 = t;

	t   = e13;
	e13 = e31;
	e31 = t;

	t   = e23;
	e23 = e32;
	e32 = t;
}
    
// Return the transpose of given matrix (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
CMatrix4x4 Transpose( const CMatrix4x4& m )
{
	CMatrix4x4 transMat;

	transMat.e00 = m.e00;
	transMat.e01 = m.e10;
	transMat.e02 = m.e20;
	transMat.e03 = m.e30;

	transMat.e10 = m.e01;
	transMat.e11 = m.e11;
	transMat.e12 = m.e21;
	transMat.e13 = m.e31;

	transMat.e20 = m.e02;
	transMat.e21 = m.e12;
	transMat.e22 = m.e22;
	transMat.e23 = m.e32;

	transMat.e30 = m.e03;
	transMat.e31 = m.e13;
	transMat.e32 = m.e23;
	transMat.e33 = m.e33;

	return transMat;
}


// Set this matrix to its inverse assuming it is affine with an orthogonal upper-left 3x3
// matrix i.e. an affine transformation with no scaling or shear
// Most efficient inverse for transformations containing rotation and translation only
void CMatrix4x4::InvertRotTrans()
{
	// Inverse of upper left 3x3 is just the transpose
	TFloat32 t1, t2;
	t1  = e01;
	e01 = e10;
	e10 = t1;

	t1  = e02;
	e02 = e20;
	e20 = t1;

	t1  = e12;
	e12 = e21;
	e21 = t1;

	// Transform negative translation by inverted 3x3 to get inverse
	t1  = -e30*e00 - e31*e10 - e32*e20;
	t2  = -e30*e01 - e31*e11 - e32*e21;
	e32 = -e30*e02 - e31*e12 - e32*e22;
	e30 = t1;
	e31 = t2;
}

// Return the inverse of given matrix assuming it is affine with an orthogonal upper-left 3x3
// matrix i.e. an affine transformation with no scaling or shear
// Most efficient inverse for transformations containing rotation and translation only
CMatrix4x4 InverseRotTrans( const CMatrix4x4& m )
{
	CMatrix4x4 mOut;

	// Inverse of upper left 3x3 is just the transpose
	mOut.e00 = m.e00;
	mOut.e01 = m.e10;
	mOut.e02 = m.e20;
	mOut.e03 = 0.0f;

	mOut.e10 = m.e01;
	mOut.e11 = m.e11;
	mOut.e12 = m.e21;
	mOut.e13 = 0.0f;

	mOut.e20 = m.e02;
	mOut.e21 = m.e12;
	mOut.e22 = m.e22;
	mOut.e23 = 0.0f;

	// Transform negative translation by inverted 3x3 to get inverse
	mOut.e30 = -m.e30*mOut.e00 - m.e31*mOut.e10 - m.e32*mOut.e20;
	mOut.e31 = -m.e30*mOut.e01 - m.e31*mOut.e11 - m.e32*mOut.e21;
	mOut.e32 = -m.e30*mOut.e02 - m.e31*mOut.e12 - m.e32*mOut.e22;
	mOut.e33 = 1.0f;

	return mOut;
}


// Set this matrix to its inverse assuming it is affine and the upper-left 3x3 matrix contains
// orthogonal vectors i.e. an affine transformation matrix with no shear
// Most efficient inverse for transformations containing rotation, translation & scale only
void CMatrix4x4::InvertRotTransScale()
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

	// Inverse of upper left 3x3 is just the transpose with scaling inverse factored in
	TFloat32 t1, t2;
	e00 *= invScaleX;
	e11 *= invScaleY;
	e22 *= invScaleZ;

	t1  = e01;
	e01 = e10 * invScaleY;
	e10 = t1 * invScaleX;

	t1  = e02;
	e02 = e20 * invScaleZ;
	e20 = t1 * invScaleX;

	t1  = e12;
	e12 = e21 * invScaleZ;
	e21 = t1 * invScaleY;

	// Transform negative translation by inverted 3x3 to get inverse
	t1  = -e30*e00 - e31*e10 - e32*e20;
	t2  = -e30*e01 - e31*e11 - e32*e21;
	e32 = -e30*e02 - e31*e12 - e32*e22;
	e30 = t1;
	e31 = t2;

	GEN_ENDGUARD;
}

// Return the inverse of the given matrix assuming it is affine and the upper-left 3x3 matrix
// contains orthogonal vectors i.e. an affine transformation matrix with no shear
// Most efficient inverse for transformations containing rotation, translation & scale only
CMatrix4x4 InverseRotTransScale( const CMatrix4x4& m )
{
	GEN_GUARD;

	CMatrix4x4 mOut;

	// Get X, Y & Z scaling (squared)
	TFloat32 scaleSqX = m.e00*m.e00 + m.e01*m.e01 + m.e02*m.e02;
	TFloat32 scaleSqY = m.e10*m.e10 + m.e11*m.e11 + m.e12*m.e12;
	TFloat32 scaleSqZ = m.e20*m.e20 + m.e21*m.e21 + m.e22*m.e22;
	
	// Calculate scaling inverse
	GEN_ASSERT( !IsZero(scaleSqX) && !IsZero(scaleSqY) && !IsZero(scaleSqZ), "Singular matrix" );
	TFloat32 invScaleX = 1.0f / scaleSqX;
	TFloat32 invScaleY = 1.0f / scaleSqY;
	TFloat32 invScaleZ = 1.0f / scaleSqZ;

	// Inverse of upper left 3x3 is just the transpose with scaling inverse factored in
	mOut.e00 = m.e00 * invScaleX;
	mOut.e01 = m.e10 * invScaleY;
	mOut.e02 = m.e20 * invScaleZ;
	mOut.e03 = 0.0f;

	mOut.e10 = m.e01 * invScaleX;
	mOut.e11 = m.e11 * invScaleY;
	mOut.e12 = m.e21 * invScaleZ;
	mOut.e13 = 0.0f;

	mOut.e20 = m.e02 * invScaleX;
	mOut.e21 = m.e12 * invScaleY;
	mOut.e22 = m.e22 * invScaleZ;
	mOut.e23 = 0.0f;

	// Transform negative translation by inverted 3x3 to get inverse
	mOut.e30 = -m.e30*mOut.e00 - m.e31*mOut.e10 - m.e32*mOut.e20;
	mOut.e31 = -m.e30*mOut.e01 - m.e31*mOut.e11 - m.e32*mOut.e21;
	mOut.e32 = -m.e30*mOut.e02 - m.e31*mOut.e12 - m.e32*mOut.e22;
	mOut.e33 = 1.0f;

	return mOut;

	GEN_ENDGUARD;
}


// Set this matrix to its inverse assuming only that it is an affine matrix
void CMatrix4x4::InvertAffine()
{
	*this = InverseAffine( *this ); // TODO: Just use non-member version
}

// Return the inverse of given matrix assuming only that it is an affine matrix
CMatrix4x4 InverseAffine( const CMatrix4x4& m )
{
	GEN_GUARD;

	CMatrix4x4 mOut;

	// Calculate determinant of upper left 3x3
	TFloat32 det0 = m.e11*m.e22 - m.e12*m.e21;
	TFloat32 det1 = m.e12*m.e20 - m.e10*m.e22;
	TFloat32 det2 = m.e10*m.e21 - m.e11*m.e20;
	TFloat32 det = m.e00*det0 + m.e01*det1 + m.e02*det2;
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Calculate inverse of upper left 3x3
	TFloat32 invDet = 1.0f / det;
	mOut.e00 = invDet * det0;
	mOut.e10 = invDet * det1;
	mOut.e20 = invDet * det2;
		  
	mOut.e01 = invDet * (m.e21*m.e02 - m.e22*m.e01);
	mOut.e11 = invDet * (m.e22*m.e00 - m.e20*m.e02);
	mOut.e21 = invDet * (m.e20*m.e01 - m.e21*m.e00);
		  
	mOut.e02 = invDet * (m.e01*m.e12 - m.e02*m.e11);
	mOut.e12 = invDet * (m.e02*m.e10 - m.e00*m.e12);
	mOut.e22 = invDet * (m.e00*m.e11 - m.e01*m.e10);

	// Transform negative translation by inverted 3x3 to get inverse
	mOut.e30 = -m.e30*mOut.e00 - m.e31*mOut.e10 - m.e32*mOut.e20;
	mOut.e31 = -m.e30*mOut.e01 - m.e31*mOut.e11 - m.e32*mOut.e21;
	mOut.e32 = -m.e30*mOut.e02 - m.e31*mOut.e12 - m.e32*mOut.e22;

	// Fill in right column for affine matrix
	mOut.e03 = 0.0f;
	mOut.e13 = 0.0f;
	mOut.e23 = 0.0f;
	mOut.e33 = 1.0f;

	return mOut;

	GEN_ENDGUARD;
}


// Return the cofactor of entry i,j. This is (-1)^(i+j) * determinant of the matrix after
// removing the ith and jth row/column. Used for calculating general inverse
TFloat32 CMatrix4x4::Cofactor( const TUInt32 i, const TUInt32 j )
{
	return gen::Cofactor( *this, i, j ); // TODO: Just use non-member version
}

// Return the cofactor of entry i,j of the given matrix. This is (-1)^(i+j) * determinant of
// the matrix after removing the ith and jth row/column. Used for calculating general inverse
TFloat32 Cofactor
(
	const CMatrix4x4& m,
	const TUInt32     i,
	const TUInt32     j
)
{
	// Get rows and columns involved
	TUInt32 rows[3];
	TUInt32 cols[3];
	TUInt32 row = 0, col = 0;
	for (TUInt32 rowCol = 0; rowCol < 4; ++rowCol)
	{
		if (rowCol != i) rows[row++] = rowCol;
		if (rowCol != j) cols[col++] = rowCol;
	}

	// Calculate 3x3 determinant
	TFloat32 det0 = m[rows[1]][cols[1]]*m[rows[2]][cols[2]] - 
	                m[rows[1]][cols[2]]*m[rows[2]][cols[1]];
	TFloat32 det1 = m[rows[1]][cols[2]]*m[rows[2]][cols[0]] - 
	                m[rows[1]][cols[0]]*m[rows[2]][cols[2]];
	TFloat32 det2 = m[rows[1]][cols[0]]*m[rows[2]][cols[1]] - 
	                m[rows[1]][cols[1]]*m[rows[2]][cols[0]];
	TFloat32 det = m[rows[0]][cols[0]]*det0 + 
	               m[rows[0]][cols[1]]*det1 +
				   m[rows[0]][cols[2]]*det2;

	// Determine if i+j is even/odd to calculate (-1) term
	return ((((i+j) & 1) == 0) ? det : -det);
}


// Set this matrix to its inverse. Most general, least efficient inverse function
// Suitable for non-affine matrices (e.g. a perspective projection matrix)
void CMatrix4x4::Invert()
{
	*this = Inverse( *this ); // TODO: Just use non-member version
}

// Return the inverse of given matrix. Most general, least efficient inverse function
// Suitable for non-affine matrices (e.g. a perspective projection matrix)
CMatrix4x4 Inverse( const CMatrix4x4& m )
{
	GEN_GUARD;

	CMatrix4x4 mOut;

	// Calculate determinant
	TFloat32 det = m.e00 * Cofactor( m, 0, 0 ) + m.e01 * Cofactor( m, 0, 1 ) + 
	               m.e02 * Cofactor( m, 0, 2 ) + m.e03 * Cofactor( m, 0, 3 ); 
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Inverse is (1/determinant)*adjoint matrix. Adjoint matrix is transposed matrix of cofactors
	TFloat32 invDet = 1.0f / det;
	for (TUInt32 i = 0; i < 4; ++i)
	{
		for (TUInt32 j = 0; j < 4; ++j)
		{
			mOut[i][j] = invDet * Cofactor( m, j, i );
		}
	}

	return mOut;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Transformation Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix the identity matrix
void CMatrix4x4::MakeIdentity()
{
	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f; 
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}

// Make this matrix a translation by the given vector
void CMatrix4x4::MakeTranslation( const CVector3& translate )
{
	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
	e23 = 0.0f;

	e30 = translate.x;
	e31 = translate.y;
	e32 = translate.z;
	e33 = 1.0f;
}

// Make this matrix an X-axis rotation by the given angle (radians)
void CMatrix4x4::MakeRotationX( const TFloat32 x )
{
	TFloat32 sX, cX;
	SinCos( x, &sX, &cX );

	e00 = 1.0f;
	e01 = 0.0f;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = cX;
	e12 = sX;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = -sX;
	e22 = cX;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}

// Make this matrix a Y-axis rotation by the given angle (radians)
void CMatrix4x4::MakeRotationY( const TFloat32 y )
{
	TFloat32 sY, cY;
	SinCos( y, &sY, &cY );

	e00 = cY;
	e01 = 0.0f;
	e02 = -sY;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = sY;
	e21 = 0.0f;
	e22 = cY;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;    
}

// Make this matrix a Z-axis rotation by the given angle (radians)
void CMatrix4x4::MakeRotationZ( const TFloat32 z )
{
	TFloat32 sZ, cZ;
	SinCos( z, &sZ, &cZ );

	e00 = cZ;
	e01 = sZ;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = -sZ;
	e11 = cZ;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = 1.0f;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}

// Make a matrix that is a combined rotation around the X, Y & Z axes by the given angles
// (radians), applied in the order specified
void CMatrix4x4::MakeRotation
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
			e03 = 0.0f;

			e10 = -sZ * cY;
			e11 = cZ * cX + -sZ * sY * sX;
			e12 = cZ * sX + sZ * sY * cX;
			e13 = 0.0f;

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
			e03 = 0.0f;

			e10 = -sZ;
			e11 = cZ * cX;
			e12 = cZ * sX;
			e13 = 0.0f;

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
			e03 = 0.0f;

			e10 = -cX * sZ * cY + sX * sY;
			e11 = cX * cZ;
			e12 = cX * sZ * sY + sX * cY;
			e13 = 0.0f;

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
			e03 = 0.0f;

			e10 = -sZ * cY + cZ * sX * sY;
			e11 = cZ * cX;
			e12 = sZ * sY + cZ * sX * cY;
			e13 = 0.0f;

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
			e03 = 0.0f;

			e10 = -cX * sZ;
			e11 = cX * cZ;
			e12 = sX;
			e13 = 0.0f;

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
			e03 = 0.0f;

			e10 = sX * sY * cZ + -cX * sZ;
			e11 = sX * sY * sZ + cX * cZ;
			e12 = sX * cY;
			e13 = 0.0f;

			e20 = cX * sY * cZ + sX * sZ;
			e21 = cX * sY * sZ + -sX * cZ;
			e22 = cX * cY;
			break;
		}

		default:
			GEN_ERROR( "Invalid parameter" );
	}
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;

	GEN_ENDGUARD;
}

// Make this matrix a rotation around the given axis by the given angle (radians)
void CMatrix4x4::MakeRotation
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
	e03 = 0.0f;

	e10 = txy - sz; 
	e11 = ty * axisNorm.y + c;
	e12 = tyz + sx;
	e13 = 0.0f;

	e20 = tzx + sy;
	e21 = tyz - sx;
	e22 = tz * axisNorm.z + c;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;

	GEN_ENDGUARD;
}

// Make this matrix a scaling in X, Y and Z by the values provided in the given vector
void CMatrix4x4::MakeScaling( const CVector3& scale )
{
	e00 = scale.x;
	e01 = 0.0f;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = scale.y;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = scale.z;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}

// Make this matrix a uniform scaling by the given amount
void CMatrix4x4::MakeScaling( const TFloat32 fScale )
{
	e00 = fScale;
	e01 = 0.0f;
	e02 = 0.0f;
	e03 = 0.0f;

	e10 = 0.0f;
	e11 = fScale;
	e12 = 0.0f;
	e13 = 0.0f;

	e20 = 0.0f;
	e21 = 0.0f;
	e22 = fScale;
	e23 = 0.0f;

	e30 = 0.0f;
	e31 = 0.0f;
	e32 = 0.0f;
	e33 = 1.0f;
}


/*-----------------------------------------------------------------------------------------
	Transformation Matrices - non-member functions
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value). Can be used
// as temporaries in calculations, e.g.
//     CMatrix4x4 m = MatrixScaling( 3.0f ) * MatrixTranslation( CVector3(10.0f, -10.0f, 20.0f) );

// Return an identity matrix - non-member function
CMatrix4x4 MatrixIdentity()
{
	CMatrix4x4 m;

	m.e00 = 1.0f;
	m.e01 = 0.0f;
	m.e02 = 0.0f; 
	m.e03 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = 1.0f;
	m.e12 = 0.0f;
	m.e13 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;
}

// Return a translation matrix of the given vector - non-member function
CMatrix4x4 MatrixTranslation( const CVector3& translate )
{
	CMatrix4x4 m;

	m.e00 = 1.0f;
	m.e01 = 0.0f;
	m.e02 = 0.0f;
	m.e03 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = 1.0f;
	m.e12 = 0.0f;
	m.e13 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;
	m.e23 = 0.0f;

	m.e30 = translate.x;
	m.e31 = translate.y;
	m.e32 = translate.z;
	m.e33 = 1.0f;

	return m;
}


// Return an X-axis rotation matrix of the given angle - non-member function
CMatrix4x4 MatrixRotationX( const TFloat32 x )
{
	CMatrix4x4 m;

	TFloat32 sX, cX;
	SinCos( x, &sX, &cX );

	m.e00 = 1.0f;
	m.e01 = 0.0f;
	m.e02 = 0.0f;
	m.e03 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = cX;
	m.e12 = sX;
	m.e13 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = -sX;
	m.e22 = cX;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;
}

// Return a Y-axis rotation matrix of the given angle - non-member function
CMatrix4x4 MatrixRotationY( const TFloat32 y )
{
	CMatrix4x4 m;

	TFloat32 sY, cY;
	SinCos( y, &sY, &cY );

	m.e00 = cY;
	m.e01 = 0.0f;
	m.e02 = -sY;
	m.e03 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = 1.0f;
	m.e12 = 0.0f;
	m.e13 = 0.0f;

	m.e20 = sY;
	m.e21 = 0.0f;
	m.e22 = cY;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;    

	return m;
}

// Return a Z-axis rotation matrix of the given angle - non-member function
CMatrix4x4 MatrixRotationZ( const TFloat32 z )
{
	CMatrix4x4 m;

	TFloat32 sZ, cZ;
	SinCos( z, &sZ, &cZ );

	m.e00 = cZ;
	m.e01 = sZ;
	m.e02 = 0.0f;
	m.e03 = 0.0f;

	m.e10 = -sZ;
	m.e11 = cZ;
	m.e12 = 0.0f;
	m.e13 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = 1.0f;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;
}

// Make a matrix that is a combined rotation around each axis of the given angles (radians),
// applied in the order specified - non-member function
CMatrix4x4 MatrixRotation
( 
	const CVector3       angles,
	const ERotationOrder eRotOrder /*= kZXY*/
)
{
	GEN_GUARD;

	CMatrix4x4 m;

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
			m.e03 = 0.0f;

			m.e10 = -sZ * cY;
			m.e11 = cZ * cX + -sZ * sY * sX;
			m.e12 = cZ * sX + sZ * sY * cX;
			m.e13 = 0.0f;

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
			m.e03 = 0.0f;

			m.e10 = -sZ;
			m.e11 = cZ * cX;
			m.e12 = cZ * sX;
			m.e13 = 0.0f;

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
			m.e03 = 0.0f;

			m.e10 = -cX * sZ * cY + sX * sY;
			m.e11 = cX * cZ;
			m.e12 = cX * sZ * sY + sX * cY;
			m.e13 = 0.0f;

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
			m.e03 = 0.0f;

			m.e10 = -sZ * cY + cZ * sX * sY;
			m.e11 = cZ * cX;
			m.e12 = sZ * sY + cZ * sX * cY;
			m.e13 = 0.0f;

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
			m.e03 = 0.0f;

			m.e10 = -cX * sZ;
			m.e11 = cX * cZ;
			m.e12 = sX;
			m.e13 = 0.0f;

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
			m.e03 = 0.0f;

			m.e10 = sX * sY * cZ + -cX * sZ;
			m.e11 = sX * sY * sZ + cX * cZ;
			m.e12 = sX * cY;
			m.e13 = 0.0f;

			m.e20 = cX * sY * cZ + sX * sZ;
			m.e21 = cX * sY * sZ + -sX * cZ;
			m.e22 = cX * cY;
			break;
		}

		default:
			GEN_ERROR( "Invalid parameter" );
	}
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;

	GEN_ENDGUARD;
}

// Return a matrix that is a rotation around the given axis of the given angle (radians)
// Non-member function
CMatrix4x4 MatrixRotation
(
	const CVector3& axis,
	const TFloat32  fAngle
)
{
	GEN_GUARD;

	CMatrix4x4 m;

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
	m.e03 = 0.0f;

	m.e10 = txy - sz; 
	m.e11 = ty * axisNorm.y + c;
	m.e12 = tyz + sx;
	m.e13 = 0.0f;

	m.e20 = tzx + sy;
	m.e21 = tyz - sx;
	m.e22 = tz * axisNorm.z + c;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;

	GEN_ENDGUARD;
}


// Return a matrix that is a scaling in X,Y and Z of the values provided in the given vector
// Non-member function
CMatrix4x4 MatrixScaling( const CVector3& scale )
{
	CMatrix4x4 m;

	m.e00 = scale.x;
	m.e01 = 0.0f;
	m.e02 = 0.0f;
	m.e03 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = scale.y;
	m.e12 = 0.0f;
	m.e13 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = scale.z;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;
}

// Return a matrix that is a uniform scaling of the given amount - non-member function
CMatrix4x4 MatrixScaling( const TFloat32 fScale )
{
	CMatrix4x4 m;

	m.e00 = fScale;
	m.e01 = 0.0f;
	m.e02 = 0.0f;
	m.e03 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = fScale;
	m.e12 = 0.0f;
	m.e13 = 0.0f;

	m.e20 = 0.0f;
	m.e21 = 0.0f;
	m.e22 = fScale;
	m.e23 = 0.0f;

	m.e30 = 0.0f;
	m.e31 = 0.0f;
	m.e32 = 0.0f;
	m.e33 = 1.0f;

	return m;
}


/*-----------------------------------------------------------------------------------------
	Facing Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix an affine 3D transformation matrix to face from current position to given
// target (in the Z direction). Can pass up vector for the constructed matrix and specify
// handedness (right-handed Z axis will face away from target)
// Will retain the matrix's current scaling
void CMatrix4x4::FaceTarget
(
	const CVector3& target,
	const CVector3& up /*= CVector3::kYAxis*/,
	const bool      bLH /*= true*/
)
{
	// Use cross product of target direction and up vector to give third axis, then orthogonalise
	CVector3 axisX, axisY, axisZ;
	if (bLH)
	{
		axisZ = Normalise( target - GetPosition() );
		if (axisZ.IsZero()) return;
		axisX = Normalise( Cross( up, axisZ ) );
		if (axisX.IsZero()) return;
		axisY = Cross( axisZ, axisX ); // Will already be normalised
	}
	else
	{
		axisZ = Normalise( GetPosition() - target );
		if (axisZ.IsZero()) return;
		axisX = Normalise( Cross( axisZ, up ) );
		if (axisX.IsZero()) return;
		axisY = Cross( axisX, axisZ ); // Will already be normalised
	}

	// Set rows of matrix, restoring existing scale. Position will be unchanged, 4th column
	// taken from unit matrix
	CVector3 scale = GetScale();
	SetRow( 0, axisX * scale.x );
	SetRow( 1, axisY * scale.y );
	SetRow( 2, axisZ * scale.z );
}

// Create affine 3D transformation matrix to face from given position to given target (in the Z
// direction). Can pass up vector for the constructed matrix and specify handedness (right-handed
// Z axis will face away from target)
CMatrix4x4 MatrixFaceTarget
(
	const CVector3& position,
	const CVector3& target,
	const CVector3& up /*= CVector3::kYAxis*/,
	const bool      bLH /*= true*/
)
{
	// Use cross product of target direction and up vector to give third axis, then orthogonalise
	CVector3 axisX, axisY, axisZ;
	if (bLH)
	{
		axisZ = Normalise( target - position );
		if (axisZ.IsZero()) return CMatrix4x4( position );
		axisX = Normalise( Cross( up, axisZ ) );
		if (axisX.IsZero()) return CMatrix4x4( position );
		axisY = Cross( axisZ, axisX ); // Will already be normalised
	}
	else
	{
		axisZ = Normalise( position - target );
		if (axisZ.IsZero()) return CMatrix4x4( position );
		axisX = Normalise( Cross( axisZ, up ) );
		if (axisX.IsZero()) return CMatrix4x4( position );
		axisY = Cross( axisX, axisZ ); // Will already be normalised
	}
	
	return CMatrix4x4( axisX, axisY, axisZ, position );
}


// Make this matrix an affine 3D transformation matrix that faces in given direction from the
// current position (Z axis). Can pass up vector for the constructed matrix and specify
// handedness (right-handed Z axis will face away from direction)
// Will retain the matrix's current scaling
void CMatrix4x4::FaceDirection
(
	const CVector3& direction,
	const CVector3& up /*= CVector3::kYAxis*/,
	const bool      bLH /*= true*/
)
{
	// Use cross product of target direction and up vector to give third axis, then orthogonalise
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

	// Set rows of matrix, restoring existing scale. Position will be unchanged, 4th column
	// taken from unit matrix
	CVector3 scale = GetScale();
	SetRow( 0, axisX * scale.x );
	SetRow( 1, axisY * scale.y );
	SetRow( 2, axisZ * scale.z );
}

// Create affine 3D transformation matrix that faces in given direction from the given position
// (Z axis). Can pass up vector for the constructed matrix and specify handedness (right-handed
// Z axis will face away from direction)
CMatrix4x4 MatrixFaceDirection
(
	const CVector3& position,
	const CVector3& direction,
	const CVector3& up /*= CVector3::kYAxis*/,
	const bool      bLH /*= true*/
)
{
	// Use cross product of target direction and up vector to give third axis, then orthogonalise
	CVector3 axisX, axisY, axisZ;
	if (bLH)
	{
		axisZ = Normalise( direction );
		if (axisZ.IsZero()) return CMatrix4x4( position );
		axisX = Normalise( Cross( up, axisZ ) );
		if (axisX.IsZero()) return CMatrix4x4( position );
		axisY = Cross( axisZ, axisX ); // Will already be normalised
	}
	else
	{
		axisZ = Normalise( -direction );
		if (axisZ.IsZero()) return CMatrix4x4( position );
		axisX = Normalise( Cross( axisZ, up ) );
		if (axisX.IsZero()) return CMatrix4x4( position );
		axisY = Cross( axisX, axisZ ); // Will already be normalised
	}
	
	return CMatrix4x4( axisX, axisY, axisZ, position );
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
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
)
{
	return AreEqual( m1.e00, m2.e00 ) && AreEqual( m1.e01, m2.e01 ) && 
	       AreEqual( m1.e02, m2.e02 ) && AreEqual( m1.e03, m2.e03 ) && 
		   AreEqual( m1.e10, m2.e10 ) && AreEqual( m1.e11, m2.e11 ) && 
	       AreEqual( m1.e12, m2.e12 ) && AreEqual( m1.e13, m2.e13 ) && 
	       AreEqual( m1.e20, m2.e20 ) && AreEqual( m1.e21, m2.e21 ) && 
	       AreEqual( m1.e22, m2.e22 ) && AreEqual( m1.e23, m2.e23 ) && 
	       AreEqual( m1.e30, m2.e30 ) && AreEqual( m1.e31, m2.e31 ) && 
	       AreEqual( m1.e32, m2.e32 ) && AreEqual( m1.e33, m2.e33 );
}

// Matrix inequality - non-member function
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator!=
(
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
)
{
	return !AreEqual( m1.e00, m2.e00 ) || !AreEqual( m1.e01, m2.e01 ) || 
	       !AreEqual( m1.e02, m2.e02 ) || !AreEqual( m1.e03, m2.e03 ) || 
		   !AreEqual( m1.e10, m2.e10 ) || !AreEqual( m1.e11, m2.e11 ) || 
	       !AreEqual( m1.e12, m2.e12 ) || !AreEqual( m1.e13, m2.e13 ) || 
	       !AreEqual( m1.e20, m2.e20 ) || !AreEqual( m1.e21, m2.e21 ) || 
	       !AreEqual( m1.e22, m2.e22 ) || !AreEqual( m1.e23, m2.e23 ) || 
	       !AreEqual( m1.e30, m2.e30 ) || !AreEqual( m1.e31, m2.e31 ) || 
	       !AreEqual( m1.e32, m2.e32 ) || !AreEqual( m1.e33, m2.e33 );
}


//////////////////////////////////
// Scalar multiplication/division

// Scalar multiplication
CMatrix4x4& CMatrix4x4::operator*=( const TFloat32 s )
{
	e00 *= s;
	e01 *= s;
	e02 *= s;
	e03 *= s;
	e10 *= s;
	e11 *= s;
	e12 *= s;
	e13 *= s;
	e20 *= s;
	e21 *= s;
	e22 *= s;
	e23 *= s;
	e30 *= s;
	e31 *= s;
	e32 *= s;
	e33 *= s;

	return *this;
}

// Matrix-scalar multiplication - non-member version
CMatrix4x4 operator*
(
	const CMatrix4x4& m,
	const TFloat32    s
)
{
	CMatrix4x4 mOut;
    mOut.e00 = m.e00 * s;
    mOut.e01 = m.e01 * s;
    mOut.e02 = m.e02 * s;
    mOut.e03 = m.e03 * s;
    mOut.e10 = m.e10 * s;
    mOut.e11 = m.e11 * s;
    mOut.e12 = m.e12 * s;
    mOut.e13 = m.e13 * s;
    mOut.e20 = m.e20 * s;
    mOut.e21 = m.e21 * s;
    mOut.e22 = m.e22 * s;
    mOut.e23 = m.e23 * s;
    mOut.e30 = m.e30 * s;
    mOut.e31 = m.e31 * s;
    mOut.e32 = m.e32 * s;
    mOut.e33 = m.e33 * s;

	return mOut;
}

// Scalar-matrix multiplication - non-member version
CMatrix4x4 operator*
(
	const TFloat32    s,
	const CMatrix4x4& m
)
{
	CMatrix4x4 mOut;
    mOut.e00 = m.e00 * s;
    mOut.e01 = m.e01 * s;
    mOut.e02 = m.e02 * s;
    mOut.e03 = m.e03 * s;
    mOut.e10 = m.e10 * s;
    mOut.e11 = m.e11 * s;
    mOut.e12 = m.e12 * s;
    mOut.e13 = m.e13 * s;
    mOut.e20 = m.e20 * s;
    mOut.e21 = m.e21 * s;
    mOut.e22 = m.e22 * s;
    mOut.e23 = m.e23 * s;
    mOut.e30 = m.e30 * s;
    mOut.e31 = m.e31 * s;
    mOut.e32 = m.e32 * s;
    mOut.e33 = m.e33 * s;

	return mOut;
}


// Scalar division
CMatrix4x4& CMatrix4x4::operator/=(	const TFloat32 s )
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	e00 /= s;
	e01 /= s;
	e02 /= s;
	e03 /= s;
	e10 /= s;
	e11 /= s;
	e12 /= s;
	e13 /= s;
	e20 /= s;
	e21 /= s;
	e22 /= s;
	e23 /= s;
	e30 /= s;
	e31 /= s;
	e32 /= s;
	e33 /= s;

	return *this;

	GEN_ENDGUARD_OPT;
}

// Matrix-scalar division - non-member version
CMatrix4x4 operator/
(
	const CMatrix4x4& m,
	const TFloat32    s
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	CMatrix4x4 mOut;
    mOut.e00 = m.e00 / s;
    mOut.e01 = m.e01 / s;
    mOut.e02 = m.e02 / s;
    mOut.e03 = m.e03 / s;
    mOut.e10 = m.e10 / s;
    mOut.e11 = m.e11 / s;
    mOut.e12 = m.e12 / s;
    mOut.e13 = m.e13 / s;
    mOut.e20 = m.e20 / s;
    mOut.e21 = m.e21 / s;
    mOut.e22 = m.e22 / s;
    mOut.e23 = m.e23 / s;
	mOut.e30 = m.e30 / s;
    mOut.e31 = m.e31 / s;
    mOut.e32 = m.e32 / s;
    mOut.e33 = m.e33 / s;

	return mOut;

	GEN_ENDGUARD_OPT;
}


///////////////////////////////
// Vector multiplication

// Vector-matrix multiplication (order is important - this is usual order for transformation
// for matrices stored as row vectors - see notes at top of header)
CVector4 operator*
(
	const CVector4&   v,
	const CMatrix4x4& m
)
{
    CVector4 vOut;
    vOut.x = v.x*m.e00 + v.y*m.e10 + v.z*m.e20 + v.w*m.e30;
    vOut.y = v.x*m.e01 + v.y*m.e11 + v.z*m.e21 + v.w*m.e31;
    vOut.z = v.x*m.e02 + v.y*m.e12 + v.z*m.e22 + v.w*m.e32;
    vOut.w = v.x*m.e03 + v.y*m.e13 + v.z*m.e23 + v.w*m.e33;

    return vOut;
}

// Matrix-vector multiplication (order is important - this is an unusual order for matrices
// stored as row vectors - see notes at top of header)
CVector4 operator*
(
	const CMatrix4x4& m,
	const CVector4&   v
)
{
    CVector4 vOut;
    vOut.x = m.e00*v.x + m.e01*v.y + m.e02*v.z + m.e03*v.w;
    vOut.y = m.e10*v.x + m.e11*v.y + m.e12*v.z + m.e13*v.w;
    vOut.z = m.e20*v.x + m.e21*v.y + m.e22*v.z + m.e23*v.w;
    vOut.w = m.e30*v.x + m.e31*v.y + m.e32*v.z + m.e33*v.w;

    return vOut;
}


// Return the given vector transformed by this matrix (pre-multiplication: V' = V*M)
CVector4 CMatrix4x4::Transform(	const CVector4& v ) const
{
	CVector4 vOut;
	vOut.x = v.x*e00 + v.y*e10 + v.z*e20 + v.w*e30;
	vOut.y = v.x*e01 + v.y*e11 + v.z*e21 + v.w*e31;
	vOut.z = v.x*e02 + v.y*e12 + v.z*e22 + v.w*e32;
	vOut.w = v.x*e03 + v.y*e13 + v.z*e23 + v.w*e33;

	return vOut;
}

// Return the given CVector3 transformed by this matrix (pre-multiplication: V' = V*M)
// Assuming it is a vector rather then a point, i.e. assume the vector's 4th element is 0
CVector3 CMatrix4x4::TransformVector( const CVector3& v ) const
{
	CVector3 vOut;
	vOut.x = v.x*e00 + v.y*e10 + v.z*e20;
	vOut.y = v.x*e01 + v.y*e11 + v.z*e21;
	vOut.z = v.x*e02 + v.y*e12 + v.z*e22;

	return vOut;
}

// Return the given CVector3 transformed by this matrix (pre-multiplication: V' = V*M)
// Assuming it is a point rather then a vector, i.e. assume the vector's 4th element is 1
CVector3 CMatrix4x4::TransformPoint( const CVector3& p ) const
{
	CVector3 pOut;
	pOut.x = p.x*e00 + p.y*e10 + p.z*e20 + e30;
	pOut.y = p.x*e01 + p.y*e11 + p.z*e21 + e31;
	pOut.z = p.x*e02 + p.y*e12 + p.z*e22 + e32;

	return pOut;
}


///////////////////////////////
// Matrix multiplication

// Post-multiply this matrix by the given one
CMatrix4x4& CMatrix4x4::operator*=( const CMatrix4x4& m )
{
	if ( this == &m )
	{
		// Special case of multiplying by self - no copy optimisations so use binary version
		*this = m * m;
	}
	else
	{
		TFloat32 t0, t1, t2;

		t0  = e00*m.e00 + e01*m.e10 + e02*m.e20 + e03*m.e30;
		t1  = e00*m.e01 + e01*m.e11 + e02*m.e21 + e03*m.e31;
		t2  = e00*m.e02 + e01*m.e12 + e02*m.e22 + e03*m.e32;
		e03 = e00*m.e03 + e01*m.e13 + e02*m.e23 + e03*m.e33;
		e00 = t0;
		e01 = t1;
		e02 = t2;

		t0  = e10*m.e00 + e11*m.e10 + e12*m.e20 + e13*m.e30;
		t1  = e10*m.e01 + e11*m.e11 + e12*m.e21 + e13*m.e31;
		t2  = e10*m.e02 + e11*m.e12 + e12*m.e22 + e13*m.e32;
		e13 = e10*m.e03 + e11*m.e13 + e12*m.e23 + e13*m.e33;
		e10 = t0;
		e11 = t1;
		e12 = t2;

		t0  = e20*m.e00 + e21*m.e10 + e22*m.e20 + e23*m.e30;
		t1  = e20*m.e01 + e21*m.e11 + e22*m.e21 + e23*m.e31;
		t2  = e20*m.e02 + e21*m.e12 + e22*m.e22 + e23*m.e32;
		e23 = e20*m.e03 + e21*m.e13 + e22*m.e23 + e23*m.e33;
		e20 = t0;
		e21 = t1;
		e22 = t2;

		t0  = e30*m.e00 + e31*m.e10 + e32*m.e20 + e33*m.e30;
		t1  = e30*m.e01 + e31*m.e11 + e32*m.e21 + e33*m.e31;
		t2  = e30*m.e02 + e31*m.e12 + e32*m.e22 + e33*m.e32;
		e33 = e30*m.e03 + e31*m.e13 + e32*m.e23 + e33*m.e33;
		e30 = t0;
		e31 = t1;
		e32 = t2;
	}
	return *this;
}


// Binary matrix-matrix multiplication - non-member version
CMatrix4x4 operator*
(
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
)
{
	CMatrix4x4 mOut;

	mOut.e00 = m1.e00*m2.e00 + m1.e01*m2.e10 + m1.e02*m2.e20 + m1.e03*m2.e30;
	mOut.e01 = m1.e00*m2.e01 + m1.e01*m2.e11 + m1.e02*m2.e21 + m1.e03*m2.e31;
	mOut.e02 = m1.e00*m2.e02 + m1.e01*m2.e12 + m1.e02*m2.e22 + m1.e03*m2.e32;
	mOut.e03 = m1.e00*m2.e03 + m1.e01*m2.e13 + m1.e02*m2.e23 + m1.e03*m2.e33;

	mOut.e10 = m1.e10*m2.e00 + m1.e11*m2.e10 + m1.e12*m2.e20 + m1.e13*m2.e30;
	mOut.e11 = m1.e10*m2.e01 + m1.e11*m2.e11 + m1.e12*m2.e21 + m1.e13*m2.e31;
	mOut.e12 = m1.e10*m2.e02 + m1.e11*m2.e12 + m1.e12*m2.e22 + m1.e13*m2.e32;
	mOut.e13 = m1.e10*m2.e03 + m1.e11*m2.e13 + m1.e12*m2.e23 + m1.e13*m2.e33;

	mOut.e20 = m1.e20*m2.e00 + m1.e21*m2.e10 + m1.e22*m2.e20 + m1.e23*m2.e30;
	mOut.e21 = m1.e20*m2.e01 + m1.e21*m2.e11 + m1.e22*m2.e21 + m1.e23*m2.e31;
	mOut.e22 = m1.e20*m2.e02 + m1.e21*m2.e12 + m1.e22*m2.e22 + m1.e23*m2.e32;
	mOut.e23 = m1.e20*m2.e03 + m1.e21*m2.e13 + m1.e22*m2.e23 + m1.e23*m2.e33;

	mOut.e30 = m1.e30*m2.e00 + m1.e31*m2.e10 + m1.e32*m2.e20 + m1.e33*m2.e30;
	mOut.e31 = m1.e30*m2.e01 + m1.e31*m2.e11 + m1.e32*m2.e21 + m1.e33*m2.e31;
	mOut.e32 = m1.e30*m2.e02 + m1.e31*m2.e12 + m1.e32*m2.e22 + m1.e33*m2.e32;
	mOut.e33 = m1.e30*m2.e03 + m1.e31*m2.e13 + m1.e32*m2.e23 + m1.e33*m2.e33;

	return mOut;
}


// Post-multiply this matrix by the given one assuming they are both affine
CMatrix4x4& CMatrix4x4::MultiplyAffine( const CMatrix4x4& m )
{
	if ( this == &m )
	{
		// Special case of multiplying by self - no copy optimisations so use binary version
		*this = gen::MultiplyAffine( m, m );
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

		t0  = e30*m.e00 + e31*m.e10 + e32*m.e20 + m.e30;
		t1  = e30*m.e01 + e31*m.e11 + e32*m.e21 + m.e31;
		e32 = e30*m.e02 + e31*m.e12 + e32*m.e22 + m.e32;
		e30 = t0;
		e31 = t1;
	}

	return *this;
}

// Binary matrix-matrix multiplication assuming both matrices are affine - non-member version
CMatrix4x4 MultiplyAffine
(
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
)
{
	CMatrix4x4 mOut;

	mOut.e00 = m1.e00*m2.e00 + m1.e01*m2.e10 + m1.e02*m2.e20;
	mOut.e01 = m1.e00*m2.e01 + m1.e01*m2.e11 + m1.e02*m2.e21;
	mOut.e02 = m1.e00*m2.e02 + m1.e01*m2.e12 + m1.e02*m2.e22;
	mOut.e03 = 0.0f;

	mOut.e10 = m1.e10*m2.e00 + m1.e11*m2.e10 + m1.e12*m2.e20;
	mOut.e11 = m1.e10*m2.e01 + m1.e11*m2.e11 + m1.e12*m2.e21;
	mOut.e12 = m1.e10*m2.e02 + m1.e11*m2.e12 + m1.e12*m2.e22;
	mOut.e13 = 0.0f;

	mOut.e20 = m1.e20*m2.e00 + m1.e21*m2.e10 + m1.e22*m2.e20;
	mOut.e21 = m1.e20*m2.e01 + m1.e21*m2.e11 + m1.e22*m2.e21;
	mOut.e22 = m1.e20*m2.e02 + m1.e21*m2.e12 + m1.e22*m2.e22;
	mOut.e23 = 0.0f;

	mOut.e30 = m1.e30*m2.e00 + m1.e31*m2.e10 + m1.e32*m2.e20 + m2.e30;
	mOut.e31 = m1.e30*m2.e01 + m1.e31*m2.e11 + m1.e32*m2.e21 + m2.e31;
	mOut.e32 = m1.e30*m2.e02 + m1.e31*m2.e12 + m1.e32*m2.e22 + m2.e32;
	mOut.e33 = 1.0f;

	return mOut;
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard matrices
const CMatrix4x4 CMatrix4x4::kIdentity(1.0f, 0.0f, 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f,
                                       0.0f, 0.0f, 0.0f, 1.0f);


} // namespace gen
