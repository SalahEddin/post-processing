/*******************************************
	GameData.h

	Main scene and game functions
********************************************/

#pragma once

namespace gen
{

///////////////////////////////
// Scene management

// Creates the scene geometry
bool SceneSetup();

// Release everything in the scene
void SceneShutdown();


///////////////////////////////
// Post processing management

// Creates the scene geometry
bool PostProcessSetup();

// Release everything in the scene
void PostProcessShutdown();


///////////////////////////////
// Game loop functions

// Draw one frame of the scene
void RenderScene();

// Render on-screen text each frame
void RenderSceneText();

// Update the scene between rendering
void UpdateScene( float updateTime );

} // namespace gen
