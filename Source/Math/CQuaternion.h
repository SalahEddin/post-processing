/**************************************************************************************************
	Module:       CQuaternion.h
	Author:       Laurent Noel
	Date created: 23/06/06

	Definition of the concrete class CQuaternion, four 32-bit floats representing a quaternion

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 23/06/06 - LN
**************************************************************************************************/

#ifndef GEN_C_QUATERNION_H_INCLUDED
#define GEN_C_QUATERNION_H_INCLUDED

#include "Defines.h"
#include "BaseMath.h"
#include "CVector3.h"
#include "CMatrix4x4.h"

namespace gen
{


class CQuaternion
{
// Concrete class - public access
public:

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised
	CQuaternion() {}

	// Construct by value - four floats
	CQuaternion
	(
		const TFloat32 initW,
		const TFloat32 initX,
		const TFloat32 initY,
		const TFloat32 initZ
	) : w( initW ), x( initX ), y( initY ), z( initZ ) {}

	// Construct by value - float and CVector3
	CQuaternion
	(
		const TFloat32 initW,
		const CVector3 initV
	) : w( initW ), x( initV.x ), y( initV.y ), z( initV.z ) {}

	// Construct through pointer to four floats
	// Specifying explicit avoids defining an implicit conversion
	explicit CQuaternion
	(
		const TFloat32* pWXYZ
	) : w( pWXYZ[0] ), x( pWXYZ[1] ), y( pWXYZ[2] ), z( pWXYZ[3] ) {}

 	// Construct from a CVector3 - w value becomes 0
	explicit CQuaternion
	(
		const CVector3& src
	) : w( 0.0f ), x( src.x ), y( src.y ), z( src.z ) {};

 	// Construct from a CMatrix4x4 - uses upper left 3x3 only
	explicit CQuaternion
	(
		const CMatrix4x4& mat
	);


	// Copy constructor
    CQuaternion
	(
		const CQuaternion& src
	) : w( src.w ), x( src.x ), y( src.y ), z( src.z ) {}

	// Assignment operator
    CQuaternion& operator=
	(
		const CQuaternion& src
	)
	{
		if ( this != &src )
		{
			w = src.w;
			x = src.x;
			y = src.y;
			z = src.z;
		}
		return *this;
	}

	// Destructor
	~CQuaternion() {}


	/*-----------------------------------------------------------------------------------------
		Setters
	-----------------------------------------------------------------------------------------*/

	// Set all four quaternion components
    void Set
	(
		const TFloat32 setW,
		const TFloat32 setX,
		const TFloat32 setY,
		const TFloat32 setZ
	)
	{
		w = setW;
		x = setX;
		y = setY;
		z = setZ;
	}

	// Set all four quaternion components from float and CVector3
    void Set
	(
		const TFloat32 setW,
		const CVector3 setV
	)
	{
		w = setW;
		x = setV.x;
		y = setV.y;
		z = setV.z;
	}

	// Set the quaternion through a pointer to four floats
    void Set
	(
		const TFloat32* pXYZ
	)
	{
		w = pXYZ[0];
		x = pXYZ[1];
		y = pXYZ[2];
		z = pXYZ[3];
	}

	// Set the quaternion to (0,0,0,0)
    void SetZero()
	{
		w = x = y = z = 0.0f;
	}

	// Set the quaternion to the idendity (1,0,0,0)
    void SetIdentity()
	{
		w = 1.0f;
		x = y = z = 0.0f;
	}


	/*-----------------------------------------------------------------------------------------
		Vector access
	-----------------------------------------------------------------------------------------*/
	// These functions are not guaranteed to be portable - but they improve efficiency and are
	// highly convenient

	// Access the x, y & z components as a CVector3
    CVector3& Vector()
	{
		return *reinterpret_cast<CVector3*>(&x);
	}

	// Access the x, y & z components as a const CVector3
    const CVector3& Vector() const
	{
		return *reinterpret_cast<const CVector3*>(&x);
	}

	// Access the x, y & z components in array style (e.g. q[1] same as q.y)
	// Note this does not access the w component
    TFloat32& operator[]
	(
		const TUInt32 index
	)
	{
		return (&x)[index];
	}

	// Access the x, y & z elements in array style - const result
	// Note this does not access the w component
	const TFloat32& operator[]
	(
		const TUInt32 index
	) const
	{
		return (&x)[index];
	}


