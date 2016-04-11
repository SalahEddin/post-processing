/**************************************************************************************************
	Module:       CMatrix2x2.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Definition of the concrete class CMatrix2x2, a 2x2 matrix of 32-bit floats. Designed for
	transformation matrices for 2D graphics

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

// This API is designed for 2D (non-affine) transformations for 2D graphics. It uses row vectors to
// represent the axes of the transformed space.
//
// A 2D transformation matrix can perform rotation, scaling, reflection and shear, but not
// translation. It is equivalent to the upper-left 2x2 of the 3x3 affine transformations supported
// by the CMatrix3x3 class, see that header file for more detail
//
// Notes:
// - As the matrix is stored in rows, the [] operator is provided to return CVector2 references to
//   the actual matrix data. This is highly convenient/efficient but non-portable,
//   i.e. the [] operator is not guaranteed to work on all compilers (though it will on most)

#ifndef GEN_C_MATRIX_2X2_H_INCLUDED
#define GEN_C_MATRIX_2X2_H_INCLUDED

#include "Defines.h"
#include "BaseMath.h"
#include "CVector2.h"

namespace gen
{

class CMatrix2x2
{
	GEN_CLASS( CMatrix2x2 );

// Concrete class - public access
public:

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised (for performance)
	CMatrix2x2() {}

	// Construct by value
	CMatrix2x2
	(
		const TFloat32 elt00, const TFloat32 elt01,
		const TFloat32 elt10, const TFloat32 elt11
	);

	// Construct through pointer to 4 floats, may specify row/column order of data
	explicit CMatrix2x2
	(
		const TFloat32* pfElts,
		const bool      bRows = true
	);
	// 'explicit' disallows implicit conversion:  TFloat32* pf; CMatrix2x2 m = pf;
	// Need to use this constructor explicitly:   TFloat32* pf; CMatrix2x2 m = CMatrix2x2(pf);
	// Only applies to constructors that can take one parameter, used to avoid confusing code

	// Construct by row or column using CVector2's, may specify if setting rows or columns
    CMatrix2x2
	(
		const CVector2& v0,
		const CVector2& v1,
		const bool      bRows = true
	);


	// Construct matrix transformation from rotation angle and optional scaling. Matrix is
	// effectively built in this order: M = Scale*Rotation
	explicit CMatrix2x2
	(
		const TFloat32  fAngle,
		const CVector2& scale = CVector2::kOne
	);
	// Require explicit conversion from angle only (see above)


	// Copy constructor
    CMatrix2x2( const CMatrix2x2& m );

	// Assignment operator
    CMatrix2x2& operator=( const CMatrix2x2& m );


	/*-----------------------------------------------------------------------------------------
		Setters
	-----------------------------------------------------------------------------------------*/

	// Set by value
	void Set
	(
		const TFloat32 elt00, const TFloat32 elt01,
		const TFloat32 elt10, const TFloat32 elt11
	);

	// Set through pointer to 4 floats, may specify column/row order of data
	void Set
	(
		const TFloat32* pfElts,
		const bool      bRows = true
	);


	/*-----------------------------------------------------------------------------------------
		Row/column getters & setters
	-----------------------------------------------------------------------------------------*/

	// Get a single row (range 0-1) of the matrix
    CVector2 GetRow( const TUInt32 iRow ) const;

	// Get a single column (range 0-1) of the matrix
    CVector2 GetColumn( const TUInt32 iCol ) const;


	// Set a single row (range 0-1) of the matrix
    void SetRow
	(
		const TUInt32   iRow,
		const CVector2& v
	);

	// Set a single column (range 0-1) of the matrix
    void SetColumn
	(
		const TUInt32   iCol,
		const CVector2& v
	);


	// Set all rows of the matrix at once
    void SetRows
	(
		const CVector2& v0,
		const CVector2& v1
	);

	// Set all columns of the matrix at once
    void SetColumns
	(
		const CVector2& v0,
		const CVector2& v1
	);


	/*-----------------------------------------------------------------------------------------
		Non-portable row access
	-----------------------------------------------------------------------------------------*/
	// These reinterpreting functions are not guaranteed to be portable, i.e. may not work on all
	// compilers (but will on most). However, they improve efficiency and are highly convenient

	// Direct access to rows as CVector2 - allows two dimensional subscripting of elements
	// Efficient but non-portable. Index in range 0-1 - no validation of index
    CVector2& operator[]( const TUInt32 iRow )
	{
		return *reinterpret_cast<CVector2*>(&e00 + iRow * 2);
	}

	// Direct access to (const) rows as CVector2 - allows two dimensional subscripting of elements
	// Efficient but non-portable. Index in range 0-1 - no validation of index
    const CVector2& operator[](	const TUInt32 iRow ) const
	{
		return *reinterpret_cast<const CVector2*>(&e00 + iRow * 2);
	}


	/*-----------------------------------------------------------------------------------------
		Creation and Decomposition of Transformation Matrices
	-----------------------------------------------------------------------------------------*/

	// Make matrix a transformation using Euler angles & optional scaling, with remaining elements
	// taken from the identity matrix. May specify order to apply rotations. Matrix is built in
	// this order: M = Scale*Rotation
	void MakeTransform
	(
		const TFloat32  fAngle,
		const CVector2& scale = CVector2::kOne
	);


	// Decompose transformation into angle of rotation and scale. Pass NULL for any unneeded
	// parameters. Assumes matrix is built in this order: M = Scale*Rotation
    void DecomposeTransform
	(
		TFloat32* pfAngle,
		CVector2* pScale
	) const;


	/*-----------------------------------------------------------------------------------------
		Manipulation of Transformation Matrices
	-----------------------------------------------------------------------------------------*/

	// Get the X scaling of a transformation matrix
	TFloat32 GetScaleX() const
	{
		return Sqrt( e00*e00 + e01*e01 );
	}

	// Get the Y scaling of a transformation matrix
	TFloat32 GetScaleY() const
	{
		return Sqrt( e10*e10 + e11*e11 );
	}

	// Get the X & Y scaling of a transformation matrix
    CVector2 GetScale() const;


	// Set the X scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
	void SetScaleX( const TFloat32 x );

	// Set the Y scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
	void SetScaleY( const TFloat32 y );

	// Set the X & Y scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
    void SetScale( const CVector2& scale );

	// Set a uniform scaling for a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
    void SetScale( const TFloat32 fScale );


	// Alter the scale of a transformation matrix in the (local) X direction. The effect is
	// multiplicative, e.g ScaleX( 2.0f ) indicates a doubling of the X scale
    void ScaleX( const TFloat32 x )
	{
		e00 *= x;
		e01 *= x;
	}

	// Alter the scale of a transformation matrix in the (local) Y direction. The effect is
	// multiplicative, e.g ScaleY( 2.0f ) indicates a doubling of the Y scale
    void ScaleY( const TFloat32 y )
	{
		e10 *= y;
		e11 *= y;
	}

	// Alter the scale of a transformation matrix in the (local) X & Y directions. The effect is
	// multiplicative, e.g Scale( CVector3(2.0f, 2.0f, 2.0f) ) indicates a doubling of the scale
	// in all directions
    void Scale( const CVector2 scale )
	{
		e00 *= scale.x;
		e01 *= scale.x;

		e10 *= scale.y;
		e11 *= scale.y;
	}

	// Alter the scale of a transformation matrix uniformly in the (local) X & Y directions. The
	// effect is multiplicative, e.g Scale( 2.0f ) indicates a uniform doubling of the scale
    void Scale( const TFloat32 fScale )
	{
		e00 *= fScale;
		e01 *= fScale;

		e10 *= fScale;
		e11 *= fScale;
	}


	// Rotate a transformation matrix by given angle (in radians)
	void Rotate( const TFloat32 fAngle )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 s, c;
		SinCos( fAngle, &s, &c );
		TFloat32 t;
		t   = e00*s + e01*c;
		e00 = e00*c - e01*s;
		e01 = t;
		t   = e10*s + e11*c;
		e10 = e10*c - e11*s;
		e11 = t;
	}


	/*-----------------------------------------------------------------------------------------
		Comparisons
	-----------------------------------------------------------------------------------------*/
	// Equality operators given as non-member operations after the class definition

	// Test if matrix is the identity
	// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
	bool IsIdentity() const;


	/*-----------------------------------------------------------------------------------------
		Othogonality
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	// "Orthogonal" means at right angles. Vectors are orthogonal if they are at right angles to
	// each other. Similarly, "orthonormal" describes normals that are at right angles to each
	// other. Most 2x2 transformation matrices contain orthogonal/orthonormal vectors, and
	// transformations that are not orthogonal in this way will skew or distort models they are
	// applied to. Unfortunately, floating point rounding can cause very persitent (long-lived)
	// matrices to gradually becom non-orthogonal. Testing for orthogonality and correction is
	// periodically required in such cases.

	// Test if matrix has orthogonal rows, i.e. if the two rows are vectors at right angles to
	// each other. Will also be orthonormal if it contains no scaling
	bool IsOrthogonal() const;

	// Test if matrix has orthonormal rows, i.e. if the two rows are *normals* at right angles to
	// each other. Scaled matrices cannot be orthonormal (can be orthogonal)
	bool IsOrthonormal() const;

	// Orthogonalise the rows/columns of the matrix. Generally used to "correct" matrices that
	// become non-orthogonal after repeated calculations/floating point rounding
	// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
	void Orthogonalise(	const CVector2& scale = CVector2::kOne );


	/*-----------------------------------------------------------------------------------------
		Inverse related
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	// Set this matrix to its transpose (matrix reflected through its diagonal)
	// This is also the (most efficient) inverse for a rotation matrix
    void Transpose();

	// Set this matrix to its inverse
	void Invert();


	/*-----------------------------------------------------------------------------------------
		Transformation Matrices
	-----------------------------------------------------------------------------------------*/
	// Non-member transformation creation functions given after the class definition

	// Make this matrix the identity matrix
    void MakeIdentity();


	// Make this matrix a rotation of the given angle (radians)
	void MakeRotation( const TFloat32 fAngle );


	// Make this matrix a scaling in X & Y by the values provided in the given vector
    void MakeScaling( const CVector2& scale );

	// Make this matrix a uniform scaling of the given amount
    void MakeScaling( const TFloat32 fScale );


	/*-----------------------------------------------------------------------------------------
		Facing Matrices
	-----------------------------------------------------------------------------------------*/

	// Make this matrix face in given direction. Can specify whether to use X or Y axis and
	// handedness (defaults: Y, left)
	void FaceDirection
	(
		const CVector2& direction,
		const bool      bUseYAxis = true,
		const bool      bLH = true
	);


	/*-----------------------------------------------------------------------------------------
		Member Operators
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	//////////////////////////////////
	// Scalar multiplication/division

    // Scalar multiplication
    CMatrix2x2& operator*=( const TFloat32 s );

    // Scalar division
    CMatrix2x2& operator/=( const TFloat32 s );


	///////////////////////////////
	// Vector multiplication

	// Return the given vector transformed by this matrix (pre-multiplication: V' = V*M)
    CVector2 Transform( const CVector2& v ) const;


	///////////////////////////////
	// Matrix multiplication

	// Post-multiply this matrix by the given one
    CMatrix2x2& operator*=( const CMatrix2x2& m );
	

	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// Matrix elements
	TFloat32 e00, e01;
	TFloat32 e10, e11;

	// Standard matrices
	static const CMatrix2x2 kIdentity;
};


/*-----------------------------------------------------------------------------------------
	Non-member Operators
-----------------------------------------------------------------------------------------*/

