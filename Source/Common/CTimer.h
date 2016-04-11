/*******************************************
	
	CTimer.h

	Timer class declarations

********************************************/

#pragma once


#include "Windows.h"

class CTimer
{
public:

	//////////////////////////////
	// Constructor

	CTimer();

	
	//////////////////////////////
	// Timer control

	// Start the timer running
	void Start();

	// Stop the timer running
	void Stop();

	// Reset the timer to zero
	void Reset();


	//////////////////////////////
	// Timing

	// Get frequency of the timer being used (in counts per second)
	float GetFrequency();

	// Get time passed (seconds) since since timer was started or last reset
	float GetTime();

	// Get time passed (seconds) since last call to this function. If this is the first call, then
	// the time since timer was started or the last reset is returned
	float GetLapTime();


private:
	// Is the timer running
	bool m_Running;


	// Using high resolution timer and if so its frequency
	bool          m_HighRes;
	LARGE_INTEGER m_HighResFreq;

	// Start time and last lap start time of high-resolution timer
	LARGE_INTEGER m_HighResStart;
	LARGE_INTEGER m_HighResLap;

	// Time when high-resolution timer was stopped (if it has been)
	LARGE_INTEGER m_HighResStop;


	// Start time and last lap start time of low-resolution timer
	DWORD m_LowResStart;
	DWORD m_LowResLap;

	// Time when low-resolution timer was stopped (if it has been)
	DWORD m_LowResStop;
};
