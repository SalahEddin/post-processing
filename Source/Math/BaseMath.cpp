/**************************************************************************************************
	Module:       BaseMath.cpp
	Author:       Laurent Noel
	Date created: 12/06/06

	Basic mathematical functions, used by other math types and available for general use. Prefer
	these definitions over the use of <math.h> as they may be optimised per-platform

	Copyright 2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 12/06/06 - LN
**************************************************************************************************/

#include "Error.h"
#include "BaseMath.h"

namespace gen
{

/*-----------------------------------------------------------------------------------------
	Float comparisons
-----------------------------------------------------------------------------------------*/

// Test if two TFloat32 values are approximately equal using relative difference. Epsilon is
// the maximum difference in the significand for the values to be considered equal. The default 
// requires ~7 significant figures to be the same for equality. 
bool AreEqualRelative
(
	const TFloat32 x,
	const TFloat32 y,
	const TFloat32 fEpsilon /*= kfEpsilon*/
)
{
	if (x >= 0.0f && y >= 0.0f)
	{
		if (x > y)
		{
			return x * (1.0f - fEpsilon) < y;
		}
		else
		{
			return y * (1.0f - fEpsilon) < x;
		}
	}
	else if (x < 0.0f && y < 0.0f)
	{
		if (x < y)
		{
			return x * (1.0f - fEpsilon) > y;
		}
		else
		{
			return y * (1.0f - fEpsilon) > x;
		}
	}
	return false;
}

// Test if two TFloat64 values are approximately equal using relative difference. Epsilon is
// the maximum difference in the significand for the values to be considered equal. The default 
// requires ~16 significant figures to be the same for equality. 
bool AreEqualRelative
(
	const TFloat64 x,
	const TFloat64 y,
	const TFloat64 fEpsilon /*= kfEpsilon64*/
)
{
	if (x >= 0.0 && y >= 0.0)
	{
		if (x > y)
		{
			return x * (1.0 - fEpsilon) < y;
		}
		else
		{
			return y * (1.0 - fEpsilon) < x;
		}
	}
	else if (x < 0.0 && y < 0.0)
	{
		if (x < y)
		{
			return x * (1.0 - fEpsilon) > y;
		}
		else
		{
			return y * (1.0 - fEpsilon) > x;
		}
	}
	return false;
}


/*-----------------------------------------------------------------------------------------
	Miscellaneous numeric functions
-----------------------------------------------------------------------------------------*/

// Round integer value to a multiple of another value. Supply the rounding method to use
TUInt32 Round
(
	const TInt32          iValue,
	const ERoundingMethod eRounding,
	const TInt32          iMultipleOf
)
{
	GEN_GUARD;

	// Get positive multiple, ensure it is not 0 
	GEN_ASSERT( iMultipleOf != 0, "Invalid parameter" );
	TUInt32 iPosMultipleOf = Abs( iMultipleOf );

	// Select rounding method - often need different technique for +ve and -ve numbers
	switch (eRounding)
	{
		case kRoundTowards0:
		{
			return iPosMultipleOf * (iValue / iPosMultipleOf);
		}
		case kRoundDown:
		{
			if (iValue >= 0)
			{
				return iPosMultipleOf * (iValue / iPosMultipleOf);
			}
			else
			{
				return iPosMultipleOf * ((iValue - iPosMultipleOf + 1) / iPosMultipleOf);
			}
		}
		case kRoundAwayFrom0:
		{
			if (iValue >= 0)
			{
				return iPosMultipleOf * ((iValue + iPosMultipleOf - 1) / iPosMultipleOf);
			}
			else
			{
				return iPosMultipleOf * ((iValue - iPosMultipleOf + 1) / iPosMultipleOf);
			}
		}
		case kRoundUp:
		{
			if (iValue >= 0)
			{
				return iPosMultipleOf * ((iValue + iPosMultipleOf - 1) / iPosMultipleOf);
			}
			else
			{
				return iPosMultipleOf * (iValue / iPosMultipleOf);
			}
		}
		case kRoundNearest:
		{
			if (iValue >= 0)
			{
				return iPosMultipleOf * ((iValue + (iPosMultipleOf >> 1)) / iMultipleOf);
			}
			else
			{
				return iMultipleOf * ((iValue - (iMultipleOf >> 1)) / iMultipleOf);
			}
		}
		default:
			GEN_ERROR( "Invalid rounding method" );
	}

	GEN_ENDGUARD;
}


// Round 32-bit floating point value to a multiple of another value, default multiple is 1.0.
// Supply rounding method to use, default is kRoundNearest. Round(x) rounds to nearest integer
TFloat32 Round
(
	const TFloat32        fValue,
	const ERoundingMethod eRounding /*= kRoundNearest*/,
	const TFloat32        fMultipleOf /*= 1.0f*/
)
{
	GEN_GUARD;

	// Get positive multiple
	TFloat32 fPosMultipleOf = Abs( fMultipleOf );

	// More efficient methods if multiple is 1.0
	if (fPosMultipleOf == 1.0f)
	{
		// Select rounding method - often need different technique for +ve and -ve numbers
		switch (eRounding)
		{
			case kRoundNearest:
			{
				return Floor(fValue + 0.5f);
			}
			case kRoundTowards0:
			{
				if (fValue >= 0.0f)
				{
					return Floor(fValue);
				}
				else
				{
					return Ceil(fValue);
				}
			}
			case kRoundDown:
			{
				return Floor(fValue);
			}
			case kRoundAwayFrom0:
			{
				if (fValue >= 0.0f)
				{
					return Ceil(fValue);
				}
				else
				{
					return Floor(fValue);
				}
			}
			case kRoundUp:
			{
				return Ceil(fValue);
			}
			default:
				GEN_ERROR( "Invalid rounding method" );
		}
	}

	else // Multiple != 1.0
	{
		// Ensure multiple is non-zero
		GEN_ASSERT( fMultipleOf != 0.0f, "Invalid parameter" );
	
		// Select rounding method - often need different technique for +ve and -ve numbers
		switch (eRounding)
		{
			case kRoundNearest:
			{
				return fPosMultipleOf * Floor((fValue + 0.5f) / fPosMultipleOf);
			}
			case kRoundTowards0:
			{
				if (fValue >= 0.0f)
				{
					return fPosMultipleOf * Floor(fValue / fPosMultipleOf);
				}
				else
				{
					return fPosMultipleOf * Ceil(fValue / fPosMultipleOf);
				}
			}
			case kRoundDown:
			{
				return fPosMultipleOf * Floor(fValue / fPosMultipleOf);
			}
			case kRoundAwayFrom0:
			{
				if (fValue >= 0.0f)
				{
					return fPosMultipleOf * Ceil(fValue / fPosMultipleOf);
				}
				else
				{
					return fPosMultipleOf * Floor(fValue / fPosMultipleOf);
				}
			}
			case kRoundUp:
			{
				return fPosMultipleOf * Ceil(fValue / fPosMultipleOf);
			}
			default:
				GEN_ERROR( "Invalid rounding method" );
		}
	}

	GEN_ENDGUARD;
}


// Round 64-bit floating point value to a multiple of another value, default multiple is 1.0.
// Supply rounding method to use, default is kRoundNearest. Round(x) rounds to nearest integer
TFloat64 Round
(
	const TFloat64        fValue,
	const ERoundingMethod eRounding /*= kRoundNearest*/,
	const TFloat64        fMultipleOf /*= 1.0f*/
)
{
	GEN_GUARD;

	// Get positive multiple
	TFloat64 fPosMultipleOf = Abs( fMultipleOf );

	// More efficient methods if multiple is 1.0
	if (fPosMultipleOf == 1.0)
	{
		// Select rounding method - often need different technique for +ve and -ve numbers
		switch (eRounding)
		{
			case kRoundNearest:
			{
				return Floor(fValue + 0.5);
			}
			case kRoundTowards0:
			{
				if (fValue >= 0)
				{
					return Floor(fValue);
				}
				else
				{
					return Ceil(fValue);
				}
			}
			case kRoundDown:
			{
				return Floor(fValue);
			}
			case kRoundAwayFrom0:
			{
				if (fValue >= 0.0)
				{
					return Ceil(fValue);
				}
				else
				{
					return Floor(fValue);
				}
			}
			case kRoundUp:
			{
				return Ceil(fValue);
			}
			default:
				GEN_ERROR( "Invalid rounding method" );
		}
	}

	else // Multiple != 1.0
	{
		// Ensure multiple is non-zero
		GEN_ASSERT( fMultipleOf != 0.0, "Invalid parameter" );
	
		// Select rounding method - often need different technique for +ve and -ve numbers
		switch (eRounding)
		{
			case kRoundNearest:
			{
				return fPosMultipleOf * Floor((fValue + 0.5) / fPosMultipleOf);
			}
			case kRoundTowards0:
			{
				if (fValue >= 0.0)
				{
					return fPosMultipleOf * Floor(fValue / fPosMultipleOf);
				}
				else
				{
					return fPosMultipleOf * Ceil(fValue / fPosMultipleOf);
				}
			}
			case kRoundDown:
			{
				return fPosMultipleOf * Floor(fValue / fPosMultipleOf);
			}
			case kRoundAwayFrom0:
			{
				if (fValue >= 0.0)
				{
					return fPosMultipleOf * Ceil(fValue / fPosMultipleOf);
				}
				else
				{
					return fPosMultipleOf * Floor(fValue / fPosMultipleOf);
				}
			}
			case kRoundUp:
			{
				return fPosMultipleOf * Ceil(fValue / fPosMultipleOf);
			}
			default:
				GEN_ERROR( "Invalid rounding method" );
		}
	}

	GEN_ENDGUARD;
}


} // namespace gen
