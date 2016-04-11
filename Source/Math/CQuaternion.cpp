/**************************************************************************************************
	Module:       CQuaternion.cpp
	Author:       Laurent Noel
	Date created: 23/06/06

	Implementation of the concrete class CQuaternion, four 32-bit floats representing a quaternion

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 23/06/06 - LN
**************************************************************************************************/

#include "CQuaternion.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Construction / conversion
-----------------------------------------------------------------------------------------*/

// Construct a quaternion from a CMatrix4x4 - uses upper left 3x3 only
CQuaternion::CQuaternion
(
	const CMatrix4x4& m
)
{
	// Calculate matrix scaling
	TFloat32 scaleX = Sqrt( m.e00*m.e00 + m.e01*m.e01 + m.e02*m.e02 );
	TFloat32 scaleY = Sqrt( m.e10*m.e10 + m.e11*m.e11 + m.e12*m.e12 );
	TFloat32 scaleZ = Sqrt( m.e20*m.e20 + m.e21*m.e21 + m.e22*m.e22 );

	// Calculate inverse scaling to extract rotational values only
	GEN_ASSERT( !gen::IsZero(scaleX) && !gen::IsZero(scaleY) && !gen::IsZero(scaleZ),
				"Cannot extract rotation from singular matrix" );
	TFloat32 invScaleX = 1.0f / scaleX;
	TFloat32 invScaleY = 1.0f / scaleY;
	TFloat32 invScaleZ = 1.0f / scaleZ;

	// Calculate trace of matrix (the sum of diagonal elements)
	TFloat32 diagX = m.e00 * invScaleX; // Remove scaling
	TFloat32 diagY = m.e11 * invScaleY;
	TFloat32 diagZ = m.e22 * invScaleZ;
	TFloat32 trace = diagX + diagY + diagZ;

	// Simple method if trace is positive
	if (trace > 0.0f)
	{
		// Derive quaternion from remaining elements
		TFloat32 s = Sqrt( trace + 1.0f );
		w = s * 0.5f;
		TFloat32 invS = 0.5f / s;
		x = (m.e12*invScaleY - m.e21*invScaleZ) * invS;
		y = (m.e20*invScaleZ - m.e02*invScaleX) * invS;
		z = (m.e01*invScaleX - m.e10*invScaleY) * invS;
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
				x = 0.5f * maxAxis;
				invMaxAxis = 0.5f / maxAxis;
				y = (m.e01*invScaleX + m.e10*invScaleY) * invMaxAxis;
				z = (m.e20*invScaleZ + m.e02*invScaleX) * invMaxAxis;
				w = (m.e12*invScaleY - m.e21*invScaleZ) * invMaxAxis;
			}
			else
			{
				maxAxis = Sqrt( diagZ - diagX - diagY + 1.0f );
				z = 0.5f * maxAxis;
				invMaxAxis = 0.5f / maxAxis;
				x = (m.e20*invScaleZ + m.e02*invScaleX) * invMaxAxis;
				y = (m.e12*invScaleY + m.e21*invScaleZ) * invMaxAxis;
				w = (m.e01*invScaleX - m.e10*invScaleY) * invMaxAxis;
			}
		}
		else if (diagY > diagZ)
		{
			maxAxis = Sqrt( diagY - diagZ - diagX + 1.0f );
			y = 0.5f * maxAxis;
			invMaxAxis = 0.5f / maxAxis;
			z = (m.e12*invScaleY + m.e21*invScaleZ) * invMaxAxis;
			x = (m.e01*invScaleX + m.e10*invScaleY) * invMaxAxis;
			w = (m.e20*invScaleZ - m.e02*invScaleX) * invMaxAxis;
		}
		else
		{
			maxAxis = Sqrt( diagZ - diagX - diagY + 1.0f );
			z = 0.5f * maxAxis;
			invMaxAxis = 0.5f / maxAxis;
			x = (m.e20*invScaleZ + m.e02*invScaleX) * invMaxAxis;
			y = (m.e12*invScaleY + m.e21*invScaleZ) * invMaxAxis;
			w = (m.e01*invScaleX - m.e10*invScaleY) * invMaxAxis;
		}
	}
}


/*-----------------------------------------------------------------------------------------
	Quaternion multiplication
-----------------------------------------------------------------------------------------*/

// Return the quaternion result of multiplying two quaternions - non-member function
CQuaternion operator*
(
	const CQuaternion& q1,
	const CQuaternion& q2
)
{
	const CVector3& v1 = q1.Vector();
	const CVector3& v2 = q2.Vector();

	return CQuaternion( q1.w*q2.w - Dot( v1, v2 ), q1.w*v2 + q2.w*v1 + Cross( v2, v1 ) );
}


