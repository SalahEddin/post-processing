/**************************************************************************************************
	Module:       CVector2.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Definition of the concrete class CVector2, two 32-bit floats representing a vector/point
	with x & y components - or a column/row of a 2x2 matrix

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#ifndef GEN_C_VECTOR_2_H_INCLUDED
#define GEN_C_VECTOR_2_H_INCLUDED

#include "Defines.h"
#include "Error.h"
#include "BaseMath.h"

namespace gen
{

// Forward declaration of classes, where includes are only possible/necessary in the .cpp file
class CVector3;
class CVector4;


class CVector2
{
	GEN_CLASS( CVector2 );

// Concrete class - public access
public:

	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - leaves values uninitialised (for performance)
	CVector2() {}

	// Construct by value
	CVector2
	(
		const TFloat32 xIn,
		const TFloat32 yIn
	) : x( xIn ), y( yIn )
	{}

	// Construct through pointer to two floats
	explicit CVector2( const TFloat32* pfElts )
	{
		GEN_GUARD_OPT;
		GEN_ASSERT_OPT( pfElts, "Invalid parameter" );

		x = pfElts[0];
		y = pfElts[1];

		GEN_ENDGUARD_OPT;
	}
	// 'explicit' disallows implicit conversion:  TFloat32* pf; CVector2 v = pf;
	// Need to use this constructor explicitly:   TFloat32* pf; CVector2 v = CVector2(pf);
	// Only applies to constructors that can take one parameter, used to avoid confusing code


	// Construct as vector between two points (p1 to p2)
	CVector2
	(
		const CVector2& p1,
		const CVector2& p2
	) : x( p2.x - p1.x ), y( p2.y - p1.y )
	{}


	// Construct from a CVector3, discarding z value
	explicit CVector2( const CVector3& v );
	// Require explicit conversion from CVector3 (see above)

	// Construct from a CVector4, discarding z & w values
	explicit CVector2( const CVector4& v );
	// Require explicit conversion from CVector4 (see above)


	// Copy constructor
    CVector2( const CVector2& v ) : x( v.x ), y( v.y )
	{}

	// Assignment operator
    CVector2& operator=( const CVector2& v )
	{
		if ( this != &v )
		{
			x = v.x;
			y = v.y;
		}
		return *this;
	}


	/*-----------------------------------------------------------------------------------------
		Setters
	-----------------------------------------------------------------------------------------*/

	// Set both vector components
    void Set
	(
		const TFloat32 xIn,
		const TFloat32 yIn
	)
	{
		x = xIn;
		y = yIn;
	}

	// Set the vector through a pointer to two floats
    void Set( const TFloat32* pfElts )
	{
		x = pfElts[0];
		y = pfElts[1];
	}

	// Set as vector between two points (p1 to p2)
    void Set
	(
		const CVector2& p1,
		const CVector2& p2
	)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
	}

	// Set the vector to (0,0)
    void SetZero()
	{
		x = y = 0.0f;
	}


	/*-----------------------------------------------------------------------------------------
		Array access
	-----------------------------------------------------------------------------------------*/

	// Access the x & y components in array style (i.e. v[0], v[1] same as v.x, v.y)
    TFloat32& operator[]( const TUInt32 index )
	{
		return (&x)[index];
	}

	// Access the x & y elements in array style - const result
	const TFloat32& operator[]( const TUInt32 index ) const
	{
		return (&x)[index];
	}


	/*-----------------------------------------------------------------------------------------
		Comparisons
	-----------------------------------------------------------------------------------------*/
	// Equality operators defined as non-member operations after the class definition

	// Test if the vector is zero length (i.e. = (0,0))
	// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
	bool IsZero() const
	{
		return gen::IsZero( x*x + y*y );
	}

	// Test if the vector is unit length (normalised)
	// Uses BaseMath.h float approximation function 'IsZero' with default epsilon (margin of error)
	bool IsUnit() const
	{
		return gen::IsZero( x*x + y*y - 1.0f );
	}


	/*-----------------------------------------------------------------------------------------
		Member Operators
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	///////////////////////////////
	// Addition / subtraction

	// Add another vector to this vector
    CVector2& operator+=( const CVector2& v )
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	// Subtract another vector from this vector
    CVector2& operator-=( const CVector2& v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}


	////////////////////////////////////
	// Scalar multiplication & division

	// Multiply this vector by a scalar
	CVector2& operator*=( const TFloat32 s )
	{
		x *= s;
		y *= s;
		return *this;
	}

	// Divide this vector by a scalar
    CVector2& operator/=( const TFloat32 s )
	{
		GEN_GUARD_OPT;
		GEN_ASSERT_OPT( !gen::IsZero(s), "Invalid parameter" );

		x /= s;
		y /= s;
		return *this;

		GEN_ENDGUARD_OPT;
	}


	////////////////////////////////////
	// Other operations

	// Set this vector to its perpendicular, in a counter-clockwise direction
	void SetPerpendicular()
	{
		TFloat32 t = x;
		x = -y;
		y = t;
	}

	// Return a vector perpendicular to this one, in a counter-clockwise direction
	CVector2 Perpendicular()
	{
		return CVector2(-y, x);
	}


	// Dot product of this with another vector
    TFloat32 Dot( const CVector2& v ) const
	{
	    return x*v.x + y*v.y;
	}
	
	
	// Cross product of this with another vector, both promoted to 3D with a z component of 0
	// Result is positive if the other vector is counter-clockwise from this vector
    CVector2 Cross3D( const CVector2& v ) const
	{
		return CVector2(y*v.x - x*v.y, x*v.y - y*v.x);
	}


	/*-----------------------------------------------------------------------------------------
		Length operations
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	// Return length of this vector
	TFloat32 Length() const
	{
		return Sqrt( x*x + y*y );
	}

	// Return squared length of this vector
	// More efficient than Length when exact value is not required (e.g. for comparisons)
	// Use InvSqrt( LengthSquared(...) ) to calculate 1 / length more efficiently
	TFloat32 LengthSquared() const
	{
		return x*x + y*y;
	}

	// Reduce vector to unit length
    void Normalise();


	/*-----------------------------------------------------------------------------------------
		Point related functions
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	// Return distance from this point to another
    TFloat32 DistanceTo( const CVector2& p );

	// Return squared distance from this point to another
	// More efficient than Distance when exact length is not required (e.g. for comparisons)
	// Use InvSqrt( DistanceToSquared(...) ) to calculate 1 / distance more efficiently
	TFloat32 DistanceToSquared( const CVector2& p );


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/
    
    // Vector components
    TFloat32 x;
	TFloat32 y;

	// Standard vectors
	static const CVector2 kZero;
	static const CVector2 kOne;
	static const CVector2 kOrigin;
	static const CVector2 kXAxis;
	static const CVector2 kYAxis;
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
	const CVector2& v1,
	const CVector2& v2
)
{
	return AreEqual( v1.x, v2.x ) && AreEqual( v1.y, v2.y );
}

// Vector inequality
// Uses BaseMath.h float approximation function 'AreEqual' with default margin of error
inline bool operator!=
(
	const CVector2& v1,
	const CVector2& v2
)
{
	return !AreEqual( v1.x, v2.x ) || !AreEqual( v1.y, v2.y );
}


///////////////////////////////
// Addition / subtraction

// Vector addition
inline CVector2 operator+
(
	const CVector2& v1,
	const CVector2& v2
)
{
	return CVector2(v1.x + v2.x, v1.y + v2.y);
}

// Vector subtraction
inline CVector2 operator-
(
	const CVector2& v1,
	const CVector2& v2
)
{
	return CVector2(v1.x - v2.x, v1.y - v2.y);
}

// Unary positive (i.e. a = +v, included for completeness)
inline CVector2 operator+( const CVector2& v )
{
	return v;
}

// Unary negation (i.e. a = -v)
inline CVector2 operator-( const CVector2& v )
{
	return CVector2(-v.x, -v.y);
}


////////////////////////////////////
// Scalar multiplication & division

// Vector multiplied by scalar
inline CVector2 operator*
(
	const CVector2& v,
	const TFloat32  s
)
{
	return CVector2(v.x*s, v.y*s);
}

// Scalar multiplied by vector
inline CVector2 operator*
(
	const TFloat32  s,
	const CVector2& v
)
{
	return CVector2(v.x*s, v.y*s);
}

// Vector divided by scalar
inline CVector2 operator/
(
	const CVector2& v,
	const TFloat32  s
)
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( !IsZero(s), "Invalid parameter" );

	return CVector2(v.x/s, v.y/s);

	GEN_ENDGUARD_OPT;
}


////////////////////////////////////
// Other operations

// Return a vector perpendicular to the given one, in a counter-clockwise direction
inline CVector2 Perpendicular( const CVector2& v )
{
	return CVector2(-v.y, v.x);
}


// Dot product of two given vectors (order not important) - non-member version
inline TFloat32 Dot
(
	const CVector2& v1,
	const CVector2& v2
)
{
    return v1.x*v2.x + v1.y*v2.y;
}

// Cross product of two given vectors (order is important), both promoted to 3D with a
// z component of 0 - non-member version
// Result is positive if the second vector is counter-clockwise from the first
inline CVector2 Cross3D
(
	const CVector2& v1,
	const CVector2& v2
)
{
	return CVector2(v1.y*v2.x - v1.x*v2.y, v1.x*v2.y - v1.y*v2.x);
}

/*-----------------------------------------------------------------------------------------
	Non-Member Length operations
-----------------------------------------------------------------------------------------*/

// Return length of given vector
inline TFloat32 Length( const CVector2& v )
{
	return Sqrt( v.x*v.x + v.y*v.y );
}

// Return squared length of given vector
// More efficient than Length when exact value is not required (e.g. for comparisons)
// Use InvSqrt( LengthSquared(...) ) to calculate 1 / length more efficiently
inline TFloat32 LengthSquared( const CVector2& v )
{
	return v.x*v.x + v.y*v.y;
}

// Return unit length vector in the same direction as given one
CVector2 Normalise( const CVector2& v );


/*-----------------------------------------------------------------------------------------
	Non-member point related functions
-----------------------------------------------------------------------------------------*/

// Return distance from one point to another - non-member version
TFloat32 Distance
(
	const CVector2& p1,
	const CVector2& p2
);

// Return squared distance from one point to another - non-member version
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceSquared(...) ) to calculate 1 / distance more efficiently
TFloat32 DistanceSquared
(
	const CVector2& p1,
	const CVector2& p2
);


} // namespace gen

#endif // GEN_C_VECTOR_2_H_INCLUDED
