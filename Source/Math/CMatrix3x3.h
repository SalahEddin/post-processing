/**************************************************************************************************
	Module:       CMatrix3x3.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Definition of the concrete class CMatrix3x3, a 3x3 matrix of 32-bit floats. Designed for
	transformation matrices for 3D graphics or affine transformations for 2D graphics

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

// This API is designed for 3D (non-affine) transformations or 2D affine transformation matrices
// It uses row vectors to represent the axes (and origin) of the transformed space.
//
// A 3D transformation matrix can perform rotation, scaling, reflection and shear, but not
// translation. It is equivalent to the upper-left 3x3 of the 4x4 affine transformations supported
// by the CMatrix4x4 class, see that header file for more detail. In general 4x4 matrices will be
// used for 3D graphics, but 3x3 matrices are occasionally useful (e.g math work, space saving)
//
// A 2D affine transformation is used for 2D graphic manipulations including translations. It is a
// 3x3 matrix equivalent to the 4x4 affine transformations for 3D. It has the form:
//     Xx Xy 0   where (Xx, Xy) is the X axis, (Yx, Yy) the Y axis and
//     Yx Yy 0         (Px, Py) the local origin (position) of the space
//     Px Py 1         that this matrix transforms into
//
// The row-based form allows efficient access to the axes and position, and makes transformations
// follow an intuitive order, e.g. if T is a translation and S a scale and V is a vertex then:
//     V' = V*T*S means take vertex V, translate by T, then scale by S to produce output vertex V'
// This is the form used by DirectX and some graphics texts
//
// However, the standard mathematical form for affine transformation matrices is transposed, and
// uses column vectors:
//     Xx Yx Px
//     Xy Yy Py
//     0  0  1
//
// In this case the tranformation order is right to left:
//     V' = S*T*V means take V then translate by T, then scale by S to produce output vertex V'
// This is the form used by the OpenGL and the majority of maths texts
//
// Exercise caution when using texts with this second form with this class
//
// Notes:
// - The methods that deal with individual components of an (affine) transformation make the
//   assumption that the matrix is composed in this manner:
//     M = Scale*Rotation(*Translation)
//   The relevant methods are those headed (affine) matrix creation, decomposition and manipulation
//   Custom code may be needed for other orders or transformations (e.g. shear)
// - As the matrix is stored in rows, the [] operator is provided to return CVector3/CVector2
//   references to  the actual matrix data. This is highly convenient/efficient but non-portable,
//   i.e. the [] operator is not guaranteed to work on all compilers (though it will on most)

#ifndef GEN_C_MATRIX_3X3_H_INCLUDED
#define GEN_C_MATRIX_3X3_H_INCLUDED

#include "Defines.h"
#include "BaseMath.h"
#include "CVector2.h"
#include "CVector3.h"

namespace gen
{

// Forward declaration of classes, where includes are only possible/necessary in the .cpp file
class CMatrix2x2;
class CQuaternion;


class CMatrix3x3
{
	GEN_CLASS( CMatrix3x3 );

// Concrete class - public access
public:

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised (for performance)
	CMatrix3x3() {}

	// Construct by value
	CMatrix3x3
	(
		const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02,
		const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12,
		const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22
	);

	// Construct through pointer to 9 floats, may specify row/column order of data
	explicit CMatrix3x3
	(
		const TFloat32* pfElts,
		const bool      bRows = true
	);
	// 'explicit' disallows implicit conversion:  TFloat32* pf; CMatrix3x3 m = pf;
	// Need to use this constructor explicitly:   TFloat32* pf; CMatrix3x3 m = CMatrix3x3(pf);
	// Only applies to constructors that can take one parameter, used to avoid confusing code

	// Construct by row or column using CVector3's, may specify if setting rows or columns
    CMatrix3x3
	(
		const CVector3& v0,
		const CVector3& v1,
		const CVector3& v2,
		const bool      bRows = true
	);

	// Construct by row or column using CVector2's, remaining elements taken from identity matrix
	// May specify if setting rows or columns
	CMatrix3x3
	(
		const CVector2& v0,
		const CVector2& v1,
		const CVector2& v2,
		const bool      bRows = true
	);


	// Construct matrix transformation from euler angles and optional scaling. Matrix is effectively
	// built in this order: M = Scale*Rotation
	explicit CMatrix3x3
	(
		const CVector3&      angles,
		const ERotationOrder eRotOrder = kZXY,
		const CVector3&      scale = CVector3::kOne
	);
	// Require explicit conversion from angles only (see above)

	// Construct matrix transformation from quaternion and optional scaling. Matrix is effectively
	// built in this order: M = Scale*Rotation
	explicit CMatrix3x3
	(
		const CQuaternion& quat,
		const CVector3&    scale = CVector3::kOne
	);
	// Require explicit conversion from CQuaternion only (see above)

	// Construct matrix transformation from axis/angle of rotation and optional scaling. Matrix is
	// effectively built in this order: M = Scale*Rotation
	CMatrix3x3
	(
		const CVector3& axis,
		const TFloat32  fAngle,
		const CVector3& scale = CVector3::kOne
	);


	// Construct 2D affine transformation from position (translation) only
	explicit CMatrix3x3( const CVector2& position );
	// Require explicit conversion from position only (see above)

	// Construct 2D affine transformation from position, rotation angle and optional scaling, with 
	// remaining elements taken from the identity matrix. Matrix is effectively built in this
	// order: M = Scale*Rotation*Translation
	CMatrix3x3
	(
		const CVector2& position,
		const TFloat32  fAngle,
		const CVector2& scale = CVector2::kOne
	);


	// Construct from a CMatrix2x2 and optional 2D position, with remaining elements taken from
	// the identity matrix
	explicit CMatrix3x3
	(
		const CMatrix2x2& m,
		const CVector2&   position = CVector2::kOrigin
	);
	// Require explicit conversion from CMatrix2x2 (see above)


	// Copy constructor
    CMatrix3x3( const CMatrix3x3& m );

	// Assignment operator
    CMatrix3x3& operator=( const CMatrix3x3& m );


	/*-----------------------------------------------------------------------------------------
		Setters
	-----------------------------------------------------------------------------------------*/

	// Set by value
	void Set
	(
		const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02,
		const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12,
		const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22
	);

	// Set through pointer to 9 floats, may specify column/row order of data
	void Set
	(
		const TFloat32* pfElts,
		const bool      bRows = true
	);


	/*-----------------------------------------------------------------------------------------
		Row/column getters & setters
	-----------------------------------------------------------------------------------------*/

	// Get a single row (range 0-2) of the matrix
    CVector3 GetRow( const TUInt32 iRow ) const;

	// Get a single column (range 0-2) of the matrix
    CVector3 GetColumn( const TUInt32 iCol ) const;


	// Set a single row (range 0-2) of the matrix
    void SetRow
	(
		const TUInt32   iRow,
		const CVector3& v
	);

	// Set a single row (range 0-2) of the matrix using a CVector2. Third element left unchanged
    void SetRow
	(
		const TUInt32   iRow,
		const CVector2& v
	);


	// Set a single column (range 0-2) of the matrix
    void SetColumn
	(
		const TUInt32   iCol,
		const CVector3& v
	);

	// Set single column (range 0-2) of the matrix using a CVector2. Third element left unchanged
    void SetColumn
	(
		const TUInt32   iCol,
		const CVector2& v
	);


	// Set all rows of the matrix at once
    void SetRows
	(
		const CVector3& v0,
		const CVector3& v1,
		const CVector3& v2
	);

	// Set all rows of the matrix at once using CVector2, with remaining elements taken from the
	// identity matrix
    void SetRows
	(
		const CVector2& v0,
		const CVector2& v1,
		const CVector2& v2
	);


	// Set all columns of the matrix at once
    void SetColumns
	(
		const CVector3& v0,
		const CVector3& v1,
		const CVector3& v2
	);

	// Set all columns of the matrix at once using CVector2, with remaining elements taken from the
	// identity matrix
    void SetColumns
	(
		const CVector2& v0,
		const CVector2& v1,
		const CVector2& v2
	);


	/*-----------------------------------------------------------------------------------------
		Non-portable row access
	-----------------------------------------------------------------------------------------*/
	// These reinterpreting functions are not guaranteed to be portable, i.e. may not work on all
	// compilers (but will on most). However, they improve efficiency and are highly convenient

	// Direct access to rows as CVector3 - allows two dimensional subscripting of elements
	// Efficient but non-portable. Index in range 0-2 - no validation of index
    CVector3& operator[]( const TUInt32 iRow )
	{
		return *reinterpret_cast<CVector3*>(&e00 + iRow * 3);
	}

	// Direct access to (const) rows as CVector3 - allows two dimensional subscripting of elements
	// Efficient but non-portable. Index in range 0-2 - no validation of index
    const CVector3& operator[](	const TUInt32 iRow ) const
	{
		return *reinterpret_cast<const CVector3*>(&e00 + iRow * 3);
	}


	// Direct access to X-axis of 2D affine transformation as CVector2
	// Efficient but non-portable
	CVector2& XAxis2D()
	{
		return *reinterpret_cast<CVector2*>(&e00);
	}

	// Direct (const) access to X-axis of 2D affine transformation as CVector2
	// Efficient but non-portable
	const CVector2& XAxis2D() const
	{
		return *reinterpret_cast<const CVector2*>(&e00);
	}

	// Direct access to Y-axis of 2D affine transformation as CVector2
	// Efficient but non-portable
	CVector2& YAxis2D()
	{
		return *reinterpret_cast<CVector2*>(&e10);
	}

	// Direct (const) access to Y-axis of 2D affine transformation as CVector2
	// Efficient but non-portable
	const CVector2& YAxis2D() const
	{
		return *reinterpret_cast<const CVector2*>(&e10);
	}


	// Direct access to position (translation) of 2D affine transformation as CVector2
	// Efficient but non-portable
	CVector2& Position2D()
	{
		return *reinterpret_cast<CVector2*>(&e20);
	}

	// Direct (const) access to position (translation) of 2D affine transformation as CVector2
	// Efficient but non-portable
	const CVector2& Position2D() const
	{
		return *reinterpret_cast<const CVector2*>(&e20);
	}


	/*-----------------------------------------------------------------------------------------
		Creation and Decomposition of Transformation Matrices
	-----------------------------------------------------------------------------------------*/
	// These methods make the assumption that the matrix is composed in this order:
	//     M = Scale*Rotation
	// Custom code may be needed for other orders or transformations (e.g. shear)

	// Make matrix a transformation using Euler angles & optional scaling, with remaining elements
	// taken from the identity matrix. May specify order to apply rotations. Matrix is built in
	// this order: M = Scale*Rotation
	void MakeTransformEuler
	(
		const CVector3&      angles,
		const ERotationOrder eRotOrder = kZXY,
		const CVector3&      scale = CVector3::kOne
	);

	// Make matrix a transformation using quaternion & optional scaling, with remaining elements
	// taken from the identity matrix. Matrix is built in this order: M = Scale*Rotation
	void MakeTransformQuaternion
	(
		const CQuaternion& quat,
		const CVector3&    scale = CVector3::kOne
	);

	// Make matrix a transformation using angle/axis of rotation and optional scaling, with
	// remaining elements taken from the identity matrix
	// Matrix is built in this order: M = Scale*Rotation*Translation
	void MakeTransformAxisAngle
	(
		const CVector3& axis,
		const TFloat32  fAngle,
		const CVector3& scale = CVector3::kOne
	);


	// Decompose transformation into Euler angles of rotation (X, Y & Z angles of rotation around
	// each axis) and scale. Optionally pass order of rotations. Pass NULL for any unneeded
	// parameters. Assumes matrix is built in this order: M = Scale*Rotation
    void DecomposeTransformEuler
	(
		CVector3*            pAngles,
		CVector3*            pScale,
		const ERotationOrder eRotOrder = kZXY
	) const;

	// Decompose transformation into quaternion of rotation and scale. Pass NULL for any unneeded
	// parameters. Assumes built in this order: M = Scale*Rotation
    void DecomposeTransformQuaternion
	(
		CQuaternion* pQuat,
		CVector3*    pScale
	) const;

	// Decompose transformation into angle/axis of rotation (an axis and amount of rotation around
	// that axis) and scale. Pass NULL for any unneeded parameters. Assumes matrix is built in this
	// order: M = Scale*Rotation
    void DecomposeTransformAxisAngle
	(
		CVector3* pAxis,
		TFloat32* pfAngle,
		CVector3* pScale
	) const;


	/*-----------------------------------------------------------------------------------------
		Creation and Decomposition of 2D Affine Matrices
	-----------------------------------------------------------------------------------------*/
	// These methods make the assumption that the matrix is composed in this order:
	//     M = Scale*Rotation*Translation
	// Custom code may be needed for other orders or transformations (e.g. shear)

	// Make 2D affine transformation from position, and optional rotation angle & scaling, with 
	// remaining elements taken from the identity matrix. Matrix is effectively built in this
	// order: M = Scale*Rotation*Translation
	void MakeAffine2D
	(
		const CVector2& position,
		const TFloat32  fAngle = 0.0f,
		const CVector2& scale = CVector2::kOne
	);


	// Decompose 2D affine transformation into position, angle of rotation & scale. Pass NULL for
	// unneeded parameters. Assumes matrix is built in this order: M = Scale*Rotation*Translation
    void DecomposeAffine2D
	(
		CVector2* pPosition,
		TFloat32* pfAngle,
		CVector2* pScale
	) const;


	/*-----------------------------------------------------------------------------------------
		Manipulation of Transformation Matrices
	-----------------------------------------------------------------------------------------*/
	// All basic operations supported, use other functions for more advanced tasks:
	// To get current rotations (Euler angles) use DecomposeTransformEuler
	// To set specific rotations (Euler angles) use MakeTransformEuler
	// To rotate round an arbitrary (world) axis use:  m *= MakeTransformAxisAngle(...)
	//
	// These methods make the assumption that the matrix is composed in this order:
	//     M = Scale*Rotation
	// Custom code may be needed for other orders or transformations (e.g. shear)

	// Get the X scaling of a transformation matrix
	TFloat32 GetScaleX() const
	{
		return Sqrt( e00*e00 + e01*e01 + e02*e02 );
	}

	// Get the Y scaling of a transformation matrix
	TFloat32 GetScaleY() const
	{
		return Sqrt( e10*e10 + e11*e11 + e12*e12 );
	}

	// Get the Z scaling of a transformation matrix
	TFloat32 GetScaleZ() const
	{
		return Sqrt( e20*e20 + e21*e21 + e22*e22 );
	}

	// Get the X, Y & Z scaling of a transformation matrix
    CVector3 GetScale() const;


	// Set the X scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
	void SetScaleX( const TFloat32 x );

	// Set the Y scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
	void SetScaleY( const TFloat32 y );

	// Set the Z scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
	void SetScaleZ( const TFloat32 z );

	// Set the X, Y & Z scaling of a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
    void SetScale( const CVector3& scale );

	// Set a uniform scaling for a transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (rotation)
    void SetScale( const TFloat32 fScale );


	// Alter the scale of a transformation matrix in the (local) X direction. The effect is
	// multiplicative, e.g ScaleX( 2.0f ) indicates a doubling of the X scale
    void ScaleX( const TFloat32 x )
	{
		e00 *= x;
		e01 *= x;
		e02 *= x;
	}

	// Alter the scale of a transformation matrix in the (local) Y direction. The effect is
	// multiplicative, e.g ScaleY( 2.0f ) indicates a doubling of the Y scale
    void ScaleY( const TFloat32 y )
	{
		e10 *= y;
		e11 *= y;
		e12 *= y;
	}

	// Alter the scale of a transformation matrix in the (local) Z direction. The effect is
	// multiplicative, e.g ScaleZ( 2.0f ) indicates a doubling of the Z scale
    void ScaleZ( const TFloat32 z )
	{
		e20 *= z;
		e21 *= z;
		e22 *= z;
	}

	// Alter the scale of a transformation matrix in the (local) X, Y & Z directions. The effect
	// is multiplicative, e.g Scale( CVector3(2.0f, 2.0f, 2.0f) ) indicates a doubling of the scale
	// in all directions
    void Scale( const CVector3 scale )
	{
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

	// Alter the scale of a transformation matrix uniformly in the (local) X, Y & Z directions.
	// The effect is multiplicative, e.g Scale( 2.0f ) indicates a uniform doubling of the scale
    void Scale( const TFloat32 fScale )
	{
		e00 *= fScale;
		e01 *= fScale;
		e02 *= fScale;

		e10 *= fScale;
		e11 *= fScale;
		e12 *= fScale;

		e20 *= fScale;
		e21 *= fScale;
		e22 *= fScale;
	}


	// Rotate a transformation matrix by given angle (radians) around world X axis
	void RotateX( const TFloat32 x )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 sX, cX;
		SinCos( x, &sX, &cX );
		TFloat32 t;
		t   = e01*sX + e02*cX;
		e01 = e01*cX - e02*sX;
		e02 = t;
		t   = e11*sX + e12*cX;
		e11 = e11*cX - e12*sX;
		e12 = t;
		t   = e21*sX + e22*cX;
		e21 = e21*cX - e22*sX;
		e22 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around world Y axis
	void RotateY( const TFloat32 y )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 sY, cY;
		SinCos( y, &sY, &cY );
		TFloat32 t;
		t   = e00*cY + e02*sY;
		e02 = e02*cY - e00*sY;
		e00 = t;
		t   = e10*cY + e12*sY;
		e12 = e12*cY - e10*sY;
		e10 = t;
		t   = e20*cY + e22*sY;
		e22 = e22*cY - e20*sY;
		e20 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around world Z axis
	void RotateZ( const TFloat32 z )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 sZ, cZ;
		SinCos( z, &sZ, &cZ );
		TFloat32 t;
		t   = e00*sZ + e01*cZ;
		e00 = e00*cZ - e01*sZ;
		e01 = t;
		t   = e10*sZ + e11*cZ;
		e10 = e10*cZ - e11*sZ;
		e11 = t;
		t   = e20*sZ + e21*cZ;
		e20 = e20*cZ - e21*sZ;
		e21 = t;
	}


	// Rotate a transformation matrix by given angle (radians) around local X axis. Assumes the
	// matrix is built in the order: M = Scale*Rotation. Use RotateLocalXUnscaled if there is no
	// scaling (more efficient)
	void RotateLocalX( const TFloat32 x )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot,
		// becoming Scale*[XRot*Rot]
		TFloat32 scaleSqY = e10*e10 + e11*e11 + e12*e12;
		TFloat32 scaleSqZ = e20*e20 + e21*e21 + e22*e22;
		GEN_ASSERT_OPT( !IsZero(scaleSqY) && !IsZero(scaleSqZ), "Singular matrix" );
		TFloat32 scaleYZ = Sqrt( scaleSqY ) * InvSqrt( scaleSqZ );

		TFloat32 sX, cX, sXY, sXZ;
		SinCos( x, &sX, &cX );
		sXY = sX * scaleYZ;
		sXZ = sX / scaleYZ;

		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 t;
		t   = e10*cX + e20*sXY;
		e20 = e20*cX - e10*sXZ;
		e10 = t;
		t   = e11*cX + e21*sXY;
		e21 = e21*cX - e11*sXZ;
		e11 = t;
		t   = e12*cX + e22*sXY;
		e22 = e22*cX - e12*sXZ;
		e12 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around local X axis. Assumes the
	// matrix has no scaling, use RotateLocalX if there is scaling (less efficient)
	void RotateLocalXUnscaled( const TFloat32 x )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 sX, cX;
		SinCos( x, &sX, &cX );
		TFloat32 t;
		t   = e10*cX + e20*sX;
		e20 = e20*cX - e10*sX;
		e10 = t;
		t   = e11*cX + e21*sX;
		e21 = e21*cX - e11*sX;
		e11 = t;
		t   = e12*cX + e22*sX;
		e22 = e22*cX - e12*sX;
		e12 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around local Y axis. Assumes the
	// matrix is built in the order: M = Scale*Rotation. Use RotateLocalYUnscaled if there is no
	// scaling (more efficient)
	void RotateLocalY( const TFloat32 y )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot,
		// becoming Scale*[YRot*Rot]
		TFloat32 scaleSqX = e00*e00 + e01*e01 + e02*e02;
		TFloat32 scaleSqZ = e20*e20 + e21*e21 + e22*e22;
		GEN_ASSERT_OPT( !IsZero(scaleSqX) && !IsZero(scaleSqZ), "Singular matrix" );
		TFloat32 scaleZX = Sqrt( scaleSqZ ) * InvSqrt( scaleSqX );

		TFloat32 sY, cY, sYZ, sYX;
		SinCos( y, &sY, &cY );
		sYZ = sY * scaleZX;
		sYX = sY / scaleZX;

		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 t;
		t   = e20*cY + e00*sYZ;
		e00 = e00*cY - e20*sYX;
		e20 = t;
		t   = e21*cY + e01*sYZ;
		e01 = e01*cY - e21*sYX;
		e21 = t;
		t   = e22*cY + e02*sYZ;
		e02 = e02*cY - e22*sYX;
		e22 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around local Y axis. Assumes the
	// matrix has no scaling, use RotateLocalY if there is scaling (less efficient)
	void RotateLocalYUnscaled( const TFloat32 y )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 sY, cY;
		SinCos( y, &sY, &cY );
		TFloat32 t;
		t   = e20*cY + e00*sY;
		e00 = e00*cY - e20*sY;
		e20 = t;
		t   = e21*cY + e01*sY;
		e01 = e01*cY - e21*sY;
		e21 = t;
		t   = e22*cY + e02*sY;
		e02 = e02*cY - e22*sY;
		e22 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around local Z axis. Assumes the
	// matrix is built in the order: M = Scale*Rotation. Use RotateLocalZUnscaled if there is no
	// scaling (more efficient)
	void RotateLocalZ( const TFloat32 z )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot,
		// becoming Scale*[ZRot*Rot]
		TFloat32 scaleSqX = e00*e00 + e01*e01 + e02*e02;
		TFloat32 scaleSqY = e10*e10 + e11*e11 + e12*e12;
		GEN_ASSERT_OPT( !IsZero(scaleSqX) && !IsZero(scaleSqY), "Singular matrix" );
		TFloat32 scaleXY = Sqrt( scaleSqX ) * InvSqrt( scaleSqY );

		TFloat32 sZ, cZ, sZX, sZY;
		SinCos( z, &sZ, &cZ );
		sZX = sZ * scaleXY;
		sZY = sZ / scaleXY;

		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 t;
		t   = e00*cZ + e10*sZX;
		e10 = e10*cZ - e00*sZY;
		e00 = t;
		t   = e01*cZ + e11*sZX;
		e11 = e11*cZ - e01*sZY;
		e01 = t;
		t   = e02*cZ + e12*sZX;
		e12 = e12*cZ - e02*sZY;
		e02 = t;
	}

	// Rotate a transformation matrix by given angle (radians) around local Z axis. Assumes the
	// matrix has no scaling, use RotateLocalZ if there is scaling (less efficient)
	void RotateLocalZUnscaled( const TFloat32 z )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 sZ, cZ;
		SinCos( z, &sZ, &cZ );
		TFloat32 t;
		t   = e00*cZ + e10*sZ;
		e10 = e10*cZ - e00*sZ;
		e00 = t;
		t   = e01*cZ + e11*sZ;
		e11 = e11*cZ - e01*sZ;
		e01 = t;
		t   = e02*cZ + e12*sZ;
		e12 = e12*cZ - e02*sZ;
		e02 = t;
	}


	/*-----------------------------------------------------------------------------------------
		Manipulation of 2D Affine Matrices
	-----------------------------------------------------------------------------------------*/
	// These methods make the assumption that the matrix is composed in this order:
	//     M = Scale*Rotation*Translation
	// Custom code may be needed for other orders or transformations (e.g. shear)

	// Get the position (translation) of a 2D affine transformation matrix. Similar to GetRow( 2 ),
	// but returning CVector2. Use of Position2D() function may be more efficient, but non-portable
	CVector2 GetPosition2D() const
	{
		return CVector2(e20, e21);
	}

	// Set the position (translation) of a 2D affine transformation matrix. Will not change other
	// components of the transformation (rotation, scale etc.). Same as SetRow( 2, p ). Use of
	// Position2D() function may be more efficient, but non-portable
	void SetPosition2D( const CVector2& p )
	{
		e20 = p.x;
		e21 = p.y;
	}

	// Return X position (translation) of a 2D affine transformation matrix
	TFloat32 GetX2D() const 
	{
		return e20;
	}

	// Return Y position (translation) of a 2D affine transformation matrix
	TFloat32 GetY2D() const
	{
		return e21;
	}


	// Set X position (translation) of a 2D affine transformation matrix
	void SetX2D( const TFloat32 x )
	{
		e20 = x;
	}

	// Set Y position (translation) of a 2D affine transformation matrix
	void SetY2D( const TFloat32 y )
	{
		e21 = y;
	}


	// Move position (translation) of a 2D affine transformation matrix by the given vector
	void Move2D( const CVector2 v ) 
	{
		e20 += v.x;
		e21 += v.y;
	}

	// Move X position (translation) of a 2D affine transformation matrix
	void MoveX2D( const TFloat32 x )
	{
		e20 += x;
	}

	// Move Y position (translation) of a 2D affine transformation matrix
	void MoveY2D( const TFloat32 y )
	{
		e21 += y;
	}

	// Move position (translation) of a 2D affine transformation matrix by the given vector in its
	// local coordinate space. Will move exactly |v| units, regardless of scaling
	void MoveLocal2D( const CVector2 v ) 
	{
		// Adjust for any scaling
		TFloat32 scaledX = v.x * InvSqrt( e00*e00 + e01*e01 );
		TFloat32 scaledY = v.y * InvSqrt( e10*e10 + e11*e11 );
		e20 += scaledX * e00 + scaledY * e10;
		e21 += scaledX * e01 + scaledY * e11;
	}

	// Move position (translation) of a 2D affine transformation matrix by the given vector in its
	// local coordinate space. Will move relative to the matrix's scaling, i.e. matrix will move 
	// v.x * x-scaling units in x, similarly in y. More efficient than MoveLocal if the matrix is
	// unscaled
	void MoveLocal2DWithScaling( const CVector2 v ) 
	{
		e20 += v.x * e00 + v.y * e10;
		e21 += v.x * e01 + v.y * e11;
	}

	// Move X position (translation) of 2D affine transformation matrix along X axis of the matrix
	// Will move exactly x units, regardless of scaling
	void MoveLocalX2D( const TFloat32 x ) 
	{
		// Adjust for any x-scaling
		TFloat32 scaledX = x * InvSqrt( e00*e00 + e01*e01 );
		e20 += scaledX * e00;
		e21 += scaledX * e01;
	}

	// Move X position (translation) of 2D affine transformation matrix along X axis of the matrix
	// Will move relative to the matrix's x-scaling, i.e. will move x * x-scale units
	// More efficient than MoveLocalX if matrix is unscaled
	void MoveLocalX2DWithScaling( const TFloat32 x ) 
	{
		e20 += x * e00;
		e21 += x * e01;
	}

	// Move Y position (translation) of 2D affine transformation matrix along Y axis of the matrix
	// Will move exactly y units, regardless of scaling
	void MoveLocalY2D( const TFloat32 y ) 
	{
		// Adjust for any y-scaling
		TFloat32 scaledY = y * InvSqrt( e10*e10 + e11*e11 );
		e20 += scaledY * e10;
		e21 += scaledY * e11;
	}

	// Move Y position (translation) of 2D affine transformation matrix along Y axis of the matrix
	// Will move relative to the matrix's y-scaling, i.e. will move y * y-scale units
	// More efficient than MoveLocalY if matrix is unscaled
	void MoveLocalY2DWithScaling( const TFloat32 y ) 
	{
		e20 += y * e10;
		e21 += y * e11;
	}


	// Get the X scaling of a 2D affine transformation matrix
	TFloat32 GetScaleX2D() const
	{
		return Sqrt( e00*e00 + e01*e01 );
	}

	// Get the Y scaling of a 2D affine transformation matrix
	TFloat32 GetScaleY2D() const
	{
		return Sqrt( e10*e10 + e11*e11 );
	}

	// Get the X & Y scaling of a 2D affine transformation matrix
    CVector2 GetScale2D() const;


	// Set the X scaling of a 2D affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
	void SetScaleX2D( const TFloat32 x );

	// Set the Y scaling of a 2D affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
	void SetScaleY2D( const TFloat32 y );

	// Set the X & Y scaling of a 2D affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
    void SetScale2D( const CVector2& scale );

	// Set a uniform scaling for a 2D affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
    void SetScale2D( const TFloat32 fScale );


	// Alter the scale of a 2D affine transformation matrix in the (local) X direction. The effect
	// is multiplicative, e.g ScaleX2D( 2.0f ) indicates a doubling of the X scale
    void ScaleX2D( const TFloat32 x )
	{
		e00 *= x;
		e01 *= x;
	}

	// Alter the scale of a 2D affine transformation matrix in the (local) Y direction. The effect
	// is multiplicative, e.g ScaleY2D( 2.0f ) indicates a doubling of the Y scale
    void ScaleY2D( const TFloat32 y )
	{
		e10 *= y;
		e11 *= y;
	}

	// Alter the scale of a 2D affine transformation matrix in the (local) X & Y directions. The
	// effect is multiplicative, e.g Scale2D( CVector3(2.0f, 2.0f) ) indicates a doubling of
	// the scale in all directions
    void Scale2D( const CVector2 scale )
	{
		e00 *= scale.x;
		e01 *= scale.x;

		e10 *= scale.y;
		e11 *= scale.y;
	}

	// Alter the scale of a 2D affine transformation matrix uniformly in the (local) X & Y
	// directions. The effect is multiplicative, e.g Scale2D( 2.0f ) indicates a uniform doubling
	// of the scale
    void Scale2D( const TFloat32 fScale )
	{
		e00 *= fScale;
		e01 *= fScale;

		e10 *= fScale;
		e11 *= fScale;
	}


	// Rotate a 2D affine transformation matrix by given angle (in radians) around world origin
	void Rotate2D( const TFloat32 fAngle )
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
		t   = e20*s + e21*c;
		e20 = e20*c - e21*s;
		e21 = t;
	}

	// Rotate a 2D affine transformation matrix by given angle (in radians) around local origin
	// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocal2DUnscaled
	// if there is no scaling (more efficient)
	void RotateLocal2D( const TFloat32 fAngle )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[NewRot*Rot]*Trans
		TFloat32 scaleSqX = e00*e00 + e01*e01;
		TFloat32 scaleSqY = e10*e10 + e11*e11;
		GEN_ASSERT_OPT( !IsZero(scaleSqX) && !IsZero(scaleSqY), "Singular matrix" );
		TFloat32 scaleXY = Sqrt( scaleSqX ) * InvSqrt( scaleSqY );

		TFloat32 s, c, sX, sY;
		SinCos( fAngle, &s, &c );
		sX = s * scaleXY;
		sY = s / scaleXY;

		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 t;
		t   = e00*c + e10*sX;
		e10 = e10*c - e00*sY;
		e00 = t;
		t   = e01*c + e11*sX;
		e11 = e11*c - e01*sY;
		e01 = t;
	}

	// Rotate a 2D affine transformation matrix by given angle (in radians) around local origin
	// Assumes matrix built in the order: M = Rotation*Translation. Use RotateLocal2D if there is
	// also scaling (less efficient)
	void RotateLocal2DUnscaled( const TFloat32 fAngle )
	{
		// Perform minimum of calculations rather than use full matrix multiply
		TFloat32 s, c;
		SinCos( fAngle, &s, &c );
		TFloat32 t;
		t   = e00*c + e10*s;
		e10 = e10*c - e00*s;
		e00 = t;
		t   = e01*c + e11*s;
		e11 = e11*c - e01*s;
		e01 = t;
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
	// other. Most 3x3 transformation matrices contain orthogonal/orthonormal vectors, and
	// transformations that are not orthogonal in this way will skew or distort models they are
	// applied to. Unfortunately, floating point rounding can cause very persitent (long-lived)
	// matrices to gradually becom non-orthogonal. Testing for orthogonality and correction is
	// periodically required in such cases.
	// 2D affine matrices should also be orthogonal in their upper 2x2, see the discussion of
	// orthogonality for 3D affine matrices in the CMatrix4x4 header

	// Test if matrix has orthogonal rows, i.e. if the three rows are vectors at right angles to
	// each other. Will also be orthonormal if it contains no scaling
	bool IsOrthogonal() const;

	// Test if matrix has orthonormal rows, i.e. if the three rows are *normals* at right angles to
	// each other. Scaled matrices cannot be orthonormal (can be orthogonal)
	bool IsOrthonormal() const;

	// Orthogonalise the rows/columns of the matrix. Generally used to "correct" matrices that
	// become non-orthogonal after repeated calculations/floating point rounding
	// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
	void Orthogonalise(	const CVector3& scale = CVector3::kOne );


	// Test if upper-left 2x2 matrix has orthogonal rows, i.e. if the two rows are vectors
	// at right angles to each other. Will also be orthonormal if it contains no scaling
	bool IsOrthogonal2x2() const;

	// Test if upper-left 2x2 matrix has orthonormal rows, i.e. if the two rows are *normals*
	// at right angles to each other. Scaled matrices cannot be orthonormal (can be orthogonal)
	bool IsOrthonormal2x2() const;

	// Orthogonalise the rows/columns of the upper-left 2x2 matrix. Generally used to "correct"
	// matrices that become non-orthogonal after repeated calculations/floating point rounding
	// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
	void Orthogonalise2x2(	const CVector2& scale = CVector2::kOne );


	/*-----------------------------------------------------------------------------------------
		Inverse related
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	// Set this matrix to its transpose (matrix reflected through its diagonal)
	// This is also the (most efficient) inverse for a rotation matrix
    void Transpose();

	// Set this matrix to its inverse assuming it has orthogonal rows, i.e. it is a transformation
	// matrix with no shear. Most efficient inverse for transformations with rotation & scale only
    void InvertRotScale();

	// Set this matrix to its inverse assuming that it is a 2D affine matrix
	void InvertAffine2D();

	// Set this matrix to its inverse. Most general, least efficient inverse function
	void Invert();


	/*-----------------------------------------------------------------------------------------
		Transformation Matrices
	-----------------------------------------------------------------------------------------*/
	// Non-member transformation creation functions given after the class definition

	// Make this matrix the identity matrix
    void MakeIdentity();


	// Make this matrix an X-axis rotation by the given angle (radians)
	void MakeRotationX( const TFloat32 x );

	// Make this matrix a Y-axis rotation by the given angle (radians)
    void MakeRotationY( const TFloat32 y );

	// Make this matrix a Z-axis rotation by the given angle (radians)
	void MakeRotationZ( const TFloat32 z );

	// Make this matrix a combined rotation around the X, Y & Z axes by the given angles (radians),
	// applied in the order specified
	void MakeRotation
	( 
		const CVector3       angles,
		const ERotationOrder eRotOrder = kZXY
	);

	// Make this matrix a rotation around the given axis by the given angle (radians)
	void MakeRotation
	(
		const CVector3& axis,
		const TFloat32  fAngle
	);


	// Make this matrix a scaling in X,Y and Z by the values provided in the given vector
    void MakeScaling( const CVector3& scale );

	// Make this matrix a uniform scaling of the given amount
    void MakeScaling( const TFloat32 fScale );


	/*-----------------------------------------------------------------------------------------
		2D Affine Transformation Matrices
	-----------------------------------------------------------------------------------------*/
	// Non-member transformation creation functions given after the class definition
	
	// Make this matrix a 2D affine translation by the given vector
	void MakeTranslation2D( const CVector2& translate );
    

	// Make this matrix an 2D affine rotation by the given angle (radians)
	void MakeRotation2D( const TFloat32 fAngle );


	// Make this matrix a 2D affine scaling in X and Y by the values provided in the given vector
    void MakeScaling2D( const CVector2& scale );

	// Make this matrix a 2D affine uniform scaling of the given amount
    void MakeScaling2D( const TFloat32 fScale );


	/*-----------------------------------------------------------------------------------------
		Facing Matrices
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	// Make this matrix a rotation matrix that faces in given direction (Z axis). Can pass up
	// vector for the constructed matrix and specify handedness (right-handed Z axis will face
	// away from direction)
    void FaceDirection
	(
		const CVector3& direction,
		const CVector3& up = CVector3::kYAxis,
		const bool      bLH = true
	);


	// Make this matrix an affine 2D transformation matrix to point from current position to given
	// target. Can specify whether to use X or Y axis and handedness (defaults: Y, left)
	// Will retain the matrix's current scaling
	void FaceTarget2D
	(
		const CVector2& target,
		const bool      bUseYAxis = true,
		const bool      bLH = true
	);


	// Make this matrix an affine 2D transformation matrix that faces in given direction from the
	// current position. Can specify whether to use X or Y axis and handedness (defaults: Y, left)
	// Will retain the matrix's current scaling
	void FaceDirection2D
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
    CMatrix3x3& operator*=( const TFloat32 s );

    // Scalar division
    CMatrix3x3& operator/=( const TFloat32 s );


	///////////////////////////////
	// Vector multiplication

	// Return the given vector transformed by this matrix (pre-multiplication: V' = V*M)
    CVector3 Transform( const CVector3& v ) const;

	// Return the given CVector2 transformed by this matrix (pre-multiplication: V' = V*M)
	// Assuming it is a vector rather then a point, i.e. assume the vector's 3th element is 0
    CVector2 TransformVector2D( const CVector2& v ) const;
    
	// Return the given CVector2 transformed by this matrix (pre-multiplication: V' = V*M)
	// Assuming it is a point rather then a vector, i.e. assume the vector's 3th element is 1
    CVector2 TransformPoint2D( const CVector2& p ) const;


	///////////////////////////////
	// Matrix multiplication

	// Post-multiply this matrix by the given one
    CMatrix3x3& operator*=( const CMatrix3x3& m );
	
	// Post-multiply this matrix by the given one, assuming they are both affine
	CMatrix3x3& MultiplyAffine2D( const CMatrix3x3& m );


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// Matrix elements
	TFloat32 e00, e01, e02;
	TFloat32 e10, e11, e12;
	TFloat32 e20, e21, e22;

	// Standard matrices
	static const CMatrix3x3 kIdentity;
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
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
);

// Matrix inequality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator!=
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
);


//////////////////////////////////
// Scalar multiplication/division

// Scalar-matrix multiplication
CMatrix3x3 operator*
(
	const TFloat32    s,
	const CMatrix3x3& m
);

// Matrix-scalar multiplication
CMatrix3x3 operator*
(
	const CMatrix3x3& m,
	const TFloat32    s
);

// Matrix-scalar division
CMatrix3x3 operator/
(
	const CMatrix3x3& m,
	const TFloat32    s
);


///////////////////////////////
// Vector multiplication

// Vector-matrix multiplication (order is important - this is usual order for transformation
// for matrices stored as row vectors - see notes at top)
CVector3 operator*
(
	const CVector3&   v,
	const CMatrix3x3& m
);

// Matrix-vector multiplication (order is important - this is an unusual order for matrices
// stored as row vectors - see notes at top)
CVector3 operator*
(
	const CMatrix3x3& m,
	const CVector3&   v
);


///////////////////////////////
// Matrix multiplication

// General matrix-matrix multiplication
CMatrix3x3 operator*
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
);

// Matrix-matrix multiplication assuming both matrices are 2D affine transformations
CMatrix3x3 MultiplyAffine2D
(
	const CMatrix3x3& m1,
	const CMatrix3x3& m2
);


/*-----------------------------------------------------------------------------------------
	Non-Member Othogonality
-----------------------------------------------------------------------------------------*/

// Return a copy of given matrix with orthogonalised rows/columns. Generally used to "correct"
// matrices that become non-orthogonal after repeated calculations/floating point rounding
// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
CMatrix3x3 Orthogonalise
(
	const CMatrix3x3& m,
	const CVector3&   scale = CVector3::kOne
);

// Return a copy of given matrix with orthogonalised rows/columns in the upper-left 2x2 matrix.
// Generally used to "correct" matrices that become non-orthogonal after repeated calculations /
// floating point rounding. May pass scaling for resultant rows - default is 1, leading to
// "orthonormalisation"
CMatrix3x3 Orthogonalise2x2
(
	const CMatrix3x3& m,
	const CVector2&   scale = CVector2::kOne
);


/*-----------------------------------------------------------------------------------------
	Non-member Inverse Related
-----------------------------------------------------------------------------------------*/
	
// Return the transpose of given matrix (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
CMatrix3x3 Transpose( const CMatrix3x3& m );

// Return the inverse of given matrix assuming it has orthogonal rows, i.e. it is a transformation
// matrix with no shear. Most efficient inverse for transformations with rotation & scale only
CMatrix3x3 InverseRotScale( const CMatrix3x3& m );

// Return the inverse of given matrix assuming only that it is a 2D affine matrix
CMatrix3x3 InverseAffine2D( const CMatrix3x3& m );

// Return the inverse of given matrix. Most general, least efficient inverse function
CMatrix3x3 Inverse( const CMatrix3x3& m );


/*-----------------------------------------------------------------------------------------
	Transformation Matrices
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value). Can be used
// as temporaries in calculations, e.g.
//     CMatrix3x3 m = MatrixScaling( 3.0f ) * MatrixRotationX( ToRadians(45.0f) );

// Return an identity matrix
CMatrix3x3 Matrix3x3Identity();


// Return an X-axis rotation matrix of the given angle
CMatrix3x3 Matrix3x3RotationX( const TFloat32 x );

// Return a Y-axis rotation matrix of the given angle
CMatrix3x3 Matrix3x3RotationY( const TFloat32 y );

// Return a Z-axis rotation matrix of the given angle
CMatrix3x3 Matrix3x3RotationZ( const TFloat32 z );


// Make a matrix that is a combined rotation around the X, Y & Z axes by the given angles
// (radians), applied in the order specified
CMatrix3x3 Matrix3x3Rotation
( 
	const CVector3       angles,
	const ERotationOrder eRotOrder = kZXY
);

// Return a matrix that is a rotation around the given axis of the given angle (radians)
CMatrix3x3 Matrix3x3Rotation
(
	const CVector3& axis,
	const TFloat32  fAngle
);


// Return a matrix that is a scaling in X,Y and Z of the values provided in the given vector
CMatrix3x3 Matrix3x3Scaling( const CVector3& scale );

// Return a matrix that is a uniform scaling of the given amount
CMatrix3x3 Matrix3x3Scaling( const TFloat32 fScale );


/*-----------------------------------------------------------------------------------------
	2D Affine Transformation Matrices
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value) - see above

// Return a matrix that is a 2D affine translation of the given vector
CMatrix3x3 MatrixTranslation2D( const CVector2& translate );


// Return a matrix that is a 2D affine rotation of the given angle (radians)
CMatrix3x3 MatrixRotation2D( const TFloat32 fAngle );


// Return a matrix that is a 2D affine scaling in X and Y by the values provided in the given vector
CMatrix3x3 MatrixScaling2D( const CVector2& scale );

// Return a matrix that is a 2D affine uniform scaling of the given amount
CMatrix3x3 MatrixScaling2D( const TFloat32 fScale );


/*-----------------------------------------------------------------------------------------
	Facing Matrices
-----------------------------------------------------------------------------------------*/

// Return rotation matrix that faces in given direction (Z axis). Can pass up vector for the
// constructed matrix and specify handedness (right-handed Z axis will face away from direction)
CMatrix3x3 MatrixFaceDirection
(
	const CVector3& direction,
	const CVector3& up = CVector3::kYAxis,
	const bool      bLH = true
);


// Return affine 3D transformation matrix that points from given position to given target. Can
// specify whether to use X or Y axis and handedness (defaults: Y, left)
CMatrix3x3 MatrixFaceTarget2D
(
	const CVector2& position,
	const CVector2& target,
	const bool      bUseYAxis = true,
	const bool      bLH = true
);


// Return affine 3D transformation matrix that faces in given direction from the given position.
// Can specify whether to use X or Y axis and handedness (defaults: Y, left)
CMatrix3x3 MatrixFaceDirection2D
(
	const CVector2& position,
	const CVector2& direction,
	const bool      bUseYAxis = true,
	const bool      bLH = true
);


} // namespace gen

#endif // GEN_C_MATRIX_3X3_H_INCLUDED
