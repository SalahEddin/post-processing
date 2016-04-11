/*******************************************
	Messenger.cpp

	Entity messenger class implementation
********************************************/

#include "Messenger.h"

namespace gen
{

/////////////////////////////////////
// Global variables

// Define a single messenger object for the program
CMessenger Messenger;


/////////////////////////////////////
// Message sending/receiving

// Send the given message to a particular UID, does not check if the UID exists
void CMessenger::SendMessage( TEntityUID to, const SMessage& msg )
{
	// Simply insert the UID/message pair into the message map. It will be inserted next
	// to any other pairs with the same UID
	m_Messages.insert( UIDMsgPair( to, msg ) );
}


// Fetch the next available message for the given UID, returns the message through the given 
// pointer. Returns false if there are no messages for this UID
bool CMessenger::FetchMessage( TEntityUID to, SMessage* msg )
{
	// Find the first message for this UID in the message map
	TMessageIter itMessage = m_Messages.find( to );

	// See if no messages for this UID
	if (itMessage == m_Messages.end())
	{
		return false;
	}

	// Return message, then delete it
	*msg = itMessage->second;
	m_Messages.erase( itMessage );

	return true;
}



} // namespace gen
