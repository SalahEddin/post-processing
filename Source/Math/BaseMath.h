/**************************************************************************************************
	Module:       BaseMath.h
	Author:       Laurent Noel
	Date created: 12/06/06

	Basic mathematical functions, used by other math types and available for general use. Prefer
	these definitions over the use of <math.h> as they may be optimised per-platform

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#ifndef GEN_C_BASE_MATH_H_INCLUDED
#define GEN_C_BASE_MATH_H_INCLUDED

#include <stdlib.h>
#include <math.h>

#include "Defines.h"
#include "Error.h"

//TODO
// Vectors: Hermite / Catmull-Rom, Lerp, Barycentric
// Matrices: ReflectioninPlane, shadow, transform plane
// All: Packing, alignment, improve efficiency (SSE etc)

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Mathematical constants
-----------------------------------------------------------------------------------------*/

const TFloat32 kfPi = 3.1415926535897932384626433832795f;
const TFloat64 kfPi64 = 3.1415926535897932384626433832795;

// Default epsilon values (margin of error for approximations), suitable for values known
// to be around 1.0. Provided for convenience, read the extensive commentary below regarding
// floating point approximation before considering if these values are appropriate
const TFloat32 kfEpsilon = 0.5e-6f;    // For 32-bit floats
const TFloat64 kfEpsilon64 = 0.5e-15f; // For 64-bit floats


/*-----------------------------------------------------------------------------------------
	Support types
-----------------------------------------------------------------------------------------*/

// Order of rotations for Euler angles in 3D - for CMatrix3x3 & CMatrix4x4. This is the intuitive
// order of transformations, so kYXZ means rotate first in Y then X then Z
enum ERotationOrder
{
	kXYZ = 0,
	kXZY,
	kYZX,
	kYXZ,
	kZXY,
	kZYX,
};

// For rounding functions
enum ERoundingMethod
{
	kRoundNearest = 0,
	kRoundUp,        // Round up always in +ve direction
	kRoundDown,      // Round down always in -ve direction
	kRoundTowards0,  // Round values towards 0
	kRoundAwayFrom0, // Round values away from 0
};


/*-----------------------------------------------------------------------------------------
	Platform-specific basic operations
-----------------------------------------------------------------------------------------*/
// Many versions provided here to allow mixing of parameter types for these basic functions

inline TUInt32 Abs( const TInt32 x ) { return abs( static_cast<int>(x) ); }
inline TUInt64 Abs( const TInt64 x ) { return _abs64( x ); }
inline TFloat32 Abs( const TFloat32 x ) { return fabsf( x ); }
inline TFloat64 Abs( const TFloat64 x ) { return fabs( x ); }

inline TInt32 Mod( const TInt32 x, const TInt32 y ) { return x % y; }
inline TInt64 Mod( const TInt64 x, const TInt64 y ) { return x % y; }
inline TInt64 Mod( const TInt32 x, const TInt64 y ) {  return Mod( static_cast<TInt64>(x), y ); }
inline TInt64 Mod( const TInt64 x, const TInt32 y ) {  return Mod( x, static_cast<TInt64>(y) ); }
inline TFloat32 Mod( const TFloat32 x, const TFloat32 y ) { return fmodf( x, y ); }
inline TFloat64 Mod( const TFloat64 x, const TFloat64 y ) { return fmod( x, y ); }
inline TFloat64 Mod( const TFloat32 x, const TFloat64 y ) {  return Mod( static_cast<TFloat64>(x), y ); }
inline TFloat64 Mod( const TFloat64 x, const TFloat32 y ) {  return Mod( x, static_cast<TFloat64>(y) ); }
inline TFloat32 Mod( const TInt32 x, const TFloat32 y ) {  return Mod( static_cast<TFloat32>(x), y ); }
inline TFloat32 Mod( const TFloat32 x, const TInt32 y ) {  return Mod( x, static_cast<TFloat32>(y) ); }
inline TFloat64 Mod( const TInt32 x, const TFloat64 y ) {  return Mod( static_cast<TFloat64>(x), y ); }
inline TFloat64 Mod( const TFloat64 x, const TInt32 y ) {  return Mod( x, static_cast<TFloat64>(y) ); }
inline TFloat64 Mod( const TInt64 x, const TFloat32 y ) {  return Mod( static_cast<TFloat64>(x),
                                                                       static_cast<TFloat64>(y) ); }
