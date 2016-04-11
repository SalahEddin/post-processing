/**************************************************************************************************
	Module:       MSDefines.cpp
	Author:       Laurent Noel
	Date created: 23/09/05

	Utility functions for Microsoft platforms

	Copyright 2005-2006, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 23/09/05 - LN
**************************************************************************************************/

#include <Windows.h>
#include <AtlBase.h> // Used for string conversion macros (CA2CT below)

#include "Defines.h"
#include "MSDefines.h"
#include "Error.h"

namespace gen
{

/*------------------------------------------------------------------------------------------------
	Helper functions
 ------------------------------------------------------------------------------------------------*/

// Helper function to unadorn function names to define __func__ properly. Not using the similar
// functions from utility.h to avoid header dependency issues, and also to force no inlining - as
// __func__ is used very frequently in exception guards
#pragma auto_inline( off )
string BareFunctionName
(
	const string& sName
)
{
	string::size_type lastDelimiter = sName.find_last_of( ':' );
	if (lastDelimiter == string::npos)
	{
		return sName;
	}
	else
	{
		return sName.substr( lastDelimiter + 1 );
	}
}
#pragma auto_inline( on )


/*------------------------------------------------------------------------------------------------
	OS-specific GUI support
 ------------------------------------------------------------------------------------------------*/

// System message box used to display errors or warnings. Defaults to having an OK button only, but
// can request Yes/No buttons. Return value is whether the Yes or OK button was pressed.
bool SystemMessageBox
(
	const string& sMessage, // Main message to display
	const string& sCaption, // Caption to display at top of box
	const bool    bYesNo    // Display Yes and No buttons instead of OK
)
{
	GEN_GUARD;

	// Using CA2CT macro to convert strings appropriately depending on project Unicode setting
	UINT buttons = (bYesNo ? (MB_YESNO | MB_ICONQUESTION) : (MB_OK | MB_ICONWARNING));
	int iPressed = ::MessageBox( NULL, CA2CT(sMessage.c_str()), CA2CT(sCaption.c_str()), buttons );
	return (iPressed == IDOK || iPressed == IDYES);

	GEN_ENDGUARD;
}


} // namespace gen
