/**************************************************************************************************
	Module:       CMatrix2x2.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Definition of the concrete class CMatrix2x2, a 2x2 matrix of 32-bit floats. Designed for
	transformation matrices for 2D graphics

	See notes at top of header for some background to this class

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "CMatrix2x2.h"

#include "Error.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/

// Construct by value
CMatrix2x2::CMatrix2x2
(
	const TFloat32 elt00, const TFloat32 elt01,
	const TFloat32 elt10, const TFloat32 elt11
)
{
	e00 = elt00;
	e01 = elt01;

	e10 = elt10;
	e11 = elt11;
}

// Construct through pointer to 4 floats, may specify row/column order of data
CMatrix2x2::CMatrix2x2
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

		e10 = pfElts[2];
		e11 = pfElts[3];
	}
	else
	{
		e00 = pfElts[0];
		e10 = pfElts[1];

		e01 = pfElts[2];
		e11 = pfElts[3];
	}

	GEN_ENDGUARD_OPT;
}

// Construct by row or column using CVector2's, may specify if setting rows or columns
CMatrix2x2::CMatrix2x2
(
	const CVector2& v0,
	const CVector2& v1,
	const bool      bRows /*= true*/
)
{
	if (bRows)
	{
		e00 = v0.x;
		e01 = v0.y;

		e10 = v1.x;
		e11 = v1.y;
	}
	else
	{
		e00 = v0.x;
		e10 = v0.y;

		e01 = v1.x;
		e11 = v1.y;
	}
}


// Construct matrix transformation from rotation angle and optional scaling. Matrix is
// effectively built in this order: M = Scale*Rotation
CMatrix2x2::CMatrix2x2
(
	const TFloat32  fAngle,
	const CVector2& scale /*= CVector2::kOne*/
)
{
	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	e00 = c * scale.x;
	e01 = s * scale.x;

	e10 = -s * scale.y;
	e11 = c * scale.y;
}


// Copy constructor
CMatrix2x2::CMatrix2x2( const CMatrix2x2& m )
{
	e00 = m.e00;
	e01 = m.e01;

	e10 = m.e10;
	e11 = m.e11;
}

// Assignment operator
CMatrix2x2& CMatrix2x2::operator=( const CMatrix2x2& m )
{
	if ( this != &m )
	{
		e00 = m.e00;
		e01 = m.e01;

		e10 = m.e10;
		e11 = m.e11;
	}
	return *this;
}


/*-----------------------------------------------------------------------------------------
	Setters
-----------------------------------------------------------------------------------------*/

// Set by value
void CMatrix2x2::Set
(
	const TFloat32 elt00, const TFloat32 elt01,
	const TFloat32 elt10, const TFloat32 elt11
)
{
	e00 = elt00;
	e01 = elt01;

	e10 = elt10;
	e11 = elt11;
}

// Set through pointer to 4 floats, may specify column/row order of data
void CMatrix2x2::Set
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

		e10 = pfElts[2];
		e11 = pfElts[3];
	}
	else
	{
		e00 = pfElts[0];
		e10 = pfElts[1];

		e01 = pfElts[2];
		e11 = pfElts[3];
	}

	GEN_ENDGUARD_OPT;
}


/*-----------------------------------------------------------------------------------------
	Row/column getters & setters
-----------------------------------------------------------------------------------------*/

// Get a single row (range 0-1) of the matrix
CVector2 CMatrix2x2::GetRow( const TUInt32 iRow ) const
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 2, "Invalid parameter" );

	return CVector2( &e00 + iRow * 2 );

	GEN_ENDGUARD_OPT;
}

// Get a single column (range 0-1) of the matrix
CVector2 CMatrix2x2::GetColumn( const TUInt32 iCol ) const
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 2, "Invalid parameter" );

	const TFloat32* pfElts = &e00 + iCol;
	return CVector2( pfElts[0], pfElts[2] );

	GEN_ENDGUARD_OPT;
}


