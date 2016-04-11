/**************************************************************************************************
	Module:       CHashTable.h
	Author:       Laurent Noel

	Hash table class storing keys and associated values, supporting quick lookup of a value for a
	given a key. A hashing function is needed for the mapping and is specified for the constructor
	
	This is a template class, which allows any types for keys and values. E.g. to implement entity
	UIDs the key is an integer (the UID), and the value is an entity pointer. For a phonebook, the
	key and value are both strings (name and the phone number - a string for flexibility).
	Templates are a form of "generic" programming. They are very powerful and encourage code reuse.
	Be careful though, C++ template syntax is rather tricky.

	Copyright 2007, University of Central Lancashire and Laurent Noel
**************************************************************************************************/

#ifndef GEN_C_HASH_TABLE_H_INCLUDED
#define GEN_C_HASH_TABLE_H_INCLUDED

#include <math.h>
#include <iostream>
#include <list>
using namespace std;

#include "Defines.h"
#include "Error.h"

namespace gen
{

/*------------------------------------------------------------------------------------------------
	Hashing functions
 ------------------------------------------------------------------------------------------------*/

// This hash map class allows the use of different hash functions. To allow this we first define
// a function pointer *type* for hash functions. This defines the function prototype for user-
// provided hash functions: the parameters are a pointer to the key data (as a sequence of bytes),
// and the length of the key data in bytes. This allows keys of any type to be hashed. The hash
// function returns a 4-byte integer, the index to use for the value associated with the key
typedef TUInt32 (*THashFunction)( const TUInt8* key, const TUInt32 keyLen );

// Basic hashing function - simply adds up each byte in the key to give the resultant index
TUInt32 AddUpHash( const TUInt8* pKey, const TUInt32 iKeyLen );

// Jenkins one-at-a-time hashing function, a high performance hashing function with good
// distribution of indexes (few collisions)
TUInt32 JOneAtATimeHash( const TUInt8* pKey, const TUInt32 iKeyLen );


/*---------------------------------------------------------------------------------------------
	CHashTable class
---------------------------------------------------------------------------------------------*/

// This is a template class, allowing any types for key and value. Template classes must have
// their member functions defined in the class definition or they won't be instantiated (will
// get link errors). Ask your tutor if you want the full reason for this. However, simply put,
// we must always write code for template class member functions in the header file
//
// Often a template class or function has restrictions on the types that can be used, these
// must be documented. Here the key type (TKeyType here) must have operator== (comparison) and
// operator= (assignment) defined and the value type must have operator= defined. This is no
// problem for entity look-up (keys are UIDs (integers), values are pointers), or phonebooks
// (keys and values are STL strings) - these are standard types have both == and = defined.
// However, in other cases we may need to implement/overload the == and = operators or the
// class would not compile.
// A further restriction is that keys must not contain pointers (although values can). This is
// because the hash function treats keys as a sequence of raw bytes, pointers are not followed
// and the data pointed at will not be hashed
template <class TKeyType, class TValueType>
class CHashTable
{

/*---------------------------------------------------------------------------------------------
	Constructors / Destructore
---------------------------------------------------------------------------------------------*/
public:
	// Constructor takes initial table size, a hashing function, and the maximum load factor
	// before the table is resized - see data section at end
	CHashTable
	(
		const TUInt32  iInitialSize,         // Initial size for the hash table
		THashFunction  pfHashFunction,       // Hashing function to use
		const TFloat32 fMaxLoadFactor = 0.7f // Maximum load factor
	) : m_kfMaxLoadFactor( fMaxLoadFactor ), m_kpfHashFunction( pfHashFunction )
	{
		GEN_GUARD;

		// Allocate initial hash table array
		m_iSize = iInitialSize;
		m_aBuckets = new TBucket[m_iSize];
		GEN_ASSERT( m_aBuckets, "Fatal memory error reserving hash table memory" );

		// Starting with no hash table entries
		m_iNumEntries = 0;

		GEN_ENDGUARD;
	}

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	// I frequently use this code sequence to avoid shallow/deep copying problems - these
	// lines will cause a compile error if I try to copy or assign an object of this class.
	// If I need to allow copying, then it reminds me to check/implement these functions.
	CHashTable( const CHashTable& );
	CHashTable& operator=( const CHashTable& );

public:
	// Destructor to free hash table memory
	~CHashTable()
	{
		delete[] m_aBuckets;
	}


/*---------------------------------------------------------------------------------------------
	Public interface
---------------------------------------------------------------------------------------------*/
public:
	// Looks up value associated with given key and puts in in given pointer. Returns true if
	// the key was found
	bool LookUpKey
	(
		const TKeyType& key,
		TValueType*     pValue
	)
	{
		// Find the index of the bucket associated with this key (will use hashing function)
		TUInt32 iBucket = FindBucket( key );

		// Search the bucket to find the the given key
		TKeyValuePairIter itKeyValuePair = FindKeyValuePair( iBucket, key );

		// Not found (reached end of list), return false
		if (itKeyValuePair == m_aBuckets[iBucket].end())
		{
			return false;
		}

		// Found key, copy its value out and return true
		*pValue = itKeyValuePair->value;
		return true;
	}


