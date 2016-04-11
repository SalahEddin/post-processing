/**************************************************************************************************
	Module:       CMatrix4x4.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Definition of the concrete class CMatrix4x4, a 4x4 matrix of 32-bit floats. Supports general
	4x4 matrices, but primarily designed for affine transformation matrices for 3D graphics

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

// This API is mainly designed for affine transformation matrices using row vectors to represent
// the axes and origin of the transformed space. An affine matrix has the form:
//     Xx Xy Xz 0
//     Yx Yy Yz 0   where (Xx, Xy, Xz) is the X axis, (Yx, Yy, Yz) the Y axis
//     Zx Zy Zz 0         (Zx, Zy, Zz) the Z axis and (Px, Py, Pz) the local origin (position)
//     Px Py Pz 1         of the space that this matrix transforms into
//
// The row-based form allows efficient access to the axes and position, and makes transformations
// follow an intuitive order, e.g. if T is a translation and S a scale and V is a vertex then:
//     V' = V*T*S means take vertex V, translate by T, then scale by S to produce output vertex V'
// This is the form used by DirectX and some graphics texts
//
// However, the standard mathematical form for affine transformation matrices is transposed, and
// uses column vectors:
//     Xx Yx Zx Px
//     Xy Yy Zy Py
//     Xz Yz Zz Pz
//     0  0  0  1
//
// In this case the tranformation order is right to left:
//     V' = S*T*V means take V then translate by T, then scale by S to produce output vertex V'
// This is the form used by OpenGL, video cards (shaders), and the majority of maths texts
//
// Exercise caution when using texts with this second form with this class
//
// Notes:
// - The affine matrix methods that deal with individual components of a transformation make the
//   assumption that the matrix is composed in this manner:
//     M = Scale*Rotation*Translation
//   The relevant methods are those headed affine matrix creation, decomposition and manipulation.
//   Custom code may be needed for other orders or transformations (e.g. shear)
// - The class can be used for general non-affine matrices (e.g. perspective projection matrices),
//   but provides less support, e.g. no addition/subtraction, general inverse function etc.
// - As the matrix is stored in rows, the [] operator is provided to returns CVector4/CVector3
//   references to the actual matrix data. This is highly convenient/efficient but non-portable,
//   i.e. the [] operator is not guaranteed to work on all compilers (though it will on most)

#ifndef GEN_C_MATRIX_4X4_H_INCLUDED
#define GEN_C_MATRIX_4X4_H_INCLUDED

#include "Defines.h"
#include "BaseMath.h"
#include "CVector2.h"
#include "CVector3.h"

namespace gen
{

// Forward declaration of classes, where includes are only possible/necessary in the .cpp file
class CVector4;
class CMatrix2x2;
class CMatrix3x3;
class CQuaternion;


class CMatrix4x4
{
	GEN_CLASS( CMatrix4x4 );

// Concrete class - public access
public:

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised (for performance)
	CMatrix4x4() {}

	// Construct by value
	CMatrix4x4
	(
		const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02, const TFloat32 elt03,
		const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12, const TFloat32 elt13,
		const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22, const TFloat32 elt23,
		const TFloat32 elt30, const TFloat32 elt31, const TFloat32 elt32, const TFloat32 elt33
	);

	// Construct through pointer to 16 floats, may specify row/column order of data
	explicit CMatrix4x4
	(
		const TFloat32* pfElts,
		const bool      bRows = true
	);
	// 'explicit' disallows implicit conversion:  TFloat32* pf; CMatrix4x4 m = pf;
	// Need to use this constructor explicitly:   TFloat32* pf; CMatrix4x4 m = CMatrix4x4(pf);
	// Only applies to constructors that can take one parameter, used to avoid confusing code

	// Construct by row or column using CVector4's, may specify if setting rows or columns
    CMatrix4x4
	(
		const CVector4& v0,
		const CVector4& v1,
		const CVector4& v2,
		const CVector4& v3,
		const bool      bRows = true
	);

	// Construct by row or column using CVector3's, remaining elements taken from identity matrix
	// May specify if setting rows or columns
	CMatrix4x4
	(
		const CVector3& v0,
		const CVector3& v1,
		const CVector3& v2,
		const CVector3& v3,
		const bool      bRows = true
	);


	// Construct affine transformation from position (translation) only
	explicit CMatrix4x4( const CVector3& position );
	// Require explicit conversion from position only (see above)

	// Construct affine transformation from position, Euler angles and optional scaling, with 
	// remaining elements taken from the identity matrix. May specify order to apply rotations
	// Matrix is effectively built in this order: M = Scale*Rotation*Translation
	CMatrix4x4
	(
		const CVector3&      position,
		const CVector3&      angles,
		const ERotationOrder eRotOrder = kZXY,
		const CVector3&      scale = CVector3::kOne
	);

	// Construct affine transformation from quaternion and optional position & scaling, with 
	// remaining elements taken from the identity matrix
	// Matrix is effectively built in this order: M = Scale*Rotation*Translation
	explicit CMatrix4x4
	(
		const CQuaternion& quat,
		const CVector3&    position = CVector3::kOrigin,
		const CVector3&    scale = CVector3::kOne
	);
	// Require explicit conversion from CQuaternion only (see above)

	// Construct affine transformation from axis/angle of rotation and optional position & scaling,
	// with remaining elements taken from the identity matrix
	// Matrix is effectively built in this order: M = Scale*Rotation*Translation
	CMatrix4x4
	(
		const CVector3& axis,
		const TFloat32  angle,
		const CVector3& position = CVector3::kOrigin,
		const CVector3& scale = CVector3::kOne
	);


	// Construct from a CMatrix2x2 and optional 2D position, with remaining elements taken from
	// the identity matrix
	explicit CMatrix4x4
	(
		const CMatrix2x2& m,
		const CVector2&   position = CVector2::kOrigin
	);
	// Require explicit conversion from CMatrix2x2 (see above)

	// Construct from a CMatrix3x3 and optional 3D position, with remaining elements from the
	// identity matrix
	explicit CMatrix4x4
	(
		const CMatrix3x3& m,
		const CVector3&   position = CVector3::kOrigin
	);
	// Require explicit conversion from CMatrix3x3 (see above)


	// Copy constructor
    CMatrix4x4( const CMatrix4x4& m );

	// Assignment operator
    CMatrix4x4& operator=( const CMatrix4x4& m );


	/*-----------------------------------------------------------------------------------------
		Setters
	-----------------------------------------------------------------------------------------*/

	// Set by value
	void Set
	(
		const TFloat32 elt00, const TFloat32 elt01, const TFloat32 elt02, const TFloat32 elt03,
		const TFloat32 elt10, const TFloat32 elt11, const TFloat32 elt12, const TFloat32 elt13,
		const TFloat32 elt20, const TFloat32 elt21, const TFloat32 elt22, const TFloat32 elt23,
		const TFloat32 elt30, const TFloat32 elt31, const TFloat32 elt32, const TFloat32 elt33
	);

	// Set through pointer to 16 floats, may specify column/row order of data
	void Set
	(
		const TFloat32* pfElts,
		const bool      bRows = true
	);


	/*-----------------------------------------------------------------------------------------
		Row/column getters & setters
	-----------------------------------------------------------------------------------------*/

	// Get a single row (range 0-3) of the matrix
    CVector4 GetRow( const TUInt32 iRow ) const;

	// Get a single column (range 0-3) of the matrix
    CVector4 GetColumn( const TUInt32 iCol ) const;


	// Set a single row (range 0-3) of the matrix
    void SetRow
	(
		const TUInt32   iRow,
		const CVector4& v
	);

	// Set a single row (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
    void SetRow
	(
		const TUInt32   iRow,
		const CVector3& v
	);


	// Set a single column (range 0-3) of the matrix
    void SetColumn
	(
		const TUInt32   iCol,
		const CVector4& v
	);

	// Set single column (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
    void SetColumn
	(
		const TUInt32   iCol,
		const CVector3& v
	);


	// Set all rows of the matrix at once
    void SetRows
	(
		const CVector4& v0,
		const CVector4& v1,
		const CVector4& v2,
		const CVector4& v3
	);

	// Set all rows of the matrix at once using CVector3, with remaining elements taken from the
	// identity matrix
    void SetRows
	(
		const CVector3& v0,
		const CVector3& v1,
		const CVector3& v2,
		const CVector3& v3
	);


	// Set all columns of the matrix at once
    void SetColumns
	(
		const CVector4& v0,
		const CVector4& v1,
		const CVector4& v2,
		const CVector4& v3
	);

	// Set all columns of the matrix at once using CVector3, with remaining elements taken from the
	// identity matrix
    void SetColumns
	(
		const CVector3& v0,
		const CVector3& v1,
		const CVector3& v2,
		const CVector3& v3
	);


	/*-----------------------------------------------------------------------------------------
		Non-portable row access
	-----------------------------------------------------------------------------------------*/
	// These reinterpreting functions are not guaranteed to be portable, i.e. may not work on all
	// compilers (but will on most). However, they improve efficiency and are highly convenient

	// Direct access to rows as CVector4 - allows two dimensional subscripting of elements
	// Efficient but non-portable. Index in range 0-3 - no validation of index
    CVector4& operator[]( const TUInt32 iRow )
	{
		return *reinterpret_cast<CVector4*>(&e00 + iRow * 4);
	}

	// Direct access to (const) rows as CVector4 - allows two dimensional subscripting of elements
	// Efficient but non-portable. Index in range 0-3 - no validation of index
    const CVector4& operator[](	const TUInt32 iRow ) const
	{
		return *reinterpret_cast<const CVector4*>(&e00 + iRow * 4);
	}


	// Direct access to X-axis of affine transformation as CVector3
	// Efficient but non-portable
	CVector3& XAxis()
	{
		return *reinterpret_cast<CVector3*>(&e00);
	}

	// Direct (const) access to X-axis of affine transformation as CVector3
	// Efficient but non-portable
	const CVector3& XAxis() const
	{
		return *reinterpret_cast<const CVector3*>(&e00);
	}

	// Direct access to Y-axis of affine transformation as CVector3
	// Efficient but non-portable
	CVector3& YAxis()
	{
		return *reinterpret_cast<CVector3*>(&e10);
	}

	// Direct (const) access to Y-axis of affine transformation as CVector3
	// Efficient but non-portable
	const CVector3& YAxis() const
	{
		return *reinterpret_cast<const CVector3*>(&e10);
	}

	// Direct access to Z-axis of affine transformation as CVector3
	// Efficient but non-portable
	CVector3& ZAxis()
	{
		return *reinterpret_cast<CVector3*>(&e20);
	}

	// Direct (const) access to Z-axis of affine transformation as CVector3
	// Efficient but non-portable
	const CVector3& ZAxis() const
	{
		return *reinterpret_cast<const CVector3*>(&e20);
	}


	// Direct access to position (translation) of affine transformation as CVector3
	// Efficient but non-portable
	CVector3& Position()
	{
		return *reinterpret_cast<CVector3*>(&e30);
	}

	// Direct (const) access to position (translation) of affine transformation as CVector3
	// Efficient but non-portable
	const CVector3& Position() const
	{
		return *reinterpret_cast<const CVector3*>(&e30);
	}


	/*-----------------------------------------------------------------------------------------
		Creation and Decomposition of Affine Matrices
	-----------------------------------------------------------------------------------------*/
	// These methods make the assumption that the matrix is/will be composed in this order:
	//     M = Scale*Rotation*Translation
	// Custom code may be needed for other orders or transformations (e.g. shear)

	// Make matrix an affine transformation given position & optional Euler angles & scaling, with
	// remaining elements taken from the identity matrix. May specify order to apply rotations.
	// Matrix is built in this order: M = Scale*Rotation*Translation
	void MakeAffineEuler
	(
		const CVector3&      position,
		const CVector3&      angles = CVector3::kZero,
		const ERotationOrder eRotOrder = kZXY,
		const CVector3&      scale = CVector3::kOne
	);

	// Make matrix an affine transformation given quaternion and optional position & scaling, with 
	// remaining elements taken from the identity matrix
	// Matrix is built in this order: M = Scale*Rotation*Translation
	void MakeAffineQuaternion
	(
		const CQuaternion& quat,
		const CVector3&    position = CVector3::kOrigin,
		const CVector3&    scale = CVector3::kOne
	);

	// Make matrix an affine transformation given angle/axis of rotation and optional position &
	// scaling, with remaining elements taken from the identity matrix
	// Matrix is built in this order: M = Scale*Rotation*Translation
	void MakeAffineAxisAngle
	(
		const CVector3& axis,
		const TFloat32  fAngle,
		const CVector3& position = CVector3::kOrigin,
		const CVector3& scale = CVector3::kOne
	);


	// Decompose affine transformation into position, Euler angles of rotation (X, Y & Z angles of
	// rotation around each axis) and scale. Optionally pass order of rotations. Pass NULL for any
	// unneeded parameters. Assumes matrix is built in this order: M = Scale*Rotation*Translation
    void DecomposeAffineEuler
	(
		CVector3*            pPosition,
		CVector3*            pAngles,
		CVector3*            pScale,
		const ERotationOrder eRotOrder = kZXY
	) const;

	// Decompose affine transformation into position, quaternion of rotation and scale. Pass NULL
	// for any unneeded parameters. Assumes built in this order: M = Scale*Rotation*Translation
    void DecomposeAffineQuaternion
	(
		CVector3*    pPosition,
		CQuaternion* pQuat,
		CVector3*    pScale
	) const;

	// Decompose affine transformation into position, angle/axis of rotation (an axis and amount of
	// rotation around that axis) and scale. Pass NULL for any unneeded parameters. Assumes matrix
	// is built in this order: M = Scale*Rotation*Translation
    void DecomposeAffineAxisAngle
	(
		CVector3* pPosition,
		CVector3* pAxis,
		TFloat32* pfAngle,
		CVector3* pScale
	) const;


	/*-----------------------------------------------------------------------------------------
		Manipulation of Affine Matrices
	-----------------------------------------------------------------------------------------*/
	// All basic operations supported, use other functions for more advanced tasks:
	// To get current rotations (Euler angles) use DecomposeAffineEuler
	// To set specific rotations (Euler angles) use MakeAffineEuler
	// To rotate round an arbitrary (world) axis use:  m *= MakeAffineAxisAngle(...)
	//
	// These methods make the assumption that the matrix is composed in this order:
	//     M = Scale*Rotation*Translation
	// Custom code may be needed for other orders or transformations (e.g. shear)

	// Get the position (translation) of an affine transformation matrix. Similar to GetRow( 3 ),
	// but returning CVector3. Use of Position() function may be more efficient, but non-portable
	CVector3 GetPosition() const
	{
		return CVector3(e30, e31, e32);
	}

	// Set the position (translation) of an affine transformation matrix. Will not change other
	// components of the transformation (rotation, scale etc.). Same as SetRow( 3, p ). Use of
	// Position() function may be more efficient, but non-portable
	void SetPosition( const CVector3& p )
	{
		e30 = p.x;
		e31 = p.y;
		e32 = p.z;
	}

	// Return X position (translation) of an affine transformation matrix
	TFloat32 GetX() const 
	{
		return e30;
	}

	// Return Y position (translation) of an affine transformation matrix
	TFloat32 GetY() const
	{
		return e31;
	}

	// Return Z position (translation) of an affine transformation matrix
	TFloat32 GetZ() const
	{
		return e32;
	}


	// Set X position (translation) of an affine transformation matrix
	void SetX( const TFloat32 x )
	{
		e30 = x;
	}

	// Set Y position (translation) of an affine transformation matrix
	void SetY( const TFloat32 y )
	{
		e31 = y;
	}

	// Set Z position (translation) of an affine transformation matrix
	void SetZ( const TFloat32 z )
	{
		e32 = z;
	}


	// Move position (translation) of an affine transformation matrix by the given vector
	void Move( const CVector3 v ) 
	{
		e30 += v.x;
		e31 += v.y;
		e32 += v.z;
	}

	// Move X position (translation) of an affine transformation matrix
	void MoveX( const TFloat32 x )
	{
		e30 += x;
	}

	// Move Y position (translation) of an affine transformation matrix
	void MoveY( const TFloat32 y )
	{
		e31 += y;
	}

	// Move Z position (translation) of an affine transformation matrix
	void MoveZ( const TFloat32 z )  
	{
		e32 += z;
	}


	// Move position (translation) of an affine transformation matrix by the given vector in its
	// local coordinate space. Will move exactly |v| units, regardless of scaling
	void MoveLocal( const CVector3 v ) 
	{
		// Adjust for any scaling
		TFloat32 scaledX = v.x * InvSqrt( e00*e00 + e01*e01 + e02*e02 );
		TFloat32 scaledY = v.y * InvSqrt( e10*e10 + e11*e11 + e12*e12 );
		TFloat32 scaledZ = v.z * InvSqrt( e20*e20 + e21*e21 + e22*e22 );
		e30 += scaledX * e00 + scaledY * e10 + scaledZ * e20;
		e31 += scaledX * e01 + scaledY * e11 + scaledZ * e21;
		e32 += scaledX * e02 + scaledY * e12 + scaledZ * e22;
	}

	// Move position (translation) of an affine transformation matrix by the given vector in its
	// local coordinate space. Will move relative to the matrix's scaling, i.e. matrix will move 
	// v.x * x-scaling units in x, and similarly in y & z. More efficient than MoveLocal if the 
	// matrix is unscaled
	void MoveLocalWithScaling( const CVector3 v ) 
	{
		e30 += v.x * e00 + v.y * e10 + v.z * e20;
		e31 += v.x * e01 + v.y * e11 + v.z * e21;
		e32 += v.x * e02 + v.y * e12 + v.z * e22;
	}

	// Move X position (translation) of an affine transformation matrix along X axis of the matrix
	// Will move exactly x units, regardless of scaling
	void MoveLocalX( const TFloat32 x ) 
	{
		// Adjust for any x-scaling
		TFloat32 scaledX = x * InvSqrt( e00*e00 + e01*e01 + e02*e02 );
		e30 += scaledX * e00;
		e31 += scaledX * e01;
		e32 += scaledX * e02;
	}

	// Move X position (translation) of an affine transformation matrix along X axis of the matrix
	// Will move relative to the matrix's x-scaling, i.e. will move x * x-scale units
	// More efficient than MoveLocalX if matrix is unscaled
	void MoveLocalXWithScaling( const TFloat32 x ) 
	{
		e30 += x * e00;
		e31 += x * e01;
		e32 += x * e02;
	}

	// Move Y position (translation) of an affine transformation matrix along Y axis of the matrix
	// Will move exactly y units, regardless of scaling
	void MoveLocalY( const TFloat32 y ) 
	{
		// Adjust for any y-scaling
		TFloat32 scaledY = y * InvSqrt( e10*e10 + e11*e11 + e12*e12 );
		e30 += y * e10;
		e31 += y * e11;
		e32 += y * e12;
	}

	// Move Y position (translation) of an affine transformation matrix along Y axis of the matrix
	// Will move relative to the matrix's y-scaling, i.e. will move y * y-scale units
	// More efficient than MoveLocalY if matrix is unscaled
	void MoveLocalYWithScaling( const TFloat32 y ) 
	{
		e30 += y * e10;
		e31 += y * e11;
		e32 += y * e12;
	}

	// Move Z position (translation) of an affine transformation matrix along Z axis of the matrix
	// Will move exactly z units, regardless of scaling
	void MoveLocalZ( const TFloat32 z ) 
	{
		// Adjust for any z-scaling
		TFloat32 scaledZ = z * InvSqrt( e20*e20 + e21*e21 + e22*e22 );
		e30 += scaledZ * e20;
		e31 += scaledZ * e21;
		e32 += scaledZ * e22;
	}

	// Move Z position (translation) of an affine transformation matrix along Z axis of the matrix
	// Will move relative to the matrix's z-scaling, i.e. will move z * z-scale units
	// More efficient than MoveLocalZ if matrix is unscaled
	void MoveLocalZWithScaling( const TFloat32 z ) 
	{
		e30 += z * e20;
		e31 += z * e21;
		e32 += z * e22;
	}


	// Get the X scaling of an affine transformation matrix
	TFloat32 GetScaleX() const
	{
		return Sqrt( e00*e00 + e01*e01 + e02*e02 );
	}

	// Get the Y scaling of an affine transformation matrix
	TFloat32 GetScaleY() const
	{
		return Sqrt( e10*e10 + e11*e11 + e12*e12 );
	}

	// Get the Z scaling of an affine transformation matrix
	TFloat32 GetScaleZ() const
	{
		return Sqrt( e20*e20 + e21*e21 + e22*e22 );
	}

	// Get the X, Y & Z scaling of an affine transformation matrix
    CVector3 GetScale() const;


	// Set the X scaling of an affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
	void SetScaleX( const TFloat32 x );

	// Set the Y scaling of an affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
	void SetScaleY( const TFloat32 y );

	// Set the Z scaling of an affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
	void SetScaleZ( const TFloat32 z );

	// Set the X, Y & Z scaling of an affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
    void SetScale( const CVector3& scale );

	// Set a uniform scaling for an affine transformation matrix. Removes any previous scaling.
	// Will not change other components of the transformation (position, rotation etc.)
    void SetScale( const TFloat32 fScale );


	// Alter the scale of an affine transformation matrix in the (local) X direction. The effect is
	// multiplicative, e.g ScaleX( 2.0f ) indicates a doubling of the X scale
    void ScaleX( const TFloat32 x )
	{
		e00 *= x;
		e01 *= x;
		e02 *= x;
	}

	// Alter the scale of an affine transformation matrix in the (local) Y direction. The effect is
	// multiplicative, e.g ScaleY( 2.0f ) indicates a doubling of the Y scale
    void ScaleY( const TFloat32 y )
	{
		e10 *= y;
		e11 *= y;
		e12 *= y;
	}

	// Alter the scale of an affine transformation matrix in the (local) Z direction. The effect is
	// multiplicative, e.g ScaleZ( 2.0f ) indicates a doubling of the Z scale
    void ScaleZ( const TFloat32 z )
	{
		e20 *= z;
		e21 *= z;
		e22 *= z;
	}

	// Alter the scale of an affine transformation matrix in the (local) X, Y & Z directions. The
	// effect is multiplicative, e.g Scale( CVector3(2.0f, 2.0f, 2.0f) ) indicates a doubling of
	// the scale in all directions
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

	// Alter the scale of an affine transformation matrix uniformly in the (local) X, Y & Z
	// directions. The effect is multiplicative, e.g Scale( 2.0f ) indicates a uniform doubling of
	// the scale
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


	// Rotate an affine transformation matrix by given angle (radians) around world X axis/origin
	// Note: The position (translation) will also rotate round the world X axis, use RotateX to
	// perform a world X rotation around local origin
	void RotateWorldX( const TFloat32 x )
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
		t   = e31*sX + e32*cX;
		e31 = e31*cX - e32*sX;
		e32 = t;
	}

	// Rotate an affine transformation matrix by given angle (radians) around world Y axis/origin
	// Note: The position (translation) will also rotate round the world Y axis, use RotateY to
	// perform a world Y rotation around local origin
	void RotateWorldY( const TFloat32 y )
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
		t   = e30*cY + e32*sY;
		e32 = e32*cY - e30*sY;
		e30 = t;
	}

	// Rotate an affine transformation matrix by given angle (radians) around world Z axis/origin
	// Note: The position (translation) will also rotate round the world Z axis, use RotateZ to
	// perform a world Z rotation around local origin
	void RotateWorldZ( const TFloat32 z )
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
		t   = e30*sZ + e31*cZ;
		e30 = e30*cZ - e31*sZ;
		e31 = t;
	}


	// Rotate an affine transformation by given angle (radians) around world X axis & local origin
	// Note: The position (translation) will not be altered, use RotateWorldX to perform a world X
	// rotation around world origin
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

	// Rotate an affine transformation by given angle (radians) around world Y axis & local origin
	// Note: The position (translation) will not be altered, use RotateWorldY to perform a world Y
	// rotation around world origin
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

	// Rotate an affine transformation by given angle (radians) around world Z axis & local origin
	// Note: The position (translation) will not be altered, use RotateWorldZ to perform a world Z
	// rotation around world origin
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


	// Rotate an affine transformation matrix by given angle (radians) around local X axis/origin
	// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocalXUnscaled
	// if there is no scaling (more efficient)
	void RotateLocalX( const TFloat32 x )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[XRot*Rot]*Trans
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


	// Rotate an affine transformation matrix by given angle (radians) around local X axis/origin
	// Assumes matrix built in the order: M = Rotation*Translation. Use RotateLocalX if there is
	// also scaling (less efficient)
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

	// Rotate an affine transformation matrix by given angle (radians) around local Y axis/origin
	// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocalYUnscaled
	// if there is no scaling (more efficient)
	void RotateLocalY( const TFloat32 y )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[YRot*Rot]*Trans
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

	// Rotate an affine transformation matrix by given angle (radians) around local Y axis/origin
	// Assumes matrix built in the order: M = Rotation*Translation. Use RotateLocalY if there is
	// also scaling (less efficient)
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

	// Rotate an affine transformation matrix by given angle (radians) around local Z axis/origin
	// Assumes matrix built in the order: M = Scale*Rotation*Translation. Use RotateLocalZUnscaled
	// if there is no scaling (more efficient)
	void RotateLocalZ( const TFloat32 z )
	{
		// Need to adjust for scaling component of matrix - matrix assumed to be Scale*Rot*Trans,
		// becoming Scale*[ZRot*Rot]*Translation
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

	// Rotate an affine transformation matrix by given angle (radians) around local Z axis/origin
	// Assumes matrix built in the order: M = Rotation*Translation. Use RotateLocalZ if there is
	// also scaling (less efficient)
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
	// other. Most 3D transformation matrices contain orthogonal/orthonormal vectors in the
	// upper 3x3 part of the matrix (occasional exception of projection matrices). As an example,
	// recall that the first 3 rows (or columns) of a world matrix define the local X, Y & Z axes
	// for the model - expected to be at right angles. 3D transformations that are not orthogonal
	// in this way will skew or distort models they are applied to. Unfortunately, floating point
	// rounding can cause very persitent (long-lived) matrices to gradually becom non-orthogonal.
	// Testing for orthogonality and correction is periodically required in such cases.
	// [N.B. Formally, a *matrix* is orthogonal only if *all* its rows/columns are *orthonormal*,
	// and there is no special name for a matrix with just orthogonal rows/columns. So these
	// are properly tests of vectors contained in the matrix, not tests of the matrix itself]

	// Test if upper-left 3x3 matrix has orthogonal rows, i.e. if the three rows are vectors
	// at right angles to each other. Will also be orthonormal if it contains no scaling
	bool IsOrthogonal3x3() const;

	// Test if upper-left 3x3 matrix has orthonormal rows, i.e. if the three rows are *normals*
	// at right angles to each other. Scaled matrices cannot be orthonormal (can be orthogonal)
	bool IsOrthonormal3x3() const;

	// Orthogonalise the rows/columns of the upper-left 3x3 matrix. Generally used to "correct"
	// matrices that become non-orthogonal after repeated calculations/floating point rounding
	// May pass scaling for resultant rows - default is 1, leading to "orthonormalisation"
	void Orthogonalise3x3(	const CVector3& scale = CVector3::kOne );


	/*-----------------------------------------------------------------------------------------
		Inverse related
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	// Set this matrix to its transpose (matrix reflected through its diagonal)
	// This is also the (most efficient) inverse for a rotation matrix
    void Transpose();

	// Set this matrix to its inverse assuming it is affine with an orthogonal upper-left 3x3
	// matrix i.e. an affine transformation with no scaling or shear
	// Most efficient inverse for transformations containing rotation and translation only
    void InvertRotTrans();

	// Set this matrix to its inverse assuming it is affine and the upper-left 3x3 matrix contains
	// orthogonal vectors i.e. an affine transformation matrix with no shear
	// Most efficient inverse for transformations containing rotation, translation & scale only
    void InvertRotTransScale();

	// Set this matrix to its inverse assuming only that it is an affine matrix
	void InvertAffine();

	// Return the cofactor of entry i,j. This is (-1)^(i+j) * determinant of the matrix after
	// removing the ith and jth row/column. Used for calculating general inverse
	TFloat32 Cofactor( const TUInt32 i, const TUInt32 j );

	// Set this matrix to its inverse. Most general, least efficient inverse function
	// Suitable for non-affine matrices (e.g. a perspective projection matrix)
	void Invert();


	/*-----------------------------------------------------------------------------------------
		Transformation Matrices
	-----------------------------------------------------------------------------------------*/
	// Non-member transformation creation functions given after the class definition

	// Make this matrix the identity matrix
    void MakeIdentity();


	// Make this matrix a translation by the given vector
	void MakeTranslation( const CVector3& translate );
    

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
		Facing Matrices
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	// Make this matrix an affine 3D transformation matrix to face from current position to given
	// target (in the Z direction). Can pass up vector for the constructed matrix and specify
	// handedness (right-handed Z axis will face away from target)
	// Will retain the matrix's current scaling
    void FaceTarget
	(
		const CVector3& target,
		const CVector3& up = CVector3::kYAxis,
		const bool      bLH = true
	);

	// Make this matrix an affine 3D transformation matrix that faces in given direction from the
	// current position (Z axis). Can pass up vector for the constructed matrix and specify
	// handedness (right-handed Z axis will face away from direction)
	// Will retain the matrix's current scaling
    void FaceDirection
	(
		const CVector3& direction,
		const CVector3& up = CVector3::kYAxis,
		const bool      bLH = true
	);

	/*-----------------------------------------------------------------------------------------
		Member Operators
	-----------------------------------------------------------------------------------------*/
	// Non-member versions given after the class definition

	//////////////////////////////////
	// Scalar multiplication/division

    // Scalar multiplication
    CMatrix4x4& operator*=( const TFloat32 s );

    // Scalar division
    CMatrix4x4& operator/=( const TFloat32 s );


	///////////////////////////////
	// Vector multiplication

	// Return the given vector transformed by this matrix (pre-multiplication: V' = V*M)
    CVector4 Transform( const CVector4& v ) const;

	// Return the given CVector3 transformed by this matrix (pre-multiplication: V' = V*M)
	// Assuming it is a vector rather then a point, i.e. assume the vector's 4th element is 0
    CVector3 TransformVector( const CVector3& v ) const;
    
	// Return the given CVector3 transformed by this matrix (pre-multiplication: V' = V*M)
	// Assuming it is a point rather then a vector, i.e. assume the vector's 4th element is 1
    CVector3 TransformPoint( const CVector3& p ) const;


	///////////////////////////////
	// Matrix multiplication

	// Post-multiply this matrix by the given one
    CMatrix4x4& operator*=( const CMatrix4x4& m );
	

	// Post-multiply this matrix by the given one, assuming they are both affine
	CMatrix4x4& MultiplyAffine( const CMatrix4x4& m );


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// Matrix elements
	TFloat32 e00, e01, e02, e03;
	TFloat32 e10, e11, e12, e13;
	TFloat32 e20, e21, e22, e23;
	TFloat32 e30, e31, e32, e33;

	// Standard matrices
	static const CMatrix4x4 kIdentity;
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
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
);