inline TFloat64 Mod( const TFloat32 x, const TInt64 y ) {  return Mod( static_cast<TFloat64>(x),
                                                                       static_cast<TFloat64>(y) ); }
inline TFloat64 Mod( const TInt64 x, const TFloat64 y ) {  return Mod( static_cast<TFloat64>(x), y ); }
inline TFloat64 Mod( const TFloat64 x, const TInt64 y ) {  return Mod( x, static_cast<TFloat64>(y) ); }

inline TFloat32 Floor( const TFloat32 x ) { return floorf( x ); }
inline TFloat64 Floor( const TFloat64 x ) { return floor( x ); }
inline TFloat32 Ceil( const TFloat32 x ) { return ceilf( x ); }
inline TFloat64 Ceil( const TFloat64 x ) { return ceil( x ); }

inline TFloat32 Sqrt( const TFloat32 x ) { return sqrtf( x ); }
inline TFloat64 Sqrt( const TFloat64 x ) { return sqrt( x ); }
inline TFloat32 Sqrt( const TInt32 x ) { return Sqrt( static_cast<TFloat32>(x) ); }
inline TFloat64 Sqrt( const TInt64 x ) { return Sqrt( static_cast<TFloat64>(x) ); }

inline TFloat32 Pow( const TFloat32 x, const TFloat32 y ) { return powf( x, y ); }
inline TFloat64 Pow( const TFloat64 x, const TFloat64 y ) { return pow( x, y ); }
inline TFloat64 Pow( const TFloat32 x, const TFloat64 y ) { return Pow( static_cast<TFloat64>(x), y ); }
inline TFloat64 Pow( const TFloat64 x, const TFloat32 y ) { return Pow( x, static_cast<TFloat64>(y) ); }
inline TFloat32 Pow( const TInt32 x, const TFloat32 y ) { return Pow( static_cast<TFloat32>(x), y ); }
inline TFloat32 Pow( const TFloat32 x, const TInt32 y ) { return Pow( x, static_cast<TFloat32>(y) ); }
inline TFloat64 Pow( const TInt32 x, const TFloat64 y ) { return Pow( static_cast<TFloat64>(x), y ); }
inline TFloat64 Pow( const TFloat64 x, const TInt32 y ) { return Pow( x, static_cast<TFloat64>(y) ); }
inline TFloat32 Pow( const TInt32 x, const TInt32 y ) { return Pow( static_cast<TFloat32>(x),
                                                                    static_cast<TFloat32>(y) ); }
inline TFloat64 Pow( const TInt64 x, const TFloat32 y ) { return Pow( static_cast<TFloat64>(x),
                                                                      static_cast<TFloat64>(y) ); }
inline TFloat64 Pow( const TFloat32 x, const TInt64 y ) { return Pow( static_cast<TFloat64>(x),
                                                                      static_cast<TFloat64>(y) ); }
inline TFloat64 Pow( const TInt64 x, const TFloat64 y ) { return Pow( static_cast<TFloat64>(x), y ); }
inline TFloat64 Pow( const TFloat64 x, const TInt64 y ) { return Pow( x, static_cast<TFloat64>(y) ); }
inline TFloat64 Pow( const TInt64 x, const TInt64 y ) { return Pow( static_cast<TFloat64>(x),
                                                                    static_cast<TFloat64>(y) ); }
inline TFloat64 Pow( const TInt32 x, const TInt64 y ) { return Pow( static_cast<TFloat64>(x), y ); }
inline TFloat64 Pow( const TInt64 x, const TInt32 y ) { return Pow( x, static_cast<TFloat64>(y) ); }

