/**************************************************************************************************
	Module:       Defines.h
	Author:       Laurent Noel
	Date created: 23/09/05

	Core definitions used in the majority of files

	Copyright 2005-2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 23/09/05 - LN
**************************************************************************************************/

#ifndef GEN_DEFINES_H_INCLUDED
#define GEN_DEFINES_H_INCLUDED

#include <string>
using namespace std;

// Include platform specific definitions
#if defined (_MSC_VER)
	#include "MSDefines.h" // _MSC_VER is only defined on Microsoft compilers
#else
	#error "Unsupported OS/compiler - only Windows and Visual Studio supported at present"
#endif

namespace gen
{

/*------------------------------------------------------------------------------------------------
	Constants
 ------------------------------------------------------------------------------------------------*/

// Define constant if we have IEC 559 (= IEEE 754) conformant floats (used in maths classes)
#if defined(__STDC_IEC_559__) || defined(_MSC_VER) // Check for C99 constant or MS compiler
	#define IEC_559_FLOATS
#endif


/*------------------------------------------------------------------------------------------------
	Helper macros
 ------------------------------------------------------------------------------------------------*/

// Specify that a parameter is (deliberately) unreferenced
#define GEN_UNREFERENCED_PARAMETER( p ) (p)


/*------------------------------------------------------------------------------------------------
	Common types
 ------------------------------------------------------------------------------------------------*/

// Define an integral type to hold enum values
typedef TInt32 TEnumInt;


/*------------------------------------------------------------------------------------------------
	Constants
 ------------------------------------------------------------------------------------------------*/

// Constant to indicate a default (non-constant) value
const TUInt32 kiDefaultValue = 0x7fffffff;

// Constant to indicate an unspecified value (e.g. unknown, n/a, free choice etc.)
const TUInt32 kiUnspecifiedValue = 0x7ffffffe;

// Constant to indicate the previous value in the same context
const TUInt32 kiPreviousValue = 0x7ffffffd;


/*------------------------------------------------------------------------------------------------
	Run-time type information
 ------------------------------------------------------------------------------------------------*/
// Basic class information, available even when RTTI is unavailable
// For C++ native RTTI, see help for typeid and dynamic_cast

// Class/struct declaration macros, defines a static string for the class name. Should be defined
// at top of classes (because of access use). Not necessary for pure interfaces
#define GEN_STRUCT( sClassName ) public:\
	static const char* ClassName() { return #sClassName; }

#define GEN_CLASS( sClassName )\
	GEN_STRUCT( sClassName ) private:


// Class/struct declaration macros with named objects, use SetObjectName member for naming
#define GEN_STRUCT_NAMED( sClassName ) public:\
	static const char* ClassName() { return #sClassName; }\
	const char* ObjectName() { return _sObjectName.c_str(); }\
	void SetObjectName( const char* sObjectName ) { _sObjectName = sObjectName; }\
	string _sObjectName;

#define GEN_CLASS_NAMED( sClassName )\
	GEN_STRUCT_NAMED( sClassName ) private:


// Define null class & object functions at global scope so always defined
inline const char* ClassName() { return ""; }
inline const char* ObjectName() { return ""; }
inline void SetObjectName( const char* ) {}


} // namespace gen

#endif // GEN_DEFINES_H_INCLUDED
