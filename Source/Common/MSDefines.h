/**************************************************************************************************
	Module:       MSDefines.h
	Author:       Laurent Noel
	Date created: 23/09/05

	Utility functions for Microsoft platforms

	Copyright 2005-2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 23/09/05 - LN
**************************************************************************************************/

#ifndef GEN_MS_DEFINES_H_INCLUDED
#define GEN_MS_DEFINES_H_INCLUDED

// Windows shell library for PathFileExists function
#pragma comment(lib, "shlwapi.lib") 

#include <string>
using namespace std;

namespace gen
{

/*------------------------------------------------------------------------------------------------
	Compiler settings
 ------------------------------------------------------------------------------------------------*/

// Check compiler version
#if _MSC_VER < 1300
	#error "Compiler version not supported - use Visual Studio .NET or better"
#endif

// Check compiler options
#ifndef _CPPUNWIND
	#error "Bad compiler option: C++ exception handling must be enabled"
#endif

// Disable unwanted warnings
#pragma warning(disable : 4239) // nonstandard extension used : conversion from 'type' to 'type'
                                // Occurs when auto_ptr returned from function (should be standard)
#pragma warning(disable : 4250) // 'class1' : inherits 'class2::member' via dominance
                                // Dominance is normal in diamond inheritance paterns
#pragma warning(disable : 4324) // 'struct_name' : structure was padded due to __declspec(align())
                                // Some structures are aligned/padded to improve cache efficiency
#pragma warning(disable : 4480) // nonstandard extension used: specifying underlying type for enum
                                // Enum size defined through compiler-specific macro TLX_TYPED_ENUM
#pragma warning(disable : 4702) // unreachable code
                                // Many warnings in standard C++ library in .NET

// Visual Studio 2005 only
#if _MSC_VER >= 1400
	#pragma warning(disable : 4996) // 'function': was declared deprecated
	                                // Not using secure versions of CRT functions
	// Could also disable checked iterators in release builds (SECURE_SCL=0 in project settings)
#endif


/*------------------------------------------------------------------------------------------------
	Macros
 ------------------------------------------------------------------------------------------------*/

// Prefix to align a structure or class in memory to a multiple of the given amount
#define GEN_ALIGN(a) __declspec(align(a))


/*------------------------------------------------------------------------------------------------
	Constants
 ------------------------------------------------------------------------------------------------*/

// Define compiler name
#if _MSC_VER < 1300
	static const string ksCompiler = "Visual C++ 6.0";
#elif _MSC_VER < 1310
	static const string ksCompiler = "Visual C++ 7.0 (.NET 2002)";
#elif _MSC_VER < 1400
	static const string ksCompiler = "Visual C++ 7.1 (.NET 2003)";
#else
	static const string ksCompiler = "Visual C++ 8.0 (2005) or greater";
#endif


// String locale
const string ksPathSeparator = "\\";
const string ksNewline = "\n";


/*------------------------------------------------------------------------------------------------
	Types
 ------------------------------------------------------------------------------------------------*/

// Typedefs for fixed size types
typedef signed char      TInt8;
typedef signed short     TInt16;
typedef signed int       TInt32;
typedef signed __int64   TInt64;

typedef unsigned char    TUInt8;
typedef unsigned short   TUInt16;
typedef unsigned int     TUInt32;
typedef unsigned __int64 TUInt64;

typedef float            TFloat32;
typedef double           TFloat64;


/*------------------------------------------------------------------------------------------------
	MS-specific GUI support
 ------------------------------------------------------------------------------------------------*/

// System message box used to display errors or warnings. Defaults to having an OK button only, but
// can request Yes/No buttons. Return value is whether the Yes or OK button was pressed.
bool SystemMessageBox
(
	const string& sMessage,                       // Main message to display
	const string& sCaption = "TL-Engine Extreme", // Caption to display at top of box
	const bool    bYesNo = false                  // Display Yes and No buttons instead of OK
);


} // namespace gen

#endif // GEN_MS_DEFINES_H_INCLUDED
