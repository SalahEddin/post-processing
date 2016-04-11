/**************************************************************************************************
	Module:       CFatalException.cpp
	Author:       Laurent Noel
	Date created: 30/07/05

	Implementation of the CFatalException class

	Copyright 2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 30/07/05 - LN
**************************************************************************************************/

#include "CFatalException.h"
#include "Utility.h"

namespace gen
{

// Display the exception details to the user
void CFatalException::Display() const
{
	// Append all information into a single text string
	string sMessage = m_sDescription + ksNewline + ksNewline;

	// Strip any path from filename
	sMessage += "File: " + LastDelimitedSubstr( m_sFileName, ksPathSeparator );

	// If line number -ve then unknown line number
	if (m_iLineNum >= 0)
	{
		sMessage += ",  Line: " + ToString( m_iLineNum );
	}
	sMessage += ksNewline + ksNewline + "Call stack: " + m_sCallStack;

	SystemMessageBox( sMessage, "Fatal Exception" );
}


// Append current class, object and function names to the call stack string as each function in
// the call stack unwinds. Can specify if this is the final (root) entry to add to the stack
void CFatalException::AppendToCallStack
(
	const char* sFunction,
	const char* sObject,
	bool        bRoot /*= false*/
)
{
	m_sCallStack += sFunction;
	if (*sObject != 0)
	{
		m_sCallStack += "[";
		m_sCallStack += sObject;
		m_sCallStack += "]";
	}
	if (!bRoot)
	{
		m_sCallStack += " <- ";
	}
}


} // namespace gen