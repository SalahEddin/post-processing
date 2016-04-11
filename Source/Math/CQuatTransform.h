/*******************************************
	CQuatTransform.h

	A class binding forming a transformation
	from a CQuaternion (rotation), and two
	CVector3 (position and scale)
********************************************/

#ifndef GEN_C_QUAT_TRANSFORM_H_INCLUDED
#define GEN_C_QUAT_TRANSFORM_H_INCLUDED

#include "Defines.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "CQuaternion.h"

namespace gen
{


// Quaternion-based transformation class
class CQuatTransform
{
// Concrete class - public access
public:
	/*-----------------------------------------------------------------------------------------
		Constructors/Destructors
	-----------------------------------------------------------------------------------------*/

	// Default constructor - no initialisation
	CQuatTransform() {}

	// Constructor by value
    CQuatTransform
	(
		const CQuaternion& initQuat,
		const CVector3&    initPos,
		const CVector3&    initScale
	) : quat( initQuat ), pos( initPos ), scale( initScale ) {}

	// Construct from a 4x4 matrix
	CQuatTransform
	(
		const CMatrix4x4& initMat
	) 
	{
		// Get quaternion from upper 3x3 of matrix
		quat = CQuaternion( initMat );
		quat.Normalise();

		// Extract position from matrix (4th row)
		pos = initMat.GetPosition();

		// Scaling is the length of the axes store in the first 3 matrix rows
		scale = initMat.GetScale();
	}


	// Copy constructor
    CQuatTransform
	(
		const CQuatTransform& src
	) : quat( src.quat ), pos( src.pos ), scale( src.scale ) {}

	// Assignment operator
    CQuatTransform& operator=
	(
		const CQuatTransform& src
	)
	{
		if ( this != &src )
		{
			quat = src.quat;
			pos = src.pos;
			scale = src.scale;
		}
		return *this;
	}

	// Destructor
	~CQuatTransform() {}


/*-----------------------------------------------------------------------------------------
	Public functions
-----------------------------------------------------------------------------------------*/
public:

	/*-----------------------------------------------------------------------------------------
		Matrix extraction
	-----------------------------------------------------------------------------------------*/

	// Get the 4x4 matrix equivalent to this quaternion-transform
    void GetMatrix
	(
		CMatrix4x4& mat
	) const
	{
		// Create initial matrix from quaternion
		mat = CMatrix4x4( quat );

		// Add scale and position to matrix
		mat.SetScale( scale );
		mat.SetPosition( pos );
	}


	/*-----------------------------------------------------------------------------------------
		Member Operators
	-----------------------------------------------------------------------------------------*/
	// Non-member versions defined after the class definition

	///////////////////////////////
	// Addition / subtraction

	// Add another quaternion transform to this one
    CQuatTransform& operator+=
	(
		const CQuatTransform& qt
	)
	{
		quat += qt.quat;
		pos += qt.pos;
		scale += qt.scale;
		return *this;
	}

	// Subtract another quaternion transform to this one
    CQuatTransform& operator-=
	(
		const CQuatTransform& qt
	)
	{
		quat -= qt.quat;
		pos -= qt.pos;
		scale -= qt.scale;
		return *this;
	}


	///////////////////////////////
	// Scalar operations

	// Scalar multiplication
    CQuatTransform& operator*=
	(
		const TFloat32& scalar
	)
	{
		quat *= scalar;
		pos *= scalar;
		scale *= scalar;
		return *this;
	}

	// Scalar division
    CQuatTransform& operator/=
	(
		const TFloat32& scalar
	)
	{
		quat /= scalar;
		pos /= scalar;
		scale /= scalar;
		return *this;
	}


	/*-----------------------------------------------------------------------------------------
		Transformation operations
	-----------------------------------------------------------------------------------------*/