///////////////////////////////
// Comparison

// Matrix equality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator==
(
	const CMatrix2x2& m1,
	const CMatrix2x2& m2
);

// Matrix inequality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator!=
(
	const CMatrix2x2& m1,
	const CMatrix2x2& m2
);


//////////////////////////////////
// Scalar multiplication/division

// Scalar-matrix multiplication
CMatrix2x2 operator*
(
	const TFloat32    s,
	const CMatrix2x2& m
);

// Matrix-scalar multiplication
CMatrix2x2 operator*
(
	const CMatrix2x2& m,
	const TFloat32    s
);

// Matrix-scalar division
CMatrix2x2 operator/
(
	const CMatrix2x2& m,
	const TFloat32    s
);


///////////////////////////////
// Vector multiplication

// Vector-matrix multiplication (order is important - this is usual order for transformation
// for matrices stored as row vectors)
CVector2 operator*
(
	const CVector2&   v,
	const CMatrix2x2& m
);

// Matrix-vector multiplication (order is important - this is an unusual order for matrices
// stored as row vectors)
CVector2 operator*
(
	const CMatrix2x2& m,
	const CVector2&   v
);


///////////////////////////////
// Matrix multiplication

// General matrix-matrix multiplication
CMatrix2x2 operator*
(
	const CMatrix2x2& m1,
	const CMatrix2x2& m2
);