inline TFloat32 Sin( const TFloat32 x ) { return sinf( x ); }
inline TFloat64 Sin( const TFloat64 x ) { return sin( x ); }
inline TFloat32 Cos( const TFloat32 x ) { return cosf( x ); }
inline TFloat64 Cos( const TFloat64 x ) { return cos( x ); }
inline TFloat32 Tan( const TFloat32 x ) { return tanf( x ); }
inline TFloat64 Tan( const TFloat64 x ) { return tan( x ); }

inline TFloat32 ASin( const TFloat32 x ) { return asinf( x ); }
inline TFloat64 ASin( const TFloat64 x ) { return asin( x ); }
inline TFloat32 ACos( const TFloat32 x ) { return acosf( x ); }
inline TFloat64 ACos( const TFloat64 x ) { return acos( x ); }
inline TFloat32 ATan( const TFloat32 x ) { return atanf( x ); }
inline TFloat64 ATan( const TFloat64 x ) { return atan( x ); }
inline TFloat32 ATan( const TFloat32 x, const TFloat32 y ) { return atan2f( x, y ); }
inline TFloat64 ATan( const TFloat64 x, const TFloat64 y ) { return atan2( x, y ); }
inline TFloat64 ATan( const TFloat32 x, const TFloat64 y ) { return ATan( static_cast<TFloat64>(x), y ); }
inline TFloat64 ATan( const TFloat64 x, const TFloat32 y ) { return ATan( x, static_cast<TFloat64>(y) ); }


/*-----------------------------------------------------------------------------------------
	Common variations of basic operations
-----------------------------------------------------------------------------------------*/

// 1 / Sqrt
inline TFloat32 InvSqrt( const TFloat32 x )
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( x != 0.0f, "Invalid parameter" );

	return 1.0f / Sqrt( x );

	GEN_ENDGUARD_OPT;
}

// 1 / Sqrt
inline TFloat64 InvSqrt( const TFloat64 x )
{
	GEN_GUARD_OPT;
	GEN_ASSERT_OPT( x != 0.0f, "Invalid parameter" );

	return 1.0f / Sqrt( x );

	GEN_ENDGUARD_OPT;
}

// 1 / Sqrt
inline TFloat32 InvSqrt( const TInt32 x ) { return InvSqrt(static_cast<TFloat32>(x)); }

// 1 / Sqrt
inline TFloat64 InvSqrt( const TInt64 x ) { return InvSqrt(static_cast<TFloat64>(x)); }


// Get both sin and cos of x, more efficient than calling functions seperately
inline void SinCos
(
	TFloat32  x,
	TFloat32* pSin,
	TFloat32* pCos
)
{
    *pSin = Sin( x );
    *pCos = Cos( x );
}

// Get both sin and cos of x, more efficient than calling functions seperately
inline void SinCos
(
	TFloat64  x,
	TFloat64* pSin,
	TFloat64* pCos
)
{
    *pSin = Sin( x );
    *pCos = Cos( x );
}


/*-----------------------------------------------------------------------------------------
	Angle conversion functions
-----------------------------------------------------------------------------------------*/

// Convert radians to degrees
inline TFloat32 ToDegrees( const TFloat32 r )
{
	return (r * 180.0f) / kfPi;
}

// Convert radians to degrees
inline TFloat64 ToDegrees( const TFloat64 r )
{
	return (r * 180.0) / kfPi64;
}

// Convert radians to degrees
inline TFloat32 ToDegrees( const TInt32 r ) { return ToDegrees(static_cast<TFloat32>(r)); }

// Convert radians to degrees
inline TFloat64 ToDegrees( const TInt64 r ) { return ToDegrees(static_cast<TFloat64>(r)); }


// Convert degrees to radians
inline TFloat32 ToRadians( const TFloat32 d )
{
	return (d * kfPi) / 180.0f;
}

// Convert degrees to radians
inline TFloat64 ToRadians( const TFloat64 d )
{
	return (d * kfPi64) / 180.0;
}

