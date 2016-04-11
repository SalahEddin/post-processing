/*******************************************
	Messenger.h

	Entity messenger class definitions
********************************************/

#pragma once

#include <map>
using namespace std;

#include "Defines.h"
#include "Entity.h"

namespace gen
{

/////////////////////////////////////
//	Public types

// Some basic message types
enum EMessageType
{
	Msg_Goto,   // Go to a given point (x,y,z)
	Msg_Follow, // Follow a given entity (UID)
	Msg_Stop,   // Stop current action (no data)
};

// A message contains a type, then one of a selection of structures. The "union" structure
// holds several sub-structures or types - all occupying the same memory (on top of each other)
// So only one of the sub-structures can be used by any particular message. E.g. A Msg_Goto
// message suggests that the {pt,distPt} structure should be used - a target point & range.
// This isn't enforced by the language - use of the union is up to the programmer
struct SMessage
{
	// Need to provide copy constructor and assignment operator because this structure contains a union
	// The compiler does not know which of the union contents are in use so it cannot provide default versions
	SMessage(const SMessage& o)
	{
		memcpy(this, &o, sizeof(SMessage)); // Use of memcpy is only safe if SMessage is a "POD" type - data
											// only, no members that need construction. A fair restriction
											// for messages, but does require programmer care.
	}
	SMessage& operator=(const SMessage& o)
	{
		memcpy(this, &o, sizeof(SMessage)); return *this;
	}

	EMessageType type;
	TEntityUID   from;
	union
	{
		struct
		{
			CVector3 pt;
			TFloat32 distPt;
		};
		struct
		{
			TUInt32  UID;
			TFloat32 distUID;
		};
	};
};


// Messenger class allows the sending and receipt of messages between entities - addressed
// by UID
class CMessenger
{
/////////////////////////////////////
//	Constructors/Destructors
public:
	// Default constructor
	CMessenger() {}

	// No destructor needed

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	CMessenger( const CMessenger& );
	CMessenger& operator=( const CMessenger& );


/////////////////////////////////////
//	Public interface
public:

	/////////////////////////////////////
	// Message sending/receiving

	// Send the given message to a particular UID, does not check if the UID exists
	void SendMessage( TEntityUID to, const SMessage& msg );

	// Fetch the next available message for the given UID, returns the message through the given 
	// pointer. Returns false if there are no messages for this UID
	bool FetchMessage( TEntityUID to, SMessage* msg );


/////////////////////////////////////
//	Private interface
private:

	// A multimap has properties similar to a hash map - mapping a key to a value. Here we
	// have the key as an entity UID and the value as a message for that UID. The stored
	// key/value pairs in a multimap are sorted by key, which means all the messages for a
	// particular UID are together. Key look-up is somewhat slower than for a hash map though
	// Define some types to make usage easier
	typedef multimap<TEntityUID, SMessage> TMessages;
	typedef TMessages::iterator TMessageIter;
    typedef pair<TEntityUID, SMessage> UIDMsgPair; // The type stored by the multimap

	TMessages m_Messages;
};


} // namespace gen
