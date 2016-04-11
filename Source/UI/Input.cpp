
/*******************************************
	
	Input.cpp

	Key input functions

	Used in the same way as the TL-Engine

********************************************/

#include "Input.h"

namespace gen
{

//////////////////////////////////
// Globals

EKeyState  g_aiKeyStates[kMaxKeyCodes];


//////////////////////////////////
// Initialisation

void InitInput()
{
	// Initialise input data
	for (int i = 0; i < kMaxKeyCodes; ++i)
	{
		g_aiKeyStates[i] = kNotPressed;
	}
}


//////////////////////////////////
// Events

// Event called to indicate that a key has been pressed down
void KeyDownEvent( EKeyCode eKeyCode )
{
	if (g_aiKeyStates[eKeyCode] == kNotPressed)
	{
		g_aiKeyStates[eKeyCode] = kPressed;
	}
	else
	{
		g_aiKeyStates[eKeyCode] = kHeld;
	}
}

// Event called to indicate that a key has been lifted up
void KeyUpEvent( EKeyCode eKeyCode )
{
	g_aiKeyStates[eKeyCode] = kNotPressed;
}


//////////////////////////////////
// Input functions

// Returns true when a given key or button is first pressed down. Use
// for one-off actions or toggles. Example key codes: Key_A or
// Mouse_LButton, see input.h for a full list.
bool KeyHit( EKeyCode eKeyCode )
{
	if (g_aiKeyStates[eKeyCode] == kPressed)
	{
		g_aiKeyStates[eKeyCode] = kHeld;
		return true;
	}
	return false;
}

// Returns true as long as a given key or button is held down. Use for
// continuous action or motion. Example key codes: Key_A or
// Mouse_LButton, see input.h for a full list.
bool KeyHeld( EKeyCode eKeyCode )
{
	if (g_aiKeyStates[eKeyCode] == kNotPressed)
	{
		return false;
	}
	g_aiKeyStates[eKeyCode] = kHeld;
	return true;
}

		
} // namespace gen