// Convert degrees to radians
inline TFloat32 ToRadians( const TInt32 d ) { return ToRadians(static_cast<TFloat32>(d)); }

// Convert degrees to radians
inline TFloat64 ToRadians( const TInt64 d ) { return ToRadians(static_cast<TFloat64>(d)); }


/*-----------------------------------------------------------------------------------------
	Float comparisons
-----------------------------------------------------------------------------------------*/

// A floating point number can only hold a finite number of different values. Other values will be
// rounded to the nearest possible value, e.g. 0.5 can be represented exactly, but 0.1 will be
// rounded up slightly (to 0.10000000149011612 for a 32-bit float). Similar rounding occurs after
// every floating point calculation. So floating point values are usually in error from the actual
// value required. So we need to:
//   o Take care with comparisons between floating point values
//     - Avoid using == for floats in general (though see below)
//     - Use < or > as an alternative for the majority of cases
//     - Robust functions to perform approximate comparisons are provided below
//     - Note that some values can be represented exactly. A 32-bit (IEEE) float can accurately
//       hold all the *integers* from -16777216.0f to +16777216.0f. Similarly, a 64-bit double can
//       accurately hold all *integers* from -9007199254740992.0 to 9007199254740992.0. In these
//       cases using == is safe, but only if *both* values can be guaranteed exact.
//   o Avoid magnifying rounding errors during calculations
//     - No relevant functions given here, but rules of thumb are:
//       = Avoid very large or small intermediate values
//       = Reorder calculations to keep intermediate values close to source/result values
//       = Reduce use of powers where possible - they magnify errors, e.g. rearrange / factorise
//       = E.g. If we use (x^2 - y^2) when x & y are close, we could lose considerable accuracy,
//              the ^2 can magnify the rounding error so x^2 & y^2 are much closer than in reality.
//              Instead use (x-y)(x+y), which avoids use of powers and stays close to in/out values
//       = However, repeated calculations will always tend to increase existing errors. Some form
//         of normalisation / recalculation may be needed for values with long persistence
//
// For the (advanced) mathematical background to these issues see:
//     http://citeseer.ist.psu.edu/goldberg91what.html


// Test if a TFloat32 value is approximately 0. Epsilon value is the range around zero that
// is considered equal to zero. Default value requires zero to 6 decimal places
inline bool IsZero
(
	const TFloat32 x,
	const TFloat32 fEpsilon = kfEpsilon
)
{
	return Abs( x ) < fEpsilon;
}

// Test if a TFloat64 value is approximately 0. Epsilon value is the range around zero that
// is considered equal to zero. Default value requires zero to 15 decimal places
inline bool IsZero
(
	const TFloat64 x,
	const TFloat64 fEpsilon = kfEpsilon64
)
{
	return Abs( x ) < fEpsilon;
}


// There are two methods to test if two floats are (approximately) equal:

// Method 1 - test their *absolute* difference: see if |a-b| < epsilon (a small value)
// This method is fast, but the appropriate epsilon value varies, e.g. 1,000,000 is nearly equal
// to 1,000,001, but 1 is not nearly equal to 2, yet the absolute difference is 1 in both cases.
// So an epsilon of 1 is OK for 1,000,000, but 0.000001 would be better for 1 - you need to know
// the rough size of the values you are comparing

// Test if two TFloat32 values are approximately equal using their absolute difference. Pass an
// epsilon value - if the two values are within this range, they are considered equal
// Default value gives equality to 6 decimal places
inline bool AreEqualAbsolute
(
	const TFloat32 x,
	const TFloat32 y,
	const TFloat32 fEpsilon = kfEpsilon
)
{
	return Abs( x - y ) < fEpsilon;
}

// Test if two TFloat64 values are approximately equal using their absolute difference. Pass an
// epsilon value - if the two values are within this range, they are considered equal
// Default value gives equality to 15 decimal places
inline bool AreEqualAbsolute
(
	const TFloat64 x,
	const TFloat64 y,
	const TFloat64 fEpsilon = kfEpsilon64
)
{
	return Abs( x - y ) < fEpsilon;
}