/*-----------------------------------------------------------------------------------------
	Non-Member Othogonality
-----------------------------------------------------------------------------------------*/

// Return a copy of given matrix with orthogonalised rows/columns. Generally used to "correct"
// matrices that become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
CMatrix2x2 Orthogonalise
(
	const CMatrix2x2& m,
	const CVector2&   scale = CVector2::kOne
);


/*-----------------------------------------------------------------------------------------
	Non-member Inverse Related
-----------------------------------------------------------------------------------------*/
	
// Return the transpose of given matrix (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
CMatrix2x2 Transpose( const CMatrix2x2& m );

// Return the inverse of given matrix
CMatrix2x2 Inverse( const CMatrix2x2& m );


/*-----------------------------------------------------------------------------------------
	Transformation Matrices
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value). Can be used
// as temporaries in calculations, e.g.
//     CMatrix2x2 m = MatrixScaling( 3.0f ) * MatrixRotation( ToRadians(45.0f) );

// Return an identity matrix
CMatrix2x2 Matrix2x2Identity();


// Return a rotation matrix of the given angle
CMatrix2x2 Matrix2x2Rotation( const TFloat32 fAngle );


// Return a matrix that is a scaling in X and Y of the values provided in the given vector
CMatrix2x2 Matrix2x2Scaling( const CVector2& scale );

// Return a matrix that is a uniform scaling of the given amount
CMatrix2x2 Matrix2x2Scaling( const TFloat32 fScale );


/*-----------------------------------------------------------------------------------------
	Facing Matrices
-----------------------------------------------------------------------------------------*/

// Return matrix that faces in given direction. Can specify whether to use X or Y axis and
// handedness (defaults: Y, left)
CMatrix2x2 MatrixFaceDirection
(
	const CVector2& direction,
	const bool      bUseYAxis = true,
	const bool      bLH = true
);


} // namespace gen

#endif // GEN_C_MATRIX_2X2_H_INCLUDED
