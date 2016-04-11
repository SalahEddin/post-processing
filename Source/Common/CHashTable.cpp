/**************************************************************************************************
	Module:       CHashTable.cpp
	Author:       Laurent Noel

	Hash table class storing keys and associated values, supporting quick lookup of a value for a
	given a key. A hashing function is needed for the mapping and can be specified through the
	constructor

	See header file for further notes

	Copyright 2007, University of Central Lancashire and Laurent Noel
**************************************************************************************************/

#include "CHashTable.h"

namespace gen
{

/*------------------------------------------------------------------------------------------------
	Hashing functions
 ------------------------------------------------------------------------------------------------*/

// Basic hashing function - simply adds up each byte in the key to give the resultant index
TUInt32 AddUpHash( const TUInt8* pKey, const TUInt32 iKeyLen )
{
	TUInt32 iHash = 0;
	TUInt32 iKeyIndex;

	for (iKeyIndex = 0; iKeyIndex < iKeyLen; ++iKeyIndex)
	{
		iHash += pKey[iKeyIndex];

	}

	return iHash;
}


// Jenkins one-at-a-time hashing function, a high performance hashing function with good
// distribution of indexes (few collisions)
TUInt32 JOneAtATimeHash( const TUInt8* pKey, const TUInt32 iKeyLen )
{
	TUInt32 iHash = 0;
	TUInt32 iKeyIndex;

	for (iKeyIndex = 0; iKeyIndex < iKeyLen; ++iKeyIndex)
	{
		iHash += pKey[iKeyIndex];
		iHash += (iHash << 10);
		iHash ^= (iHash >> 6);
	}
	iHash += (iHash << 3);
	iHash ^= (iHash >> 11);
	iHash += (iHash << 15);
	return iHash;
}


} // namespace gen
