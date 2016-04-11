///////////////////////////////////////////////////////////
//  Colour.h
//  Colour related definitions
//
//  Original author: LN
///////////////////////////////////////////////////////////

#ifndef GEN_COLOUR_H_INCLUDED
#define GEN_COLOUR_H_INCLUDED

#include <d3dx9.h>

#include "Defines.h"

namespace gen
{

// A basic colour structure with red, green, blue and and alpha values from 0->1
// Should really be a concrete class
struct SColourRGBA
{
	SColourRGBA() {}
	SColourRGBA( float initR, float initG, float initB, float initA = 0.0f )
	{
		r = initR;
		g = initG;
		b = initB;
		a = initA;
	}
	
	TFloat32 r, g, b, a;
};
// Colour multiplied by scalar (affects RGB only)
inline SColourRGBA operator*( const SColourRGBA& c, const TFloat32 s ) { return SColourRGBA(c.r*s, c.g*s, c.b*s, c.a); }
inline SColourRGBA operator*( const TFloat32 s, const SColourRGBA& c ) { return SColourRGBA(c.r*s, c.g*s, c.b*s, c.a); }

// Reinterpret a SColourRGBA as a D3DXCOLOR - in various forms (const & ptr)
inline D3DXCOLOR& ToD3DXCOLOR( SColourRGBA& colour )
{
	return *reinterpret_cast<D3DXCOLOR*>(&colour);
}

inline const D3DXCOLOR& ToD3DXCOLOR( const SColourRGBA& colour )
{
	return *reinterpret_cast<const D3DXCOLOR*>(&colour);
}


} // namespace gen

#endif // GEN_COLOUR_H_INCLUDED