// Matrix inequality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
bool operator!=
(
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
);


//////////////////////////////////
// Scalar multiplication/division

// Scalar-matrix multiplication
CMatrix4x4 operator*
(
	const TFloat32    s,
	const CMatrix4x4& m
);

// Matrix-scalar multiplication
CMatrix4x4 operator*
(
	const CMatrix4x4& m,
	const TFloat32    s
);

// Matrix-scalar division
CMatrix4x4 operator/
(
	const CMatrix4x4& m,
	const TFloat32    s
);


///////////////////////////////
// Vector multiplication

// Vector-matrix multiplication (order is important - this is usual order for transformation
// for matrices stored as row vectors - see notes at top)
CVector4 operator*
(
	const CVector4&   v,
	const CMatrix4x4& m
);

// Matrix-vector multiplication (order is important - this is an unusual order for matrices
// stored as row vectors - see notes at top)
CVector4 operator*
(
	const CMatrix4x4& m,
	const CVector4&   v
);


///////////////////////////////
// Matrix multiplication

// General matrix-matrix multiplication
CMatrix4x4 operator*
(
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
);


// Matrix-matrix multiplication assuming both matrices are affine
CMatrix4x4 MultiplyAffine
(
	const CMatrix4x4& m1,
	const CMatrix4x4& m2
);


