///////////////////////////////////////////////////////////
//  CParseXML.cpp
//  An event driven XML parser (base) class
//  Created on:      30-Jul-2005 14:40:00
//  Original author: LN
///////////////////////////////////////////////////////////

#ifndef GEN_C_PARSE_XML_H_INCLUDED
#define GEN_C_PARSE_XML_H_INCLUDED

#include <string>
using namespace std;

#include <expat.h>

#include "Defines.h"

namespace gen
{

/*---------------------------------------------------------------------------------------------
	CParseXML class
---------------------------------------------------------------------------------------------*/
// A simple C++ wrapper for a XML parser (the Expat XML Parser from www.libexpat.org)
// This is an event-driven parser - it scans through the XML calling callback functions when
// each element (tag) is opened and closed. The developer is expected create derived classes
// for specific XML file types and override the callback functions to deal with the elements
// as appropriate. Note that no validation of the XML is performed
class CParseXML
{

/*---------------------------------------------------------------------------------------------
	Constructors / Destructors
---------------------------------------------------------------------------------------------*/
public:
	// Constructor initialises the underlying parser
	CParseXML();

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	CParseXML( const CParseXML& );
	CParseXML& operator=( const CParseXML& );

public:
	// Destructor to free the underlying parser
	virtual ~CParseXML();


/*---------------------------------------------------------------------------------------------
	Public interface
---------------------------------------------------------------------------------------------*/
public:
	/*---------------------------------------------------------------------------------------------
		Parsing Functions
	---------------------------------------------------------------------------------------------*/
	
	// Parse the the given XML file. The callback functions will handle the actual processing
	// of the elements / text read. Can pass the buffer size to use when parsing, the file will be
	// read in and parsed in chunks of this size. Returns false on file or parse error
	bool ParseFile( const string& fileName, TUInt32 bufferSize = 32768 );


/*---------------------------------------------------------------------------------------------
	Protected interface
---------------------------------------------------------------------------------------------*/
protected:

	/*---------------------------------------------------------------------------------------------
		Types
	---------------------------------------------------------------------------------------------*/

	// Structure to hold element attributes (name/value pairs)
	struct SAttribute
	{
		const char* name;
		const char* value;
	};


	/*---------------------------------------------------------------------------------------------
		Getters
	---------------------------------------------------------------------------------------------*/

	// Allow derived classes to query the current element depth
	TUInt32 GetDepth()
	{
		return m_Depth;
	}

	/*---------------------------------------------------------------------------------------------
		Static Support Functions
	---------------------------------------------------------------------------------------------*/

	// Return the string value associated with the given name in the given attribute list
	// Returns defaultValue if the name isn't in the list
	static string GetAttribute( SAttribute* attrs, const string& name,
	                            const string& defaultValue = "" );
	
	// Return the integer value associated with the given name in the given attribute list
	// Returns defaultValue if the name isn't in the list
	static TInt32 GetAttributeInt( SAttribute* attrs, const string& name,
	                               TInt32 defaultValue = 0 );
	
	// Return the float value associated with the given name in the given attribute list
	// Returns defaultValue if the name isn't in the list
	static TFloat32 GetAttributeFloat( SAttribute* attrs, const string& name,
	                                   TFloat32 defaultValue = 0.0f );
	

/*-----------------------------------------------------------------------------------------
	Private interface
-----------------------------------------------------------------------------------------*/
private:

	/*---------------------------------------------------------------------------------------------
		Callback functions
	---------------------------------------------------------------------------------------------*/
	// These callback functions are expected to be overrided in derived classes to provide
	// processing for specific XML file types

	// Callback function called at the start of an new element (opening tag). The element name
	// is passed as a (C-style) string. The attributes are passed as a list of (C-style) string
	// pairs: attribute name, attribute value. The last attribute is marked with a null name
	virtual void StartElt( const string& eltName, SAttribute* attrs );

	// Callback function called at the end of an element (closing tag). The element name
	// is passed as a (C-style) string
	virtual void EndElt( const string& eltName );


	/*---------------------------------------------------------------------------------------------
		Static Callback Routers
	---------------------------------------------------------------------------------------------*/
	// Static callback functions - needed since the Expat library is C based and can't use class
	// member functions. These functions route the calls to the appropriate parser member functions
	// They also reinterpret the attribute parameter into a more convenient form

	// Routers for the start and end of an element (opening/closing tags)
	static void XMLCALL StartEltRouter( void* userData, const char* eltName, const char** attrs );
	static void XMLCALL EndEltRouter( void* userData, const char* eltName );


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// The underlying Expat parser
	XML_Parser m_Parser;

	// Current depth of elements
	TUInt32    m_Depth;
};


} // namespace gen

#endif // GEN_C_PARSE_LEVEL_H_INCLUDED