	// Return the given CVector3 transformed by this quaternion-transform
	// Assuming it is a vector rather then a point
    CVector3 TransformVector
	(
		const CVector3& vec
	) const
	{
		// First scale the vector
		CVector3 scaleVec( scale.x * vec.x, scale.y * vec.y, scale.z * vec.z );

		// Then rotate the vector with the quaternion. Ignore position for vector transform
		return quat.Rotate( scaleVec );
	}

	// Return the given CVector3 transformed by this quaternion-transform
	// Assuming it is a point rather than a vector
    CVector3 TransformPoint
	(
		const CVector3& vec
	) const
	{
		// First scale the point
		CVector3 scaleVec( scale.x * vec.x, scale.y * vec.y, scale.z * vec.z );

		// Then rotate the vector with the quaternion and add the position
		return quat.Rotate( scaleVec ) + pos;
	}


	// Combine this transform by the given one
    CQuatTransform& operator*=
	(
		const CQuatTransform& q
	);
	
	// Combine two transforms together - non-member function
	friend CQuatTransform operator*
	(
		const CQuatTransform& q1,
		const CQuatTransform& q2
	);


	/*---------------------------------------------------------------------------------------------
		Interpolation
	---------------------------------------------------------------------------------------------*/

	// Linear interpolation of two quaternion-transforms q0 and q1, with parameter t, result in qt
	// Non-member function
	friend void Lerp
	(
		const CQuatTransform& q0,
		const CQuatTransform& q1,
		const TFloat32        t,
		CQuatTransform&       qt
	);

	// Linear interpolation of two quaternion-transforms q0 and q1, with parameter t, result in qt
	// Normalises quaternion after linear interpolation
	// Non-member function
	friend void NLerp
	(
		const CQuatTransform& q0,
		const CQuatTransform& q1,
		const TFloat32        t,
		CQuatTransform&       qt
	);

	// Spherical linear interpolation of two quaternion-transforms q0 and q1, with parameter t, 
	// result in qt. Only the quaternion uses slerp, position and scaling use lerp
	// Non-member function
	friend void Slerp
	(
		const CQuatTransform& q0,
		const CQuatTransform& q1,
		const TFloat32        t,
		CQuatTransform&       qt
	);


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// Rotation, position and scale using a CQuaternion and two CVector3
	CVector3    pos;
	CQuaternion quat;
	CVector3    scale;
};


/*-----------------------------------------------------------------------------------------
	Non-member Operators
-----------------------------------------------------------------------------------------*/

///////////////////////////////
// Addition / subtraction

// Addition
inline CQuatTransform operator+
(
	const CQuatTransform& qt1,
	const CQuatTransform& qt2
)
{
	return CQuatTransform( qt1.quat + qt2.quat, qt1.pos + qt2.pos, qt1.scale + qt2.scale );
}

// Subtraction
inline CQuatTransform operator-
(
	const CQuatTransform& qt1,
	const CQuatTransform& qt2
)
{
	return CQuatTransform( qt1.quat - qt2.quat, qt1.pos - qt2.pos, qt1.scale - qt2.scale );
}

// Unary positive (for completeness)
inline CQuatTransform operator+
(
	const CQuatTransform& qt
)
{
	return CQuatTransform( qt );
}

// Unary negation
inline CQuatTransform operator-
(
	const CQuatTransform& qt
)
{
	return CQuatTransform( -qt.quat, -qt.pos, -qt.scale );
}


///////////////////////////////
// Scalar operations

// Scalar multiplication
inline CQuatTransform operator*
(
	const CQuatTransform& qt1,
	const TFloat32        scalar
)
{
	return CQuatTransform( qt1.quat * scalar, qt1.pos * scalar, qt1.scale * scalar );
}

// Scalar division
inline CQuatTransform operator/
(
	const CQuatTransform& qt1,
	const TFloat32        scalar
)
{
	return CQuatTransform( qt1.quat / scalar, qt1.pos / scalar, qt1.scale / scalar );
}


} // namespace gen

#endif // GEN_C_QUATERNION_H_INCLUDED