/*-----------------------------------------------------------------------------------------
	Non-Member Othogonality
-----------------------------------------------------------------------------------------*/

// Return a copy of given matrix with orthogonalised rows/columns in the upper-left 3x3 matrix.
// Generally used to "correct" matrices that become non-orthogonal after repeated calculations /
// floating point rounding. May pass scaling for resultant rows - default is 1, leading to
// "orthonormalisation"
CMatrix4x4 Orthogonalise3x3
(
	const CMatrix4x4& m,
	const CVector3&   scale = CVector3::kOne
);

	
/*-----------------------------------------------------------------------------------------
	Non-member Inverse Related
-----------------------------------------------------------------------------------------*/

// Return the transpose of given matrix (matrix reflected through its diagonal)
// This is also the (most efficient) inverse for a rotation matrix
CMatrix4x4 Transpose( const CMatrix4x4& m );

// Return the inverse of given matrix assuming it is affine with an orthogonal upper-left 3x3
// matrix i.e. an affine transformation with no scaling or shear
// Most efficient inverse for transformations containing rotation and translation only
CMatrix4x4 InverseRotTrans( const CMatrix4x4& m );

// Return the inverse of given matrix assuming it is affine and the upper-left 3x3 matrix
// contains orthogonal vectors i.e. an affine transformation matrix with no shear
// Most efficient inverse for transformations containing rotation, translation & scale only
CMatrix4x4 InverseRotTransScale( const CMatrix4x4& m );