// Floating point comparison method 2:
// Test the *relative* difference of two values: see if |a-b| < m*epsilon where m = max(|a|,|b|)
// This scales the epsilon value by the value with the largest magnitude, giving a result that
// is independent of the value size, but involves more calculation.
// The equation can be rearranged to minimise tests and reduce problems with float rounding:
//      if a & b are +ve then, if a > b  check a*(1-e) < b,  otherwise check  b*(1-e) < a
//   or if a & b are -ve then, if a < b  check a*(1-e) > b,  otherwise check  b*(1-e) > a
//   or if a & b are opposite signs then always fail
// Note that the relative difference may not be appropriate for values near to zero. For example,
// 0.000001 != 0.000002 - since the second value is twice as large as the first. Also a +ve value
// will never be considered approximately equal to a -ve value, no matter how small. In these
// cases, consider if absolute comparison (AreEqualAbsolute or IsZero) is more appropriate as an
// alternative or as an extra test

// Test if two TFloat32 values are approximately equal using relative difference. Epsilon is the
// maximum difference in the significand for the values to be considered equal. The default 
// requires 6-7 significant figures to be the same for equality. 
bool AreEqualRelative
(
	const TFloat32 x,
	const TFloat32 y,
	const TFloat32 fEpsilon = kfEpsilon
);

// Test if two TFloat64 values are approximately equal using relative difference. Epsilon is the
// maximum difference in the significand for the values to be considered equal. The default 
// requires 15-16 significant figures to be the same for equality. 
bool AreEqualRelative
(
	const TFloat64 x,
	const TFloat64 y,
	const TFloat64 fEpsilon = kfEpsilon64
);


// An efficient alternative method for relative difference comparison relies on the properties of
// floating point values stored in the standard IEC 559 (=IEEE 754) format. We compare the actual
// integer representation of the float values, see:
//     http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm and
// C++ does not require floats to be stored in IEC/IEEE format, it is compiler dependent. For
// details of the IEC 559 format, see:
//     http://en.wikipedia.org/wiki/IEC_559
// This method avoids multiplications, but the reinterpretation of floats to integers might perform
// worse than the above method (e.g. if float values are already in registers TODO: test this)
// This method has the same issues for comparisons near zero as highlighted for AreEqualRelative

// Ensure that float types conform to IEC standard (IEC 559 == IEEE 754) before compiling
#if defined(IEC_559_FLOATS)

// Test if two TFloat32 values are approximately equal using relative difference. iEpsilonFrac is
// the maximum difference in the fractional part of the IEEE float representation of the two
// values (sometimes called ulps - units in the last place). The default value gives high accuracy,
// only giving equality if the values are equal to *at least* 6 significant figures, and always for
// 8 or more equal significant figures. Multiply (~4.5) by 10 for each less digit of accuracy
// required. E.g. for 2 significant figures (4 less than max of 6), use ~4.5 * 10^4 = ~45000
inline bool AreEqual
(
	const TFloat32 x,
	const TFloat32 y,
	const TUInt32  iEpsilonFrac = 4
)
{
	// Reinterpret 32-bit float as 32-bit unsigned int
    TInt32 xInt = *(TInt32*)&x;
    if (xInt < 0)
	{
		// Reorder negative values so we can use integer comparison
        xInt = 0x80000000 - xInt;
	}

	// Same with second value
    TInt32 yInt = *(TInt32*)&y;
    if (yInt < 0)
	{
        yInt = 0x80000000 - yInt;
	}

	// Values are equal if integer representations are near enough
    return Abs( xInt - yInt ) < iEpsilonFrac;
}

