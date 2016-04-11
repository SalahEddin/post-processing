/**************************************************************************************************
	Module:       CVector3.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Definition of the concrete class CVector3, three 32-bit floats representing a vector/point
	with x, y & z components - or a column/row of a 3x3 matrix

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#ifndef GEN_C_VECTOR_3_H_INCLUDED
#define GEN_C_VECTOR_3_H_INCLUDED

#include "Defines.h"
#include "Error.h"
#include "BaseMath.h"
#include "CVector2.h"

namespace gen
{

// Forward declaration of classes, where includes are only possible/necessary in the .cpp file
class CVector4;


class CVector3
{
	GEN_CLASS( CVector3 );

// Concrete class - public access
public:

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised (for performance)
	CVector3() {}

	// Construct by value
	CVector3
	(
		const TFloat32 xIn,
		const TFloat32 yIn,
		const TFloat32 zIn
	) : x( xIn ), y( yIn ), z( zIn )
	{}

	// Construct through pointer to three floats
	explicit CVector3( const TFloat32* pfElts )
	{
		GEN_GUARD_OPT;
		GEN_ASSERT_OPT( pfElts, "Invalid parameter" );

		x = pfElts[0];
		y = pfElts[1];
		z = pfElts[2];

		GEN_ENDGUARD_OPT;
	}
	// 'explicit' disallows implicit conversion:  TFloat32* pf; CVector3 v = pf;
	// Need to use this constructor explicitly:   TFloat32* pf; CVector3 v = CVector3(pf);
	// Only applies to constructors that can take one parameter, used to avoid confusing code


	// Construct as vector between two points (p1 to p2)
	CVector3
	(
		const CVector3& p1,
		const CVector3& p2
	) : x( p2.x - p1.x ), y( p2.y - p1.y ), z( p2.z - p1.z )
	{}


	// Construct from a CVector2 and a z value (defaults to 0)
	explicit CVector3
	(
		const CVector2& v,
		const TFloat32 zIn = 0.0f
	) : x( v.x ), y( v.y ), z( zIn )
	{}
	// Require explicit conversion from CVector2 (see above)

	// Construct from a CVector4, discarding w value
	explicit CVector3( const CVector4& v );
	// Require explicit conversion from CVector4 (see above)


	// Copy constructor, construct from CVector3
    CVector3( const CVector3& v ) : x( v.x ), y( v.y ), z( v.z )
	{}

	// Assignment operator
    CVector3& operator=( const CVector3& v )
	{
		if ( this != &v )
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}


	/*-----------------------------------------------------------------------------------------
		Setters
	-----------------------------------------------------------------------------------------*/

	// Set all three vector components
    void Set
	(
		const TFloat32 xIn,
		const TFloat32 yIn,
		const TFloat32 zIn
	)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}

	// Set the vector through a pointer to three floats
    void Set( const TFloat32* pfElts )
	{
		x = pfElts[0];
		y = pfElts[1];
		z = pfElts[2];
	}

	// Set as vector between two points (p1 to p2)
    void Set
	(
		const CVector3& p1,
		const CVector3& p2
	)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
		z = p2.z - p1.z;
	}

	// Set the vector to (0,0,0)
    void SetZero()
	{
		x = y = z = 0.0f;
	}


	/*-----------------------------------------------------------------------------------------
		Array access
	-----------------------------------------------------------------------------------------*/

	// Access the x, y & z components in array style (i.e. v[0], v[1], v[2] same as v.x, v.y, v.z)
	// No validation on index
    TFloat32& operator[]( const TUInt32 index )
	{
		return (&x)[index];
	}

	// Access the x, y & z elements in array style - const result
	// No validation on index
	const TFloat32& operator[]( const TUInt32 index ) const
	{
		return (&x)[index];
	}


	/*-----------------------------------------------------------------------------------------
		Comparisons
	-----------------------------------------------------------------------------------------*/
	// Equality operators defined as non-member operations after the class definition

	// Test if the vector is zero length (i.e. = (0,0,0))
	// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
	bool IsZero() const
	{
		return gen::IsZero( x*x + y*y + z*z );
	}

	// Test if the vector is unit length (normalised)
	// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
	bool IsUnit() const
	{
		return gen::IsZero( x*x + y*y + z*z - 1.0f );
	}


	/*-----------------------------------------------------------------------------------------
		Member Operators
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	///////////////////////////////
	// Reinterpretation

	// Reinterpreting vector types is not guaranteed to be portable - but improves
	// efficiency and is highly convenient

	// Return reference to x & y components as CVector2. Efficient but non-portable
	CVector2& Vector2()
	{
		return *reinterpret_cast<CVector2*>(&x);
	}

	// Return const reference to x & y components as CVector2. Efficient but non-portable
	const CVector2& Vector2() const
	{
		return *reinterpret_cast<const CVector2*>(&x);
	}


	///////////////////////////////
	// Addition / subtraction

	// Add another vector to this vector
    CVector3& operator+=( const CVector3& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	// Subtract another vector from this vector
    CVector3& operator-=( const CVector3& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}


	////////////////////////////////////
	// Scalar multiplication & division

	// Multiply this vector by a scalar
	CVector3& operator*=( const TFloat32 s )
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	// Divide this vector by a scalar
    CVector3& operator/=( const TFloat32 s )
	{
		GEN_GUARD_OPT;
		GEN_ASSERT_OPT( !gen::IsZero(s), "Invalid parameter" );

		x /= s;
		y /= s;
		z /= s;
		return *this;

		GEN_ENDGUARD_OPT;
	}


	////////////////////////////////////
	// Other operations

	// Dot product of this with another vector
    TFloat32 Dot( const CVector3& v ) const
	{
	    return x*v.x + y*v.y + z*v.z;
	}
	
	
	// Cross product of this with another vector
    CVector3 Cross( const CVector3& v ) const
	{
		return CVector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}


	/*-----------------------------------------------------------------------------------------
		Length operations
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	// Return length of this vector
	TFloat32 Length() const
	{
		return Sqrt( x*x + y*y + z*z );
	}

	// Return squared length of this vector
	// More efficient than Length when exact value is not required (e.g. for comparisons)
	// Use InvSqrt( LengthSquared(...) ) to calculate 1 / length more efficiently
	TFloat32 LengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	// Reduce vector to unit length
    void Normalise();


	/*-----------------------------------------------------------------------------------------
		Point related functions
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	// Return distance from this point to another
    TFloat32 DistanceTo( const CVector3& p );

	// Return squared distance from this point to another
	// More efficient than Distance when exact length is not required (e.g. for comparisons)
	// Use InvSqrt( DistanceToSquared(...) ) to calculate 1 / distance more efficiently
	TFloat32 DistanceToSquared( const CVector3& p );


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/
    
    // Vector components
    TFloat32 x;
	TFloat32 y;
	TFloat32 z;

	// Standard vectors
	static const CVector3 kZero;
	static const CVector3 kOne;
	static const CVector3 kOrigin;
	static const CVector3 kXAxis;
	static const CVector3 kYAxis;
	static const CVector3 kZAxis;
};


/*-----------------------------------------------------------------------------------------
	Non-member Operators
-----------------------------------------------------------------------------------------*/