	// Add the given key-value pair to the table, if the key already exists, just update its value
	void SetKeyValue
	(
		const TKeyType&   key,
		const TValueType& value
	)
	{
		// Find the index of the bucket associated with this key (will use hashing function)
		TUInt32 iBucket = FindBucket( key );

		// See if given key already exists in the bucket 
		TKeyValuePairIter itKeyValuePair = FindKeyValuePair( iBucket, key );
		if (itKeyValuePair != m_aBuckets[iBucket].end())
		{
			// If key already exists, simply update the value associated with it
			itKeyValuePair->value = value;
		}
		else // otherwise a new key/value pair needs to be inserted in the bucket
		{
			// Check loading of table - if too full, then double it in size
			if (m_iNumEntries > m_iSize * m_kfMaxLoadFactor)
			{
				Resize( m_iSize * 2 );
				iBucket = FindBucket( key ); // Find new bucket for key after resizing
			}

			// Create a new key/value pair and add it to the list in this bucket
			TKeyValuePair newPair;
			newPair.key = key;
			newPair.value = value;
			m_aBuckets[iBucket].push_back( newPair );

			// Increase total number of entries in hash table
			++m_iNumEntries;
		}
	}


	// Remove the given key (and associated value) from the table, returns false if not found
	bool RemoveKey(	const TKeyType& key )
	{
		// Find the index of the bucket associated with this key (will use hashing function)
		TUInt32 iBucket = FindBucket( key );

		// Search the bucket to find the the given key
		TKeyValuePairIter itKeyValuePair = FindKeyValuePair( iBucket, key );

		// If not found then nothing to do
		if (itKeyValuePair == m_aBuckets[iBucket].end())
		{   
			return false;
		}

		// Remove the found key from the bucket
		m_aBuckets[iBucket].erase( itKeyValuePair );

		// Decrease number of table entries - note that table is never resized downwards
		--m_iNumEntries; 

		return true;
	}


	// Remove all keys and associated values
	void RemoveAllKeys()
	{
		for (TUInt32 iBucket = 0; iBucket < m_iSize; ++iBucket)
		{
			m_aBuckets[iBucket].clear();
		}
	}


	// Output a table illustrating the number of entries in each bucket - that is the number
	// of keys that correspond to each hash value. Ideally there should always be 0 or 1 - no
	// collisions. As ideal has functions are hard to produce, there will be some keys that
	// have the same hash and so end up in the same bucket. This reduces the efficiency of the
	// hash table - we find the bucket associated with our key, if it has multiple entries, we
	// must search through them all. So we aim for a hash function that minimises the number
	// of such situations. This function will show up good / bad hash functions
	void OutputDistribution() const
	{
		cout << "Hash Table Distribution:" << endl << endl;
		
		// Calculate the average size of those buckets that contain keys. This gives an idea of the
		// efficiency to look up a key
		TUInt32 iAverageBucketSize = 0;
		TUInt32 iUsedBuckets = 0;

		// Output in a square based on table size
		TUInt32 iBucket = 0;
		while (iBucket != m_iSize)
		{
			TUInt32 iCollision = static_cast<TUInt32>(m_aBuckets[iBucket].size());
			// Output a digit if less than 10 entries in a bucket
			if (iCollision < 10)
			{
				cout << iCollision;
			}
			else
			{
				cout << '+'; // Output '+' for 10 or more entries
			}
			if (iCollision > 0)
			{
				iAverageBucketSize += iCollision;
				++iUsedBuckets;
			}
			++iBucket;
		}
		cout << endl << "% used buckets: " << 100.0f * static_cast<float>(iUsedBuckets) / m_iSize;
		cout << endl << "Average (used) bucket size: " 
		     << static_cast<float>(iAverageBucketSize) / iUsedBuckets << endl;
		cout << endl;
	}

/*-----------------------------------------------------------------------------------------
	Private interface
-----------------------------------------------------------------------------------------*/
private:

