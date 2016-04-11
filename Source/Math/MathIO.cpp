/**************************************************************************************************
	Module:       MathIO.cpp
	Author:       Laurent Noel
	Date created: 11/07/07

	Support for stream input and output for math classes

	Copyright 2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 11/07/07 - LN
**************************************************************************************************/

#include <iostream>
using namespace std;

#include "Defines.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CVector4.h"
#include "CMatrix2x2.h"
#include "CMatrix3x3.h"
#include "CMatrix4x4.h"
#include "CQuaternion.h"

namespace gen
{

/*---------------------------------------------------------------------------------------------
	Vector IO
---------------------------------------------------------------------------------------------*/

ostream& operator<<( ostream& s, const CVector2& v )
{
	return s << '(' << v.x << ", " << v.y << ')';
}

ostream& operator<<( ostream& s, const CVector3& v )
{
	return s << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

ostream& operator<<( ostream& s, const CVector4& v )
{
	return s << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
}


istream& operator>>( istream& s, CVector2& v )
{
	TFloat32 x, y;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing on error
	s >> x >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> y >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		v.Set( x, y );
	}
	return s;
}

istream& operator>>( istream& s, CVector3& v )
{
	TFloat32 x, y, z;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing on error
	s >> x >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> y >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> z >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		v.Set( x, y, z );
	}
	return s;
}

istream& operator>>( istream& s, CVector4& v )
{
	TFloat32 x, y, z, w;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing on error
	s >> x >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> y >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> z >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> w >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		v.Set( x, y, z, w );
	}
	return s;
}


/*---------------------------------------------------------------------------------------------
	Matrix IO
---------------------------------------------------------------------------------------------*/

ostream& operator<<( ostream& s, const CMatrix2x2& m )
{
	return s << '(' << m.e00 << "," << m.e01 << ",  "
	                << m.e10 << "," << m.e11 << ')';
}

ostream& operator<<( ostream& s, const CMatrix3x3& m )
{
	return s << '(' << m.e00 << "," << m.e01 << "," << m.e02 << ",  "
	                << m.e10 << "," << m.e11 << "," << m.e12 << ",  "
	                << m.e20 << "," << m.e21 << "," << m.e22 << ')';
}

ostream& operator<<( ostream& s, const CMatrix4x4& m )
{
	return s << '(' << m.e00 << "," << m.e01 << "," << m.e02 << "," << m.e03 << ",  "
	                << m.e10 << "," << m.e11 << "," << m.e12 << "," << m.e13 << ",  "
	                << m.e20 << "," << m.e21 << "," << m.e22 << "," << m.e23 << ",  "
	                << m.e30 << "," << m.e31 << "," << m.e32 << "," << m.e33 << ')';
}


istream& operator>>( istream& s, CMatrix2x2& m )
{
	TFloat32 e00, e01, e10, e11;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing
	s >> e00 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e01 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	
	s >> e10 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e11 >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		m.Set( e00, e01, e10, e11 );
	}
	return s;
}

istream& operator>>( istream& s, CMatrix3x3& m )
{
	TFloat32 e00, e01, e02, e10, e11, e12, e20, e21, e22;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing on error
	s >> e00 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e01 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e02 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	
	s >> e10 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e11 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e12 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	
	s >> e20 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e21 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e22 >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		m.Set( e00, e01, e02, e10, e11, e12, e20, e21, e22 );
	}
	return s;
}

istream& operator>>( istream& s, CMatrix4x4& m )
{
	TFloat32 e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23, e30, e31, e32, e33;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing on error
	s >> e00 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e01 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e02 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e03 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	
	s >> e10 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e11 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e12 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e13 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	
	s >> e20 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e21 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e22 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e23 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	
	s >> e30 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e31 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e32 >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> e33 >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		m.Set( e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23, e30, e31, e32, e33 );
	}
	return s;
}


/*---------------------------------------------------------------------------------------------
	Quaternion IO
---------------------------------------------------------------------------------------------*/

ostream& operator<<( ostream& s, const CQuaternion& v )
{
	return s << '(' << v.w << "," << v.x << "," << v.y << "," << v.z << ')';
}

istream& operator>>( istream& s, CQuaternion& v )
{
	TFloat32 w, x, y, z;
	char c = 0;

	s >> c;
	if (c != '(') s.clear( ios_base::badbit ); // Subsequent >> operations will do nothing on error
	s >> w >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> x >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> y >> c;
	if (c != ',') s.clear( ios_base::badbit );
	s >> z >> c;
	if (c != ')') s.clear( ios_base::badbit );

	if (s) // Only set output if all input was successful
	{
		v.Set( w, x, y, z );
	}
	return s;
}


} // namespace gen