// Return the inverse of given matrix assuming only that it is an affine matrix
CMatrix4x4 InverseAffine( const CMatrix4x4& m );

// Return the cofactor of entry i,j of the given matrix. This is (-1)^(i+j) * determinant of
// the matrix after removing the ith and jth row/column. Used for calculating general inverse
TFloat32 Cofactor
(
	const CMatrix4x4& m,
	const TUInt32     i,
	const TUInt32     j
);

// Return the inverse of given matrix. Most general, least efficient inverse function.
// Suitable for non-affine matrices (e.g. a perspective projection matrix)
CMatrix4x4 Inverse( const CMatrix4x4& m );


/*-----------------------------------------------------------------------------------------
	Transformation Matrices
-----------------------------------------------------------------------------------------*/
// Same as class member functions, but these return a new matrix (by value). Can be used
// as temporaries in calculations, e.g.
//     CMatrix4x4 m = MatrixScaling( 3.0f ) * MatrixTranslation( CVector3(10.0f, -10.0f, 20.0f) );

// Return an identity matrix
CMatrix4x4 MatrixIdentity();

// Return an affine translation matrix of the given vector
CMatrix4x4 MatrixTranslation( const CVector3& translate );

// Return an X-axis rotation matrix of the given angle
CMatrix4x4 MatrixRotationX( const TFloat32 x );

