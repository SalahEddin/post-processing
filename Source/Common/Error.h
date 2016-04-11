/**************************************************************************************************
	Module:       Error.h
	Author:       Laurent Noel
	Date created: 04/08/05

	Brings together exception classes and adds various error handling macros

	Copyright 2005-2007, University of Central Lancashire and Laurent Noel

	Change history:
		V1.0    Created 04/08/05 - LN
**************************************************************************************************/

#ifndef GEN_ERROR_H_INCLUDED
#define GEN_ERROR_H_INCLUDED

#include "Defines.h"
#include "CFatalException.h"

#define GEN_NO_OPT_TESTS_RELEASE

namespace gen
{

/*------------------------------------------------------------------------------------------------
	Macros
 ------------------------------------------------------------------------------------------------*/

/////////////////////////////////////
// Errors and Assertions

// Note that these macros should be used to catch fatal errors - unexpected unrecoverable errors.
// It is better in some cases to attempt to recover from an error, for example by returning an
// error code or a non-fatal exception

// Immediately throw exception with a message. Used to flag if code has reached an invalid point
// e.g. reaching the default section in a switch statement when all situations have specific cases
#define GEN_ERROR( sError )\
	throw gen::CFatalException( (sError), __FILE__, __LINE__ );

// Assert that a condition is true or throw exception. Use to test critical code preconditions
// E.g. function parameters are within correct ranges
#define GEN_ASSERT( bCondition, sError )\
	if (!(bCondition)) { throw gen::CFatalException( (sError), __FILE__, __LINE__ ); }

// Macro for unimplemented functions. Put in an otherwise empty function that is yet to be
// implemented. Will catch any attempted calls.
#define GEN_UNIMPLEMENTED\
	GEN_ERROR( "Function unimplemented" );


/////////////////////////////////////
// Exception guards

// Exception guards are macro code blocks for functions that catch all exceptions and rethrow
// them as CFatalException types. These are repeatedly rethrown, generating a call stack, until
// picked up and displayed when thrown into a sentry block (see below)

// Start a guarded block with a GEN_GUARD statement
#define GEN_GUARD\
	try\
	{

// Use GEN_CATCHGUARD as a catch all handler on an existing try block
#define GEN_CATCHGUARD\
	catch( gen::CFatalException& e ) { e.AppendToCallStack( __FUNCTION__, ObjectName() ); throw e; }\
	catch( ... ) { throw gen::CFatalException( __FILE__, __FUNCTION__, ObjectName() ); }

// Finish a guarded block with a GEN_ENDGUARD statement
#define GEN_ENDGUARD\
	}\
	GEN_CATCHGUARD


// Exception sentry used with guards above, wraps the outer code block that calls guarded functions
#define GEN_SENTRY\
	try\
	{

#define GEN_ENDSENTRY\
	}\
	catch( gen::CFatalException& e )\
	{ e.AppendToCallStack( __FUNCTION__, ObjectName(), true ); e.Display(); exit( EXIT_FAILURE ); }\
	catch( ... )\
	{ gen::CFatalException e( "Unknown Exception", __FILE__, __LINE__ ); \
	  e.AppendToCallStack( __FUNCTION__, ObjectName(), true ); e.Display(); exit( EXIT_FAILURE ); }


/////////////////////////////////////
// Optional Error Tests

// Optional guards / tests are removed in release builds if user defines GEN_NO_OPT_TESTS_RELEASE
// before this point. This allows for debugging tests that are removed from time-critical code on
// release. Use sparingly

#if defined(_DEBUG) || !defined(GEN_NO_OPT_TESTS_RELEASE)
	#define GEN_ASSERT_OPT( bCondition, sError ) GEN_ASSERT( bCondition, sError )
	#define GEN_ERROR_OPT( sError ) GEN_ERROR( sError )
	#define GEN_GUARD_OPT GEN_GUARD
	#define GEN_CATCHGUARD_OPT GEN_CATCHGUARD
	#define GEN_ENDGUARD_OPT GEN_ENDGUARD
	#define GEN_SENTRY_OPT GEN_SENTRY
	#define GEN_ENDSENTRY_OPT GEN_ENDSENTRY
#else
	#define GEN_ASSERT_OPT( bCondition, sError )
	#define GEN_ERROR_OPT( sError )
	#define GEN_GUARD_OPT
	#define GEN_CATCHGUARD_OPT
	#define GEN_ENDGUARD_OPT
	#define GEN_SENTRY_OPT
	#define GEN_ENDSENTRY_OPT
#endif


} // namespace gen

#endif // GEN_ERROR_H_INCLUDED
