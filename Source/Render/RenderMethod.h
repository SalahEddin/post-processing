/***************************************************************************************
	RenderMethod.h

	Render methods allow a flexible association of mesh materials to shader / texture
	setup. Moves towards using a art-driven method of rendering
****************************************************************************************/

#pragma once

#include <string>
using namespace std;

#include <d3d10.h>
#include <d3dx10.h>

#include "Defines.h"
#include "CMatrix4x4.h"
#include "Camera.h"
#include "Light.h"

namespace gen
{

//-----------------------------------------------------------------------------
// Render method types
//-----------------------------------------------------------------------------

// Customisable list of render methods available for use in materials and implemented in
// RenderMethod.cpp/.h. This list can be changed to to support new rendering methods
enum ERenderMethod
{
	PlainColour        = 0,
	PlainTexture       = 1,
	PixelLit           = 2,
	PixelLitTex        = 3,
	NormalMap          = 4,
	ParallaxMap        = 5,
	PPTint             = 6, // A post-processed material
	PPCutGlass         = 7, // A post-processed material
	NumRenderMethods  // Leave this entry at end
};


// Pointer to a function to initialise a render method - typically sets shader constants
typedef void (*PRenderMethodFn)(D3DXCOLOR* diffuseColour, D3DXCOLOR* specularColour, float specularPower, ID3D10ShaderResourceView** textures, CMatrix4x4* worldMatrix);

// Structure defining a rendering method - defines vertex and pixel shader source files,
// initialisation functions, number of textures used and the structure of the vertex elements
// Also contains DirectX pointers associated with the shaders and the vertex declaration
struct SRenderMethod
{
	string                 techniqueName; // Name of technique in fx file for this render method
	PRenderMethodFn        setupFn;       // Function pointer to custom setup for render method (e.g. to set shader constants)
	
	unsigned int           numTextures;   // How many textures used by the methods (diffuse map, normal map etc.)
	bool                   usesTangents;  // Whether vertex tangents should be calculated for meshes using this method

	bool                   isPostProcess; //**** Whether this render method is a post-process or not. Post process methods are rendered in a second pass (see main code)

	ID3D10EffectTechnique* technique;     // Pointer to actual technique
};



//-----------------------------------------------------------------------------
// Render method usage / information
//-----------------------------------------------------------------------------

// Given a material name and the main texture used by that material, return the render method to
// use for that material. The available render methods are in ERenderMethod in RenderMethod.h
ERenderMethod RenderMethodFromMaterial
(
	const string&  materialName,
	const string&  textureName
);

// Return the number of textures used by a given render method
// If a render method uses multiple textures, secondary texture names will be based on the main
// texture name. E.g. if a normal mapping method uses 3 textures and the main texture is "wall.jpg"
// then the other textures must be named "wall1.jpg" and "wall2.jpg"
// The available render methods are in ERenderMethod in RenderMethod.h
unsigned int NumTexturesUsedByRenderMethod( ERenderMethod method );

// Return whether given render method uses tangents
bool RenderMethodUsesTangents( ERenderMethod method );

// Return whether given render method should be used as a post process
bool RenderMethodIsPostProcess( ERenderMethod method );

// Return the .fx file technique used by given render method
ID3D10EffectTechnique* GetRenderMethodTechnique( ERenderMethod method );

// Use the given method for rendering
void SetRenderMethod( ERenderMethod method, D3DXCOLOR* diffuseColour, D3DXCOLOR* specularColour, float specularPower,
                      ID3D10ShaderResourceView** textures, CMatrix4x4* worldMatrix );


//-----------------------------------------------------------------------------
// Method initialisation
//-----------------------------------------------------------------------------

// Initialise general method data
bool InitialiseMethods();

// Initialises the given render method, returns true on success
bool PrepareMethod( ERenderMethod method );

// Releases the DirectX data associated with all render methods
void ReleaseMethods();


//-----------------------------------------------------------------------------
// Common setup functions - shader variables shared amongst render methods
//-----------------------------------------------------------------------------

// Set the ambient light colour used for all methods
void SetAmbientLight( const SColourRGBA& colour );

// Set the light list to use for all methods
void SetLights( CLight** lights );

// Set the camera to use for all methods
void SetCamera( CCamera* camera );

// Set the scene texture / viewport dimensions used for post-processing material shaders - called from post-processing code
void SetSceneTexture( ID3D10ShaderResourceView* sceneShaderResource, int ViewportWidth, int ViewportHeight );


} // namespace gen