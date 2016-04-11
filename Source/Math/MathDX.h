/**************************************************************************************************
	Module:       MathDX.h
	Author:       Laurent Noel
	Date created: 11/07/07

	Conversions from math classes to DirectX types

	Copyright 2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 11/07/07 - LN
**************************************************************************************************/

// These math classes are designed to be closely compatible with DirectX. Most types can be
// directly cast into their DirectX equivalent with no overhead. In general, such reinterpretation
// casts are non-portable (i.e. may not work on all compilers), but they are likely to work widely,
// certainly so on Microsoft compilers. Below are helper functions to perform the casting in a
// readable fashion

//TODO/WARNING: Not tested on 64-bit compiler/environment

#ifndef GEN_C_MATHDX_H_INCLUDED
#define GEN_C_MATHDX_H_INCLUDED

#include <d3dx9.h>

#include "Defines.h"

namespace gen
{

// Forward declaration of classes, includes not necessary (using pointers and not using class data)
class CVector2;
class CVector3;
class CVector4;
class CMatrix4x4;
class CQuaternion;

/*---------------------------------------------------------------------------------------------
	Vector Conversions
---------------------------------------------------------------------------------------------*/

// Reinterpret CVector2 as a D3DXVECTOR2 - in various forms (const & ptr)
inline D3DXVECTOR2& ToD3DXVECTOR( CVector2& v )
{
	return *reinterpret_cast<D3DXVECTOR2*>(&v);
}

inline const D3DXVECTOR2& ToD3DXVECTOR( const CVector2& v )
{
	return *reinterpret_cast<const D3DXVECTOR2*>(&v);
}

inline D3DXVECTOR2* ToD3DXVECTORPtr( CVector2* pV )
{
	return reinterpret_cast<D3DXVECTOR2*>(pV);
}

inline const D3DXVECTOR2* ToD3DXVECTORPtr( const CVector2* pV )
{
	return reinterpret_cast<const D3DXVECTOR2*>(pV);
}


// Reinterpret a CVector3 as a D3DXVECTOR3 - in various forms (const & ptr)
// Result can also be used as a D3DVECTOR (base class for D3DXVECTOR3)
inline D3DXVECTOR3& ToD3DXVECTOR( CVector3& v )
{
	return *reinterpret_cast<D3DXVECTOR3*>(&v);
}

inline const D3DXVECTOR3& ToD3DXVECTOR( const CVector3& v )
{
	return *reinterpret_cast<const D3DXVECTOR3*>(&v);
}

inline D3DXVECTOR3* ToD3DXVECTORPtr( CVector3* pV )
{
	return reinterpret_cast<D3DXVECTOR3*>(pV);
}

inline const D3DXVECTOR3* ToD3DXVECTORPtr( const CVector3* pV )
{
	return reinterpret_cast<const D3DXVECTOR3*>(pV);
}


// Reinterpret a CVector4 as a D3DXVECTOR4 - in various forms (const & ptr)
inline D3DXVECTOR4& ToD3DXVECTOR( CVector4& v )
{
	return *reinterpret_cast<D3DXVECTOR4*>(&v);
}

inline const D3DXVECTOR4& ToD3DXVECTOR( const CVector4& v )
{
	return *reinterpret_cast<const D3DXVECTOR4*>(&v);
}

inline D3DXVECTOR4* ToD3DXVECTORPtr( CVector4* pV )
{
	return reinterpret_cast<D3DXVECTOR4*>(pV);
}

inline const D3DXVECTOR4* ToD3DXVECTORPtr( const CVector4* pV )
{
	return reinterpret_cast<const D3DXVECTOR4*>(pV);
}


/*---------------------------------------------------------------------------------------------
	Matrix Conversions
---------------------------------------------------------------------------------------------*/
// Note: DirectX has no 2x2 or 3x3 matrix types

// Reinterpret a CMatrix4x4 as a D3DXMATRIX - in various forms (const & ptr)
inline D3DXMATRIX& ToD3DXMATRIX( CMatrix4x4& m )
{
	return *reinterpret_cast<D3DXMATRIX*>(&m);
}

inline const D3DXMATRIX& ToD3DXMATRIX( const CMatrix4x4& m )
{
	return *reinterpret_cast<const D3DXMATRIX*>(&m);
}

inline D3DXMATRIX* ToD3DXMATRIXPtr( CMatrix4x4* pM )
{
	return reinterpret_cast<D3DXMATRIX*>(pM);
}

inline const D3DXMATRIX* ToD3DXMATRIXPtr( const CMatrix4x4* pM )
{
	return reinterpret_cast<const D3DXMATRIX*>(pM);
}


/*---------------------------------------------------------------------------------------------
	Quaternion Conversions
---------------------------------------------------------------------------------------------*/

// Reinterpret a CQuaternion as a D3DXQUATERNION - in various forms (const & ptr)
inline D3DXQUATERNION& ToD3DXQUATERNION( CQuaternion& m )
{
	return *reinterpret_cast<D3DXQUATERNION*>(&m);
}

inline const D3DXQUATERNION& ToD3DXQUATERNION( const CQuaternion& m )
{
	return *reinterpret_cast<const D3DXQUATERNION*>(&m);
}

inline D3DXQUATERNION* ToD3DXQUATERNIONPtr( CQuaternion* pQ )
{
	return reinterpret_cast<D3DXQUATERNION*>(pQ);
}

inline const D3DXQUATERNION* ToD3DXQUATERNIONPtr( const CQuaternion* pQ )
{
	return reinterpret_cast<const D3DXQUATERNION*>(pQ);
}


} // namespace gen

#endif // GEN_C_MATHDX_H_INCLUDED