	/*-----------------------------------------------------------------------------------------
		Comparisons
	-----------------------------------------------------------------------------------------*/
	// Equality operators defined as non-member operations after the class definition

	// Test if the quaternion is zero length (i.e. = (0,0,0,0))
	bool IsZero() const
	{
		return gen::IsZero( w*w + x*x + y*y + z*z );
	}

	// Test if the quaternion is the identity (i.e. = (1,0,0,0))
	bool IsIdentity() const
	{
		return gen::AreEqual( w, 1.0f ) &&
		       gen::IsZero( x ) && gen::IsZero( y ) && gen::IsZero( z );
	}

	// Test if the quaternion is unit length (normalised)
	bool IsUnit() const
	{
		return gen::IsZero( w*w + x*x + y*y + z*z - 1.0f );
	}


	/*-----------------------------------------------------------------------------------------
		Member Operators
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	///////////////////////////////
	// Casting

	// These functions are not guaranteed to be portable - but improve efficiency and are 
	// highly convenient

	// Cast to CVector3 - w component is dropped
	operator CVector3()
	{
		return *reinterpret_cast<CVector3*>(&x);
	}

	// Cast to const CVector3 - w component is dropped
	operator const CVector3() const
	{
		return *reinterpret_cast<const CVector3*>(&x);
	}


	///////////////////////////////
	// Addition / subtraction

	// Add another quaternion to this quaternion
    CQuaternion& operator+=
	(
		const CQuaternion& quat
	)
	{
		w += quat.w;
		x += quat.x;
		y += quat.y;
		z += quat.z;
		return *this;
	}

	// Subtract another quaternion from this quaternion
    CQuaternion& operator-=
	(
		const CQuaternion& quat
	)
	{
		w -= quat.w;
		x -= quat.x;
		y -= quat.y;
		z -= quat.z;
		return *this;
	}


	////////////////////////////////////
	// Scalar multiplication & division

	// Multiply this quaternion by a scalar
	CQuaternion& operator*=
	(
		const TFloat32 scalar
	)
	{
		w *= scalar;
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// Divide this quaternion by a scalar
    CQuaternion& operator/=
	(
		const TFloat32 scalar
	)
	{
		w /= scalar;
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}


	////////////////////////////////////
	// Quaternion multiplication

	// Binary form as friend to define function below
	friend CQuaternion operator*
	(
		const CQuaternion& quat1,
		const CQuaternion& quat2
	);

	// Multiply this quaternion by another
    CQuaternion& operator*=
	(
		const CQuaternion& quat
	)
	{
		*this = (*this) * quat;
		return *this;
	}


	////////////////////////////////////
	// Other operations

	// Dot product of this with another quaternion
    TFloat32 Dot
	(
		const CQuaternion& quat
	) const
	{
	    return w*quat.w + x*quat.x + y*quat.y + z*quat.z;
	}
		

	/*-----------------------------------------------------------------------------------------
		Length operations
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	// Return the magnitude or norm of the quaternion - equivalent to its length as a 4-vector
	TFloat32 Norm() const
	{
		return Sqrt( w*w + x*x + y*y + z*z );
	}

	// Return squared norm of this quaternion
	TFloat32 NormSquared() const
	{
		return w*w + x*x + y*y + z*z;
	}

	// Normalise the quaternion - make it unit length as a 4-vector
    void Normalise();


	/*-----------------------------------------------------------------------------------------
		Inverse functions
	-----------------------------------------------------------------------------------------*/

	// Set this quaternion to its inverse
	void SetInverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	// Return the inverse of this quaternion
	CQuaternion Inverse() const
	{
		return CQuaternion( w, -x, -y, -z );
	}


	/*-----------------------------------------------------------------------------------------
		Vector transformation
	-----------------------------------------------------------------------------------------*/

	// Rotate a CVector3 by this quaternion
	CVector3 Rotate
	(
		const CVector3& vec
	) const;


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/
    
	// Standard quaternions
	static const CQuaternion kZero;
	static const CQuaternion kIdentity;