	/*---------------------------------------------------------------------------------------------
		Types
	---------------------------------------------------------------------------------------------*/

	// A key/value pair held by the hash table
	struct TKeyValuePair
	{
		TKeyType         key;
		TValueType       value;
	};

	// A bucket is a list of key/value pairs that have the same hash index. The list only has
	// more than one entry if there has been a collision from the hashing function
	// Define a couple of types to make for better readability
	typedef list<TKeyValuePair>        TBucket;
	typedef typename TBucket::iterator TKeyValuePairIter;
	// Use of templates is powerful, but can cause syntax headaches - the need for "typename"
	// here is an example


	/*---------------------------------------------------------------------------------------------
		Support functions
	---------------------------------------------------------------------------------------------*/

	// Find the index of the bucket that should contain the given key
	TUInt32 FindBucket(	const TKeyType& key	) const
	{
		// Get a pointer to the key as raw bytes - this cast is OK for this kind of purpose
		const TUInt8* pKeyData = reinterpret_cast<const TUInt8*>(&key);

		// Use hashing function to convert key data to a single 4-byte integer
		TUInt32 iIndex = m_kpfHashFunction( pKeyData, sizeof(TKeyType) );
		
		// Convert this 4-byte hash value to a bucket index. We have m_iSize buckets, so just
		// use the integer modulus operator. Could use faster bitwise operator if number of
		// buckets was a power of 2, but will deal with the general case here
		iIndex %= m_iSize;

		return iIndex;
	}


	// Find the key/value pair associated with the given key in the given bucket
	// Returns the end of list iterator if not found
	TKeyValuePairIter FindKeyValuePair
	(
		const int       iBucket,
		const TKeyType& key
	) const
	{
		// Start at beginning of bucket and step through each key/value pair
		TKeyValuePairIter itKeyValuePair = m_aBuckets[iBucket].begin();
		while (itKeyValuePair != m_aBuckets[iBucket].end())
		{
			// If we find a matching key, then quit loop
			if (key == itKeyValuePair->key)
			{
				break;
			}
			++itKeyValuePair;
		}

		// Return found key/value pair, or end of list iterator if not found
		return itKeyValuePair;
	}

	// Resize the hash table - reinserts all keys
	void Resize( const TUInt32 iNewSize )
	{
		GEN_GUARD;

		// Store old buckets and size
		TUInt32 iOldSize = m_iSize;
		TBucket* aOldBuckets = m_aBuckets;

		// Update size and create new set of buckets
		m_iSize = iNewSize;
		m_aBuckets = new TBucket[m_iSize];
		GEN_ASSERT( m_aBuckets, "Fatal memory error reserving hash table memory" );

		// Go through old buckets and set each key/value pair into new buckets
		m_iNumEntries = 0;
		for (TUInt32 iBucket = 0; iBucket < iOldSize; ++iBucket)
		{
			while (aOldBuckets[iBucket].size())
			{
				SetKeyValue( aOldBuckets[iBucket].front().key, aOldBuckets[iBucket].front().value );
				aOldBuckets[iBucket].pop_front(); // Delete each old key/value pair after it is copied
			}
		}

		delete[] aOldBuckets;

		GEN_ENDGUARD;
	}


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	TBucket* m_aBuckets;    // Dynamically allocated array of buckets of key/value pairs
	TUInt32  m_iSize;       // Size (capacity) of the table - number of buckets
	TUInt32  m_iNumEntries; // Number of key/value pairs in the table

	// Hash function to use is stored as a function pointer - converts a key given as a
	// sequence of bytes into a 4-byte unsigned integer
	const THashFunction m_kpfHashFunction;

	// If table becomes too full, then it is increased in size to avoid hash collisions. The max
	// load factor defines how full it needs to be before this happens. In this implementation, the
	// table is never decreased in size
	const TFloat32 m_kfMaxLoadFactor;
};


} // namespace gen

#endif // GEN_C_HASH_TABLE_H_INCLUDED