// Set a single row (range 0-1) of the matrix
void CMatrix2x2::SetRow
(
	const TUInt32   iRow,
	const CVector2& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iRow < 2, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iRow * 2;
	pfElts[0] = v.x;
	pfElts[1] = v.y;

	GEN_ENDGUARD_OPT;
}

// Set a single column (range 0-1) of the matrix
void CMatrix2x2::SetColumn
(
	const TUInt32   iCol,
	const CVector2& v
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( iCol < 2, "Invalid parameter" );

	TFloat32* pfElts = &e00 + iCol;
	pfElts[0] = v.x;
	pfElts[2] = v.y;

	GEN_ENDGUARD_OPT;
}


// Set all rows of the matrix at once
void CMatrix2x2::SetRows
(
	const CVector2& v0,
	const CVector2& v1
)
{
	e00 = v0.x;
	e01 = v0.y;

	e10 = v1.x;
	e11 = v1.y;
}

// Set all columns of the matrix at once
void CMatrix2x2::SetColumns
(
	const CVector2& v0,
	const CVector2& v1
)
{
	e00 = v0.x;
	e10 = v0.y;

	e01 = v1.x;
	e11 = v1.y;
}


/*-----------------------------------------------------------------------------------------
	Creation and Decomposition of Transformation Matrices
-----------------------------------------------------------------------------------------*/

// Make matrix a transformation using Euler angles & optional scaling, with remaining elements
// taken from the identity matrix. May specify order to apply rotations. Matrix is built in
// this order: M = Scale*Rotation
void CMatrix2x2::MakeTransform
(
	const TFloat32  fAngle,
	const CVector2& scale /*= CVector2::kOne*/
)
{
	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	e00 = c * scale.x;
	e01 = s * scale.x;

	e10 = -s * scale.y;
	e11 = c * scale.y;
}


