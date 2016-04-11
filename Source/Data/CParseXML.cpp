///////////////////////////////////////////////////////////
//  CParseXML.cpp
//  An event driven XML parser (base) class
//  Created on:      30-Jul-2005 14:40:00
//  Original author: LN
///////////////////////////////////////////////////////////

#include <iostream>
#include <memory> // For auto_ptr
using namespace std;

#include "CParseXML.h"

namespace gen
{

/*---------------------------------------------------------------------------------------------
	Constructors / Destructors
---------------------------------------------------------------------------------------------*/

// Constructor initialises the underlying parser
CParseXML::CParseXML()
{
	// Create the Expat parser
	m_Parser = XML_ParserCreate( NULL );
	if (!m_Parser)
	{
	//	throw Failure;
	}

	// Tell the parser to use the static routers below for element handling
	XML_SetElementHandler( m_Parser, StartEltRouter, EndEltRouter );

	// Set the parser's user data to point at this object to enable the routers to work
	XML_SetUserData( m_Parser, this );
}

// Destructor to free the underlying parser
CParseXML::~CParseXML()
{
	// Create the Expat parser
	XML_ParserFree( m_Parser );
}


/*---------------------------------------------------------------------------------------------
	Parsing Functions
---------------------------------------------------------------------------------------------*/

// Parse the the given XML file. The callback functions will handle the actual processing
// of the elements / text read. Can pass the buffer size to use when parsing, the file will be
// read in and parsed in chunks of this size. Returns false on file or parse error
bool CParseXML::ParseFile( const string& fileName, TUInt32 bufferSize /*= 8192*/ )
{
	// Allocate a buffer for reading the file data into - using the C++ standard library
	// <auto_ptr>, a *smart* pointer that will automatically delete itself when it goes out
	// of scope. This allows the function to return at any point and leads to more robust code
	// Can use * and -> on an auto_ptr, but for other needs use the get() function to access
	// the stored pointer
	auto_ptr<char> buffer( new char[bufferSize] );
	if (!buffer.get())
	{
		return false;
	}

	// Open the file (using C library)
	FILE *file = fopen( fileName.c_str(), "r" );
	if (!file)
	{
		return false;
	}

	// Initialise element depth
	m_Depth = 0;

	// While not at the end of the file
	while (!feof( file ))
	{
		// Read some characters from the file
		TUInt32 charsRead = static_cast<TUInt32>(fread( buffer.get(), 1, bufferSize, file ));

		// Parse this next set of bytes
		if (XML_Parse( m_Parser, buffer.get(), charsRead, feof( file ) ) == XML_STATUS_ERROR)
		{
			// Parsing error
			cout << "Parse error at line: " << XML_GetCurrentLineNumber( m_Parser )
				<< " : " << XML_ErrorString(XML_GetErrorCode( m_Parser )) << endl;
			fclose( file );
			return false;
		}
	}

	fclose( file );
	return true;
}


/*---------------------------------------------------------------------------------------------
	Attribute Reading
---------------------------------------------------------------------------------------------*/

// Return the string value associated with the given name in the given attribute list
// Returns defaultValue if the name isn't in the list
string CParseXML::GetAttribute( SAttribute* attrs, const string& name,
                                const string& defaultValue /*= ""*/ )
{
	for (TUInt32 i = 0; attrs[i].name != 0; ++i)
	{
		if (attrs[i].name == name)
		{
			return attrs[i].value;
		}
	}
	return defaultValue;
}

// Return the integer value associated with the given name in the given attribute list
// Returns defaultValue if the name isn't in the list
TInt32 CParseXML::GetAttributeInt( SAttribute* attrs, const string& name,
                                   TInt32 defaultValue /*= 0*/ )
{
	for (TUInt32 i = 0; attrs[i].name != 0; ++i)
	{
		if (attrs[i].name == name)
		{
			return static_cast<TInt32>(atoi( attrs[i].value ));
		}
	}
	return defaultValue;
}

// Return the float value associated with the given name in the given attribute list
// Returns defaultValue if the name isn't in the list
TFloat32 CParseXML::GetAttributeFloat( SAttribute* attrs, const string& name,
                                       TFloat32 defaultValue /*= 0.0f*/ )
{
	for (TUInt32 i = 0; attrs[i].name != 0; ++i)
	{
		if (attrs[i].name == name)
		{
			return static_cast<TFloat32>(atof( attrs[i].value ));
		}
	}
	return defaultValue;
}


/*---------------------------------------------------------------------------------------------
	Member Callback Functions
---------------------------------------------------------------------------------------------*/
// These callback functions are expected to be overrided in derived classes to provide
// processing for specific XML file types

// Callback function called at the start of an new element (opening tag). The element name
// is passed as a (C-style) string. The attributes are passed as a list of (C-style) string
// pairs: attribute name, attribute value. The last attribute is marked with a null name
// Base class version simply writes elements to standard output (for testing)
void CParseXML::StartElt( const string& eltName, SAttribute* attrs )
{
	for (TUInt32 i = 0; i < m_Depth; ++i)
	{
		cout << "  ";
	}
	cout << eltName;
	for (TUInt32 i = 0; attrs[i].name != 0; ++i)
	{
		cout << " " << attrs[i].name << "='" << attrs[i].value << "'";
	}
	cout << endl;
}

// Callback function called at the end of an element (closing tag). The element name
// is passed as a (C-style) string
// Base class version has nothing to do
void CParseXML::EndElt( const string& eltName )
{
}


/*---------------------------------------------------------------------------------------------
	Static Callback Routers
---------------------------------------------------------------------------------------------*/
// Static callback functions - needed since the Expat library is C based and can't use class
// member functions. These functions route the calls to the appropriate parser member functions
// They also reinterpret the attribute parameter into a more convenient form

// Routers for the start of an element (opening tag)
void XMLCALL CParseXML::StartEltRouter( void* userData, const char* eltName, const char** attrs )
{
	// Get the actual parser object from the user data (stored here during the constructor)
	CParseXML* parser = reinterpret_cast<CParseXML*>(userData);

	// Call the parser objects member start element callback. Convert the attributes to
	// a more convenient form too
	parser->StartElt( eltName, reinterpret_cast<SAttribute*>(attrs));

	// Maintain element depth
	parser->m_Depth++;
}

// Routers for the end of an element (closing tag)
void XMLCALL CParseXML::EndEltRouter( void* userData, const char* eltName )
{
	// Get the actual parser object from the user data (stored here during the constructor)
	CParseXML* parser = reinterpret_cast<CParseXML*>(userData);

	// Maintain element depth
	parser->m_Depth--;

	// Call the parser objects member end element callback
	parser->EndElt( eltName );
}


} // namespace gen