    // Quaternion components
    TFloat32 w;
	TFloat32 x;
	TFloat32 y;
	TFloat32 z;
};


/*-----------------------------------------------------------------------------------------
	Non-member Operators
-----------------------------------------------------------------------------------------*/

///////////////////////////////
// Comparison

// Vector equality
inline bool operator==
(
	const CQuaternion& cmp1,
	const CQuaternion& cmp2
)
{
	return AreEqual( cmp1.w, cmp2.w ) && AreEqual( cmp1.x, cmp2.x ) && 
	       AreEqual( cmp1.y, cmp2.y ) && AreEqual( cmp1.z, cmp2.z );
}

// Vector inequality
inline bool operator!=
(
	const CQuaternion& cmp1,
	const CQuaternion& cmp2
)
{
	return !AreEqual( cmp1.w, cmp2.w ) || !AreEqual( cmp1.x, cmp2.x ) ||
		   !AreEqual( cmp1.y, cmp2.y ) || !AreEqual( cmp1.z, cmp2.z );
}


///////////////////////////////
// Addition / subtraction

// Quaternion addition
inline CQuaternion operator+
(
	const CQuaternion& quat1,
	const CQuaternion& quat2
)
{
	return CQuaternion( quat1.w+quat2.w, quat1.x+quat2.x, quat1.y+quat2.y, quat1.z+quat2.z );
}

// Quaternion subtraction
inline CQuaternion operator-
(
	const CQuaternion& quat1,
	const CQuaternion& quat2
)
{
	return CQuaternion( quat1.w-quat2.w, quat1.x-quat2.x, quat1.y-quat2.y, quat1.z-quat2.z );
}

// Unary positive (for completeness)
inline CQuaternion operator+
(
	const CQuaternion& quat
)
{
	return CQuaternion( quat );
}

// Unary negation
inline CQuaternion operator-
(
	const CQuaternion& quat
)
{
	return CQuaternion( -quat.w, -quat.x, -quat.y, -quat.z );
}


////////////////////////////////////
// Scalar multiplication & division

// Quaternion multiplied by scalar
inline CQuaternion operator*
(
	const CQuaternion& quat,
	const TFloat32     scalar
)
{
	return CQuaternion( quat.w*scalar, quat.x*scalar, quat.y*scalar, quat.z*scalar );
}

// Scalar multiplied by quaternion
inline CQuaternion operator*
(
	const TFloat32     scalar,
	const CQuaternion& quat
)
{
	return CQuaternion( quat.w*scalar, quat.x*scalar, quat.y*scalar, quat.z*scalar );
}

// Quaternion divided by scalar
inline CQuaternion operator/
(
	const CQuaternion& quat,
	const TFloat32     scalar
)
{
	return CQuaternion( quat.w/scalar, quat.x/scalar, quat.y/scalar, quat.z/scalar );
}


////////////////////////////////////
// Quaternion multiplication

// Return the quaternion result of multiplying two quaternions
CQuaternion operator*
(
	const CQuaternion& quat1,
	const CQuaternion& quat2
);


////////////////////////////////////
// Other operations

// Dot product of two given quaternions (order not important) - non-member version
inline TFloat32 Dot
(
	const CQuaternion& quat1,
	const CQuaternion& quat2
)
{
    return quat1.w*quat2.w + quat1.x*quat2.x + quat1.y*quat2.y + quat1.z*quat2.z;
}


/*-----------------------------------------------------------------------------------------
	Non-member length operations
-----------------------------------------------------------------------------------------*/

// Return the magnitude or norm of a quaternion (its length as a 4-vector) - non-member version
inline TFloat32 Norm
(
	const CQuaternion& quat
)
{
	return Sqrt( quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z );
}

// Return squared norm of a quaternion - non-member version
inline TFloat32 NormSquared
(
	const CQuaternion& quat
)
{
	return quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;
}

// Return a normalised version of a quaternion (unit length as a 4-vector) - non-member version
CQuaternion Normalise
(
	const CQuaternion& quat
);


/*---------------------------------------------------------------------------------------------
	Interpolation
---------------------------------------------------------------------------------------------*/

// Linear interpolation of two quaternions q0 and q1, with parameter t, result in qt
// Result may not be normalised
void Lerp
(
	const CQuaternion& q0,
	const CQuaternion& q1,
	const TFloat32     t,
	CQuaternion&       qt
);

// Linear interpolation of two quaternions q0 and q1, with parameter t, result in qt
// Result is normalised to give an approximation to slerp
void NLerp
(
	const CQuaternion& q0,
	const CQuaternion& q1,
	const TFloat32     t,
	CQuaternion&       qt
);

// Spherical linear interpolation of two quaternions q0 and q1, with parameter t, result in qt
void Slerp
(
	const CQuaternion& q0,
	const CQuaternion& q1,
	const TFloat32     t,
	CQuaternion&       qt
);


} // namespace gen

#endif // GEN_C_QUATERNION_H_INCLUDED