// Decompose transformation into angle of rotation and scale. Pass NULL for any unneeded
// parameters. Assumes matrix is built in this order: M = Scale*Rotation
void CMatrix2x2::DecomposeTransform
(
	TFloat32* pfAngle,
	CVector2* pScale
) const
{
	GEN_GUARD;

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

// Get the X & Y scaling of a 2D affine transformation matrix
CVector2 CMatrix2x2::GetScale() const
{
	CVector2 scale;
	scale.x = Sqrt( e00*e00 + e01*e01 );
	scale.y = Sqrt( e10*e10 + e11*e11 );
	return scale;
}


// Set the X scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (rotation)
void CMatrix2x2::SetScaleX( const TFloat32 x )
{
	TFloat32 scaleSq = Sqrt( e00*e00 + e01*e01 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = x * InvSqrt( scaleSq );
		e00 *= rescale;
		e01 *= rescale;
	}
}	

// Set the X scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (rotation)
void CMatrix2x2::SetScaleY( const TFloat32 y )
{
	TFloat32 scaleSq = Sqrt( e10*e10 + e11*e11 );
	if (!IsZero(scaleSq)) // No effect on zero axes
	{
		TFloat32 rescale = y * InvSqrt( scaleSq );
		e10 *= rescale;
		e11 *= rescale;
	}
}	

// Set the X & Y scaling of a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (rotation)
void CMatrix2x2::SetScale( const CVector2& scale )
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

// Set a uniform scaling for a transformation matrix. Removes any previous scaling.
// Will not change other components of the transformation (rotation)
void CMatrix2x2::SetScale( const TFloat32 fScale )
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
bool CMatrix2x2::IsIdentity() const
{
	return IsZero( e00 - 1.0f ) && IsZero( e11 - 1.0f ) && IsZero( e01 ) && IsZero( e10 );
}


/*-----------------------------------------------------------------------------------------
	Othogonality
-----------------------------------------------------------------------------------------*/

// See extensive notes in header file

// Test if matrix has orthogonal rows, i.e. if the two rows are vectors at right angles to
// each other. Will also be orthonormal if it contains no scaling
bool CMatrix2x2::IsOrthogonal() const
{
	// Check dot product of the rows is zero
	return IsZero( e00*e10 + e01*e11 );
}

// Test if matrix has orthonormal rows, i.e. if the two rows are *normals* at right angles to
// each other. Scaled matrices cannot be orthonormal (can be orthogonal)
bool CMatrix2x2::IsOrthonormal() const
{
	// Check each row is length 1 and dot product the rows is zero
	return IsZero( e00*e00 + e01*e01 - 1.0f ) && IsZero( e10*e10 + e11*e11 - 1.0f ) &&
	       IsZero( e00*e10 + e01*e11 );
}

// Orthogonalise the rows/columns of the matrix. Generally used to "correct" matrices that
// become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
void CMatrix2x2::Orthogonalise(	const CVector2& scale /*= CVector2::kOne*/ )
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

// Return a copy of given matrix with orthogonalised rows/columns. Generally used to "correct"
// matrices that become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
CMatrix2x2 Orthogonalise
(
	const CMatrix2x2& m,
	const CVector2&   scale /*= CVector2::kOne*/
)
{
	GEN_GUARD;

	CMatrix2x2 mOut;

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
void CMatrix2x2::Transpose()
{
	TFloat32 t;
	t   = e01;
	e01 = e10;
	e10 = t;
}

// Return the transpose of given matrix (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
CMatrix2x2 Transpose( const CMatrix2x2& m )
{
	CMatrix2x2 transMat;

	transMat.e00 = m.e00;
	transMat.e01 = m.e10;

	transMat.e10 = m.e01;
	transMat.e11 = m.e11;

	return transMat;
}


// Set this matrix to its inverse
void CMatrix2x2::Invert()
{
	GEN_GUARD;

	// Calculate determinant
	TFloat32 det = e00*e11 - e01*e10;
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Calculate inverse
	TFloat32 invDet = 1.0f / det;
	TFloat32 t;
	t   = invDet * e00;
	e00 = invDet * e11;
	e11 = t;

	e01 *= invDet;
	e10 *= invDet;

	GEN_ENDGUARD;
}

// Return the inverse of given matrix
CMatrix2x2 Inverse( const CMatrix2x2& m )
{
	GEN_GUARD;

	CMatrix2x2 mOut;

	// Calculate determinant of upper left 2x2
	TFloat32 det = m.e00*m.e11 - m.e01*m.e10;
	GEN_ASSERT( !IsZero(det), "Singular matrix" );

	// Calculate inverse of upper left 2x2
	TFloat32 invDet = 1.0f / det;
	mOut.e00 = invDet * m.e11;
	mOut.e01 = invDet * m.e01;

	mOut.e10 = invDet * m.e10;
	mOut.e11 = invDet * m.e00;

	return mOut;

	GEN_ENDGUARD;
}


/*-----------------------------------------------------------------------------------------
	Transformation Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix the identity matrix
void CMatrix2x2::MakeIdentity()
{
	e00 = 1.0f;
	e01 = 0.0f;

	e10 = 0.0f;
	e11 = 1.0f;
}


// Make this matrix a rotation of the given angle (radians)
void CMatrix2x2::MakeRotation( const TFloat32 fAngle )
{
	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	e00 = c;
	e01 = s;

	e10 = -s;
	e11 = c;
}


// Make this matrix a scaling in X & Y by the values provided in the given vector
void CMatrix2x2::MakeScaling( const CVector2& scale )
{
	e00 = scale.x;
	e01 = 0.0f;

	e10 = 0.0f;
	e11 = scale.y;
}

// Make this matrix a uniform scaling of the given amount
void CMatrix2x2::MakeScaling( const TFloat32 fScale )
{
	e00 = fScale;
	e01 = 0.0f;

	e10 = 0.0f;
	e11 = fScale;
}


/*-----------------------------------------------------------------------------------------
	Transformation Matrices - non-member functions
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value). Can be used
// as temporaries in calculations, e.g.
//     CMatrix2x2 m = MatrixScaling( 3.0f ) * MatrixRotation( ToRadians(45.0f) );

// Return an identity matrix
CMatrix2x2 Matrix2x2Identity()
{
	CMatrix2x2 m;

	m.e00 = 1.0f;
	m.e01 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = 1.0f;

	return m;
}

// Return a matrix that is a rotation of the given angle (radians)
CMatrix2x2 Matrix2x2Rotation( const TFloat32 fAngle )
{
	CMatrix2x2 m;

	TFloat32 s, c;
	SinCos( fAngle, &s, &c );

	m.e00 = c;
	m.e01 = s;

	m.e10 = -s;
	m.e11 = c;

	return m;
}


// Return a matrix that is a scaling in X and Y by the values provided in the given vector
CMatrix2x2 Matrix2x2Scaling( const CVector2& scale )
{
	CMatrix2x2 m;

	m.e00 = scale.x;
	m.e01 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = scale.y;

	return m;
}

// Return a matrix that is a uniform scaling of the given amount
CMatrix2x2 Matrix2x2Scaling( const TFloat32 fScale )
{
	CMatrix2x2 m;

	m.e00 = fScale;
	m.e01 = 0.0f;

	m.e10 = 0.0f;
	m.e11 = fScale;

	return m;
}


/*-----------------------------------------------------------------------------------------
	Facing Matrices
-----------------------------------------------------------------------------------------*/

// Make this matrix face in given direction. Can specify whether to use X or Y axis and
// handedness (defaults: Y, left)
void CMatrix2x2::FaceDirection
(
	const CVector2& direction,
	const bool      bUseYAxis /*= true*/,
	const bool      bLH /*= true*/
)
{
	CVector2 axis = Normalise( direction );
	if (axis.IsZero()) return;
	if (bUseYAxis)
	{
		e10 = axis.x;
		e11 = axis.y;
		if (bLH)
		{
			e00 = e11;
			e01 = -e10;
		}
		else
		{
			e00 = -e11;
			e01 = e10;
		}
	}
	else
	{
		e00 = axis.x;
		e01 = axis.y;
		if (bLH)
		{
			e11 = -e00;
			e10 = e01;
		}
		else
		{
			e11 = e00;
			e10 = -e01;
		}
	}
}

// Return matrix that faces in given direction. Can specify whether to use X or Y axis and
// handedness (defaults: Y, left)
CMatrix2x2 MatrixFaceDirection
(
	const CVector2& direction,
	const bool      bUseYAxis /*= true*/,
	const bool      bLH /*= true*/
)
{
	CMatrix2x2 m;

	CVector2 axis = Normalise( direction );
	if (axis.IsZero()) return CMatrix2x2::kIdentity;
	if (bUseYAxis)
	{
		m.e10 = axis.x;
		m.e11 = axis.y;
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
	}
	else
	{
		m.e00 = axis.x;
		m.e01 = axis.y;
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
	}

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
	const CMatrix2x2& m1,
	const CMatrix2x2& m2
)
{
	return AreEqual( m1.e00, m2.e00 ) && AreEqual( m1.e01, m2.e01 ) &&
		   AreEqual( m1.e10, m2.e10 ) && AreEqual( m1.e11, m2.e11 );
}

// Matrix inequality - non-member function
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator!=
(
	const CMatrix2x2& m1,
	const CMatrix2x2& m2
)
{
	return !AreEqual( m1.e00, m2.e00 ) || !AreEqual( m1.e01, m2.e01 ) &&
		   !AreEqual( m1.e10, m2.e10 ) || !AreEqual( m1.e11, m2.e11 );
}


//////////////////////////////////
// Scalar multiplication/division

// Scalar multiplication
CMatrix2x2& CMatrix2x2::operator*=( const TFloat32 s )
{
	e00 *= s;
	e01 *= s;
	e10 *= s;
	e11 *= s;

	return *this;
}

// Matrix-scalar multiplication - non-member version
CMatrix2x2 operator*
(
	const CMatrix2x2& m,
	const TFloat32    s
)
{
	CMatrix2x2 mOut;
    mOut.e00 = m.e00 * s;
    mOut.e01 = m.e01 * s;
    mOut.e10 = m.e10 * s;
    mOut.e11 = m.e11 * s;

	return mOut;
}

// Scalar-matrix multiplication - non-member version
CMatrix2x2 operator*
(
	const TFloat32    s,
	const CMatrix2x2& m
)
{
	CMatrix2x2 mOut;
    mOut.e00 = m.e00 * s;
    mOut.e01 = m.e01 * s;
    mOut.e10 = m.e10 * s;
    mOut.e11 = m.e11 * s;

	return mOut;
}


// Scalar division
CMatrix2x2& CMatrix2x2::operator/=(	const TFloat32 s )
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	e00 /= s;
	e01 /= s;
	e10 /= s;
	e11 /= s;

	return *this;

	GEN_ENDGUARD_OPT;
}

// Matrix-scalar division - non-member version
CMatrix2x2 operator/
(
	const CMatrix2x2& m,
	const TFloat32    s
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	CMatrix2x2 mOut;
    mOut.e00 = m.e00 / s;
    mOut.e01 = m.e01 / s;
    mOut.e10 = m.e10 / s;
    mOut.e11 = m.e11 / s;

	return mOut;

	GEN_ENDGUARD_OPT;
}


///////////////////////////////
// Vector multiplication

// Vector-matrix multiplication (order is important - this is usual order for transformation
// for matrices stored as row vectors - see notes at top of header)
CVector2 operator*
(
	const CVector2&   v,
	const CMatrix2x2& m
)
{
    CVector2 vOut;
    vOut.x = v.x*m.e00 + v.y*m.e10;
    vOut.y = v.x*m.e01 + v.y*m.e11;

    return vOut;
}

// Matrix-vector multiplication (order is important - this is an unusual order for matrices
// stored as row vectors - see notes at top of header)
CVector2 operator*
(
	const CMatrix2x2& m,
	const CVector2&   v
)
{
    CVector2 vOut;
    vOut.x = m.e00*v.x + m.e01*v.y;
    vOut.y = m.e10*v.x + m.e11*v.y;

    return vOut;
}


// Return the given vector transformed by this matrix (pre-multiplication: V' = V*M)
CVector2 CMatrix2x2::Transform(	const CVector2& v ) const
{
	CVector2 vOut;
	vOut.x = v.x*e00 + v.y*e10;
	vOut.y = v.x*e01 + v.y*e11;

	return vOut;
}


///////////////////////////////
// Matrix multiplication

// Post-multiply this matrix by the given one
CMatrix2x2& CMatrix2x2::operator*=( const CMatrix2x2& m )
{
	if ( this == &m )
	{
		// Special case of multiplying by self - no copy optimisations so use binary version
		*this = m * m;
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
	}
	return *this;
}


// Binary matrix-matrix multiplication - non-member version
CMatrix2x2 operator*
(
	const CMatrix2x2& m1,
	const CMatrix2x2& m2
)
{
	CMatrix2x2 mOut;

	mOut.e00 = m1.e00*m2.e00 + m1.e01*m2.e10;
	mOut.e01 = m1.e00*m2.e01 + m1.e01*m2.e11;

	mOut.e10 = m1.e10*m2.e00 + m1.e11*m2.e10;
	mOut.e11 = m1.e10*m2.e01 + m1.e11*m2.e11;

	return mOut;
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard matrices
const CMatrix2x2 CMatrix2x2::kIdentity(1.0f, 0.0f,
                                       0.0f, 1.0f);


} // namespace gen