// Test if two TFloat64 values are approximately equal using relative difference. iEpsilonFrac is
// the maximum difference in the fractional part of the IEEE float representation of the two
// values (sometimes called ulps - units in the last place). The default value gives high accuracy,
// only giving equality if the values are equal to *at least* 15 significant figures, and always
// for 17 or more equal significant figures. Multiply (~2.4) by 10 for each less digit of accuracy
// required. E.g. for 10 significant figures (5 less than max of 15), use ~2.4 * 10^5 = ~240000
inline bool AreEqual
(
	const TFloat64 x,
	const TFloat64 y,
	const TUInt32  iEpsilonFrac = 2
)
{
	// Reinterpret 64-bit float as 64-bit unsigned int
    TInt64 xInt = *(TInt64*)&x;
    if (xInt < 0)
	{
		// Reorder negative values so we can use integer comparison
        xInt = 0x8000000000000000 - xInt;
	}

	// Same with second value
    TInt64 yInt = *(TInt64*)&y;
    if (yInt < 0)
	{
        yInt = 0x8000000000000000 - yInt;
	}

	// Values are equal if integer representations are near enough
    return Abs( xInt - yInt ) < iEpsilonFrac;
}

#endif // IEC_559_FLOATS


/*-----------------------------------------------------------------------------------------
	Miscellaneous numeric functions
-----------------------------------------------------------------------------------------*/

// Min template function - find minimum of two values of any type that has < operator defined
// If the values are equivalent, then the first is considered the minimum
template <class C>
inline C Min( const C a, const C b ) { return ((b < a) ? b : a); }

// Max template function - find maximum of two values of any type that has < operator defined
// If the values are equivalent, then the second is considered the maximum
template <class C>
inline C Max( const C a, const C b ) { return (!(b < a) ? b : a); }


// Return random integer from a to b (inclusive)
// Can only return up to RAND_MAX different values, spread evenly across the given range
// RAND_MAX is defined in stdlib.h and is compiler-specific (32767 on VS-2005, higher elsewhere)
inline TInt32 Random( const TInt32 a, const TInt32 b )
{
	// Could just use a + rand() % (b-a), but using a more complex form to allow range
	// to exceed RAND_MAX and still return values spread across the range
	TInt32 t = (b - a + 1) * rand();
	return t == 0 ? a : a + (t - 1) / RAND_MAX;
}

// Return random 32-bit float from a to b (inclusive)
// Can only return up to RAND_MAX different values, spread evenly across the given range
// RAND_MAX is defined in stdlib.h and is compiler-specific (32767 on VS-2005, higher elsewhere)
inline TFloat32 Random( const TFloat32 a, const TFloat32 b )
{
	return a + (b - a) * (static_cast<TFloat32>(rand()) / RAND_MAX);
}

// Return random 64-bit float from a to b (inclusive)
// Can only return up to RAND_MAX different values, spread evenly across the given range
// RAND_MAX is defined in stdlib.h and is compiler-specific (32767 on VS-2005, higher elsewhere)
inline TFloat64 Random( const TFloat64 a, const TFloat64 b )
{
	return a + (b - a) * (static_cast<TFloat64>(rand()) / RAND_MAX);
}


// Round integer value to a multiple of another value. Supply the rounding method to use
TUInt32 Round
(
	const TInt32          iValue,
	const ERoundingMethod eRounding,
	const TInt32          iMultipleOf
);

// Round 32-bit floating point value to a multiple of another value, default multiple is 1.0.
// Supply rounding method to use, default is kRoundNearest. Round(x) rounds to nearest integer
TFloat32 Round
(
	const TFloat32        fValue,
	const ERoundingMethod eRounding = kRoundNearest,
	const TFloat32        fMultipleOf = 1.0f
);

// Round 64-bit floating point value to a multiple of another value, default multiple is 1.0.
// Supply rounding method to use, default is kRoundNearest. Round(x) rounds to nearest integer
TFloat64 Round
(
	const TFloat64        fValue,
	const ERoundingMethod eRounding = kRoundNearest,
	const TFloat64        fMultipleOf = 1.0
);


} // namespace gen

#endif // GEN_C_BASE_MATH_H_INCLUDED