// Return a Y-axis rotation matrix of the given angle
CMatrix4x4 MatrixRotationY( const TFloat32 y );

// Return a Z-axis rotation matrix of the given angle
CMatrix4x4 MatrixRotationZ( const TFloat32 z );

// Make a matrix that is a combined rotation around the X, Y & Z axes by the given angles
// (radians), applied in the order specified
CMatrix4x4 MatrixRotation
( 
	const CVector3       angles,
	const ERotationOrder eRotOrder = kZXY
);

// Return a matrix that is a rotation around the given axis of the given angle (radians)
CMatrix4x4 MatrixRotation
(
	const CVector3& axis,
	const TFloat32  fAngle
);


// Return a matrix that is a scaling in X,Y and Z of the values provided in the given vector
CMatrix4x4 MatrixScaling( const CVector3& scale );

// Return a matrix that is a uniform scaling of the given amount
CMatrix4x4 MatrixScaling( const TFloat32 fScale );


/*-----------------------------------------------------------------------------------------
	Facing Matrices
-----------------------------------------------------------------------------------------*/

// Create affine 3D transformation matrix to face from given position to given target (in the Z
// direction). Can pass up vector for the constructed matrix and specify handedness (right-handed
// Z axis will face away from target)
CMatrix4x4 MatrixFaceTarget
(
	const CVector3& position,
	const CVector3& target,
	const CVector3& up = CVector3::kYAxis,
	const bool      bLH = true
);

// Create affine 3D transformation matrix that faces in given direction from the given position
// (Z axis). Can pass up vector for the constructed matrix and specify handedness (right-handed
// Z axis will face away from direction)
CMatrix4x4 MatrixFaceDirection
(
	const CVector3& position,
	const CVector3& direction,
	const CVector3& up = CVector3::kYAxis,
	const bool      bLH = true
);


} // namespace gen

#endif // GEN_C_MATRIX_4X4_H_INCLUDED