///////////////////////////////
// Comparison

// Vector equality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
inline bool operator==
(
	const CVector3& v1,
	const CVector3& v2
)
{
	return AreEqual( v1.x, v2.x ) && AreEqual( v1.y, v2.y ) && AreEqual( v1.z, v2.z );
}

// Vector inequality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
inline bool operator!=
(
	const CVector3& v1,
	const CVector3& v2
)
{
	return !AreEqual( v1.x, v2.x ) || !AreEqual( v1.y, v2.y ) || !AreEqual( v1.z, v2.z );
}


///////////////////////////////
// Addition / subtraction

// Vector addition
inline CVector3 operator+
(
	const CVector3& v1,
	const CVector3& v2
)
{
	return CVector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// Vector subtraction
inline CVector3 operator-
(
	const CVector3& v1,
	const CVector3& v2
)
{
	return CVector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

// Unary positive (i.e. a = +v, included for completeness)
inline CVector3 operator+( const CVector3& v )
{
	return v;
}

// Unary negation (i.e. a = -v)
inline CVector3 operator-( const CVector3& v )
{
	return CVector3(-v.x, -v.y, -v.z);
}


////////////////////////////////////
// Scalar multiplication & division

// Vector multiplied by scalar
inline CVector3 operator*
(
	const CVector3& v,
	const TFloat32  s
)
{
	return CVector3(v.x*s, v.y*s, v.z*s);
}

// Scalar multiplied by vector
inline CVector3 operator*
(
	const TFloat32  s,
	const CVector3& v
)
{
	return CVector3(v.x*s, v.y*s, v.z*s);
}

// Vector divided by scalar
inline CVector3 operator/
(
	const CVector3& v,
	const TFloat32  s
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	return CVector3(v.x/s, v.y/s, v.z/s);

	GEN_ENDGUARD_OPT;
}


////////////////////////////////////
// Other operations

// Dot product of two given vectors (order not important) - non-member version
inline TFloat32 Dot
(
	const CVector3& v1,
	const CVector3& v2
)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

// Cross product of two given vectors (order is important) - non-member version
inline CVector3 Cross
(
	const CVector3& v1,
	const CVector3& v2
)
{
	return CVector3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


/*-----------------------------------------------------------------------------------------
	Non-Member Length operations
-----------------------------------------------------------------------------------------*/

// Return length of given vector
inline TFloat32 Length( const CVector3& v )
{
	return Sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
}

// Return squared length of given vector
// More efficient than Length when exact value is not required (e.g. for comparisons)
// Use InvSqrt( LengthSquared(...) ) to calculate 1 / length more efficiently
inline TFloat32 LengthSquared( const CVector3& v )
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

// Return unit length vector in the same direction as given one
CVector3 Normalise( const CVector3& v );


/*-----------------------------------------------------------------------------------------
	Non-member point related functions
-----------------------------------------------------------------------------------------*/

// Return distance from one point to another - non-member version
TFloat32 Distance
(
	const CVector3& p1,
	const CVector3& p2
);

// Return squared distance from one point to another - non-member version
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceSquared(...) ) to calculate 1 / distance more efficiently
TFloat32 DistanceSquared
(
	const CVector3& p1,
	const CVector3& p2
);


} // namespace gen

#endif // GEN_C_VECTOR_3_H_INCLUDED
