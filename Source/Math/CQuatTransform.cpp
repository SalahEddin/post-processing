/*******************************************
	CQuatTransform.cpp

	A class binding forming a transformation
	from a CQuaternion (rotation), and two
	CVector3 (position and scale)
********************************************/

#include "CQuatTransform.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Transformation operations
-----------------------------------------------------------------------------------------*/

// Combine this transform by the given one
CQuatTransform& CQuatTransform::operator*=
(
	const CQuatTransform& q
)
{
	// Just use binary operator*
	*this = *this * q;
	return *this;
}

// Combine two transforms together - non-member function
CQuatTransform operator*
(
	const CQuatTransform& q1,
	const CQuatTransform& q2
)
{
	CQuatTransform qr;  // Result transform

	// See Van Verth 3.4.3 for rationale behind this method (uses rotation matrix in book rather
	// than quaternion, but principle is the same)

	// Combine scales and quaternions simply
	qr.scale.Set( q1.scale.x * q2.scale.x, q1.scale.y * q2.scale.y, q1.scale.z * q2.scale.z );
	qr.quat = q1.quat * q2.quat; // Reversed from maths text for our left-handed system

	// Formula to combine position is: quat2 * (scale2 * pos1) + pos2
	// This is because the position from the first transform will be scaled and rotated by the
	// second transform before the second transform's position is added
	
	// First calculate scale2 * pos1...
	CVector3 scalePos( q2.scale.x * q1.pos.x, q2.scale.y * q1.pos.y, q2.scale.z * q1.pos.z );
	qr.pos = q2.quat.Rotate( scalePos ) + q2.pos;  // ...then complete the formula

	return qr;
}


/*---------------------------------------------------------------------------------------------
	Interpolation
---------------------------------------------------------------------------------------------*/

// Linear interpolation of two quaternion-transforms q0 and q1, with parameter t, result in qt
// Non-member function
void Lerp
(
	const CQuatTransform& q0,
	const CQuatTransform& q1,
	const TFloat32        t,
	CQuatTransform&       qt
)
{
	// Calculate lerp for position and scale
	qt.pos = q0.pos*(1.0f-t) + q1.pos*t;
	qt.scale = q0.scale*(1.0f-t) + q1.scale*t;

	// Call lerp function for quaternion rotation
	Lerp( q0.quat, q1.quat, t, qt.quat );
}


// Linear interpolation of two quaternion-transforms q0 and q1, with parameter t, result in qt
// Normalises quaternion after linear interpolation
// Non-member function
void NLerp
(
	const CQuatTransform& q0,
	const CQuatTransform& q1,
	const TFloat32        t,
	CQuatTransform&       qt
)
{
	// Calculate lerp for position and scale
	qt.pos = q0.pos*(1.0f-t) + q1.pos*t;
	qt.scale = q0.scale*(1.0f-t) + q1.scale*t;

	// Call lerp function for quaternion rotation
	NLerp( q0.quat, q1.quat, t, qt.quat );
}


// Spherical linear interpolation of two quaternion-transforms q0 and q1, with parameter t, 
// result in qt. Only the quaternion uses slerp, position and scaling use lerp
// Non-member function
void Slerp
(
	const CQuatTransform& q0,
	const CQuatTransform& q1,
	const TFloat32        t,
	CQuatTransform&       qt
)
{
	// Calculate lerp for position and scale
	qt.pos = q0.pos*(1.0f-t) + q1.pos*t;
	qt.scale = q0.scale*(1.0f-t) + q1.scale*t;

	// Call slerp function for quaternion rotation
	Slerp( q0.quat, q1.quat, t, qt.quat );
}


} // namespace gen