/*-----------------------------------------------------------------------------------------
	Length operations
-----------------------------------------------------------------------------------------*/

// Normalise the quaternion - make it unit length as a 4-vector
void CQuaternion::Normalise()
{
	TFloat32 fNormSquared = w*w + x*x + y*y + z*z;

	if ( gen::IsZero( fNormSquared ) )
	{
		w = x = y = z = 0.0f;
	}
	else
	{
		TFloat32 fInvLength = InvSqrt( fNormSquared );
		w *= fInvLength;
		x *= fInvLength;
		y *= fInvLength;
		z *= fInvLength;
	}
}


// Return a normalised version of a quaternion (unit length as a 4-vector) - non-member version
CQuaternion Normalise
(
	const CQuaternion& quat
)
{
	TFloat32 fNormSquared = quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;

	if ( gen::IsZero( fNormSquared ) )
	{
		return CQuaternion( 0.0f, 0.0f, 0.0f, 0.0f );
	}
	else
	{
		TFloat32 fInvLength = InvSqrt( fNormSquared );
		return CQuaternion( quat.w*fInvLength, quat.x*fInvLength,
		                    quat.y*fInvLength, quat.z*fInvLength );
	}
}


/*-----------------------------------------------------------------------------------------
	Vector transformation
-----------------------------------------------------------------------------------------*/

// Rotate a CVector3 by this quaternion
CVector3 CQuaternion::Rotate
(
	const CVector3& p
) const
{
	const CVector3& v = Vector();

	float tmp = 2.0f*w;
	return CVector3( (tmp*w-1.0f)*p + (2.0f*gen::Dot( v, p ))*v + tmp*Cross( v, p ) );
}


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
)
{
	// Simple linear interpolation equation
	qt = q0*(1.0f-t) + q1*t;
}


// Linear interpolation of two quaternions q0 and q1, with parameter t, result in qt
// Result is normalised to give an approximation to slerp
void NLerp
(
	const CQuaternion& q0,
	const CQuaternion& q1,
	const TFloat32     t,
	CQuaternion&       qt
)
{
	// Simple linear interpolation equation
	qt = q0*(1.0f-t) + q1*t;

	// Normalise resultant quaternion
	qt.Normalise();
}


// Spherical linear interpolation of two quaternions p and q, with parameter t, result in slerp
void Slerp
(
	const CQuaternion& p,
	const CQuaternion& q,
	const TFloat32     t,
	CQuaternion&       slerp
)
{
	// Slerp formula: qt = (sin((1-t)*theta)*p + sin(t*theta)*q) / sin theta , theta is angle
	// between quaternions. First get cos of angle between quaternions - can use dot product if
	// we assume our quaternions are normalised
	TFloat32 cosTheta = Dot( p, q );

	// Two routes round a circle from q0 to q1, choose the short one with this test
	if (cosTheta >= 0.0f)
	{
		// Slerp formula prone to error with small angles, ensure that is not the case
		if (!AreEqual( cosTheta, 1.0f ))
		{
			// Slerp calculation
			TFloat32 theta = ACos( cosTheta );
			
			// Now we have p, q, t and theta. Calculate slerp (from the equation in the notes)
			TFloat32 invSinTheta = 1.0f / Sin( theta );
			TFloat32 w1 = Sin( (1.0f-t)*theta ) * invSinTheta;
			TFloat32 w2 = Sin( t*theta ) * invSinTheta;
			slerp = p*w1 + q*w2;
		}
		else
		{
			// Small angle - lerp calculation is better
			slerp = p*(1.0f-t) + q*t;
		}
	}
	else
	{
		// Want opposite route round circle - negate first quaternion, otherwise same formula
		if (!AreEqual( cosTheta, -1.0f ))
		{
			TFloat32 theta = ACos( -cosTheta);
			TFloat32 invSinTheta = 1.0f / Sin( theta );
			TFloat32 w1 = Sin( (t-1.0f)*theta ) * invSinTheta;
			TFloat32 w2 = Sin( t*theta ) * invSinTheta;
			slerp = p*w1 + q*w2;
		}
		else
		{
			// Small angle - lerp calculation is better - again, negate first quaternion
			slerp = p*(t-1.0f) + q*t;
		}
	}
}


/*---------------------------------------------------------------------------------------------
	Static constants
---------------------------------------------------------------------------------------------*/

// Standard vectors
const CQuaternion CQuaternion::kZero( 0.0f, 0.0f, 0.0f, 0.0f );
const CQuaternion CQuaternion::kIdentity( 1.0f, 0.0f, 0.0f, 0.0f );


} // namespace gen
