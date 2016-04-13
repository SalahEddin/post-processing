//--------------------------------------------------------------------------------------
//	File: Scene.fx
//
//	Standard scene shaders
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

// Matrices for transforming model vertices from model space -> world space -> camera space -> 2D projected vertices
float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;

// Viewport Dimensions - needed for converting pixel coordinates (0->Width, 0->Height) to UV coordinates (0->1) - used in polygon post-processing
float ViewportWidth;
float ViewportHeight;

// Camera position (needed for specular lighting at least)
float3 CameraPos;

// Light data
float3 Light1Pos;
float3 Light2Pos;
float3 Light1Colour;
float3 Light2Colour;
float3 AmbientColour;

// Material colour data
float4 DiffuseColour;
float4 SpecularColour;
float  SpecularPower;

// Other
float ParallaxDepth; // A factor to strengthen/weaken the parallax effect. Cannot exaggerate it too much or will get distortion

// Texture maps
Texture2D DiffuseMap;
Texture2D DiffuseMap2; // Second diffuse map for special techniques (currently unused)
Texture2D NormalMap;

//**** Scene texture used for post-processing materials (same as texture used in PostProcess.fx)
Texture2D SceneTexture;

// Sampler to use with the most texture maps
SamplerState TrilinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Usually use point sampling for the scene texture (i.e. no bilinear/trilinear blending) since don't want to blur it in the copy process
SamplerState PointClamp
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
	MaxLOD = 0.0f;
};


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

// Standard vertex data to be sent into the vertex shader
struct VS_INPUT
{
    float3 Pos     : POSITION;
    float3 Normal  : NORMAL;
	float2 UV      : TEXCOORD0;
};

// Input vertex data with additional tangents for normal mapping
struct VS_NORMALMAP_INPUT
{
    float3 Pos     : POSITION;
    float3 Normal  : NORMAL;
    float3 Tangent : TANGENT;
	float2 UV      : TEXCOORD0;
};


// Minimum vertex shader output 
struct VS_BASIC_OUTPUT
{
    float4 ProjPos       : SV_POSITION;
};

// Vertex shader output for texture only
struct VS_TEX_OUTPUT
{
    float4 ProjPos       : SV_POSITION;
    float2 UV            : TEXCOORD0;
};


// Vertex shader output for pixel lighting with no texture
struct VS_LIGHTING_OUTPUT
{
    float4 ProjPos       : SV_POSITION;
	float3 WorldPos      : POSITION;
	float3 WorldNormal   : NORMAL;
};

// Vertex shader output for pixel lighting with a texture
struct VS_LIGHTINGTEX_OUTPUT
{
    float4 ProjPos       : SV_POSITION;  // 2D "projected" position for vertex (required output for vertex shader)
	float3 WorldPos      : POSITION;
	float3 WorldNormal   : NORMAL;
    float2 UV            : TEXCOORD0;
};

// Vertex shader output for normal mapping
struct VS_NORMALMAP_OUTPUT
{
	float4 ProjPos      : SV_POSITION;
	float3 WorldPos     : POSITION;
	float3 ModelNormal  : NORMAL;
	float3 ModelTangent : TANGENT;
	float2 UV           : TEXCOORD0;
};



//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------

// Basic vertex shader to transform 3D model vertices to 2D only
//
VS_BASIC_OUTPUT VSTransformOnly( VS_INPUT vIn )
{
	VS_BASIC_OUTPUT vOut;
	
	// Transform the input model vertex position into world space, then view space, then 2D projection space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );

	return vOut;
}


// Basic vertex shader to transform 3D model vertices to 2D and pass UVs to the pixel shader
//
VS_TEX_OUTPUT VSTransformTex( VS_INPUT vIn )
{
	VS_TEX_OUTPUT vOut;
	
	// Transform the input model vertex position into world space, then view space, then 2D projection space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	return vOut;
}


// Standard vertex shader for pixel-lit untextured models
//
VS_LIGHTING_OUTPUT VSPixelLit( VS_INPUT vIn )
{
	VS_LIGHTING_OUTPUT vOut;

	// Add 4th element to position and normal (needed to multiply by 4x4 matrix. Recall lectures - set 1 for position, 0 for vector)
	float4 modelPos = float4(vIn.Pos, 1.0f);
	float4 modelNormal = float4(vIn.Normal, 0.0f);

	// Transform model vertex position and normal to world space
	float4 worldPos    = mul( modelPos,    WorldMatrix );
	float3 worldNormal = mul( modelNormal, WorldMatrix );

	// Pass world space position & normal to pixel shader for lighting calculations
   	vOut.WorldPos    = worldPos.xyz;
	vOut.WorldNormal = worldNormal;

	// Use camera matrices to further transform the vertex from world space into view space (camera's point of view) and finally into 2D "projection" space for rendering
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );

	return vOut;
}

// Standard vertex shader for pixel-lit textured models
//
VS_LIGHTINGTEX_OUTPUT VSPixelLitTex( VS_INPUT vIn )
{
	VS_LIGHTINGTEX_OUTPUT vOut;

	// Add 4th element to position and normal (needed to multiply by 4x4 matrix. Recall lectures - set 1 for position, 0 for vector)
	float4 modelPos = float4(vIn.Pos, 1.0f);
	float4 modelNormal = float4(vIn.Normal, 0.0f);

	// Transform model vertex position and normal to world space
	float4 worldPos    = mul( modelPos,    WorldMatrix );
	float3 worldNormal = mul( modelNormal, WorldMatrix );

	// Pass world space position & normal to pixel shader for lighting calculations
   	vOut.WorldPos    = worldPos.xyz;
	vOut.WorldNormal = worldNormal;

	// Use camera matrices to further transform the vertex from world space into view space (camera's point of view) and finally into 2D "projection" space for rendering
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
	vOut.UV = vIn.UV;

	return vOut;
}


// Vertex shader for normal-mapped models
//
VS_NORMALMAP_OUTPUT VSNormalMap( VS_NORMALMAP_INPUT vIn )
{
	VS_NORMALMAP_OUTPUT vOut;

	//Transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	vOut.WorldPos = worldPos.xyz;

	// Transform the vertex from world space into view space and finally into 2D "projection" space for rendering
	float4 viewPos = mul( worldPos, ViewMatrix );
	vOut.ProjPos   = mul( viewPos,  ProjMatrix );

	// Just send the model's normal and tangent untransformed (in model space). The pixel shader will do the matrix work on normals
	vOut.ModelNormal  = vIn.Normal;
	vOut.ModelTangent = vIn.Tangent;

	// Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
	vOut.UV = vIn.UV;

	return vOut;
}



//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

// A pixel shader that just outputs the diffuse material colour
//
float4 PSPlainColour( VS_BASIC_OUTPUT vOut ) : SV_Target
{
	return DiffuseColour;
}

// A pixel shader that just outputs an diffuse map tinted by the diffuse material colour
//
float4 PSTexColour( VS_TEX_OUTPUT vOut ) : SV_Target
{
	float4 diffuseMapColour = DiffuseMap.Sample( TrilinearWrap, vOut.UV );
	return float4( DiffuseColour.xyz * diffuseMapColour.xyz, diffuseMapColour.a ); // Only tint the RGB, get alpha from texture directly
}



// Per-pixel lighting pixel shader using diffuse/specular colours but no maps
//
float4 PSPixelLit( VS_LIGHTING_OUTPUT vOut ) : SV_Target 
{
	// Can't guarantee the normals are length 1 at this point, because the world matrix may contain scaling and because interpolation
	// from vertex shader to pixel shader will also rescale normal. So renormalise the normals we receive
	float3 worldNormal = normalize(vOut.WorldNormal); 


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 cameraDir = normalize(CameraPos - vOut.WorldPos.xyz); // Position of camera - position of current pixel (in world space)
	
	//// LIGHT 1
	float3 light1Dir = normalize(Light1Pos - vOut.WorldPos.xyz);   // Direction for each light is different
	float3 light1Dist = length(Light1Pos - vOut.WorldPos.xyz); 
	float3 diffuseLight1 = Light1Colour * max( dot(worldNormal.xyz, light1Dir), 0 ) / light1Dist;
	float3 halfway = normalize(light1Dir + cameraDir);
	float3 specularLight1 = diffuseLight1 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	//// LIGHT 2
	float3 light2Dir = normalize(Light2Pos - vOut.WorldPos.xyz);
	float3 light2Dist = length(Light2Pos - vOut.WorldPos.xyz);
	float3 diffuseLight2 = Light2Colour * max( dot(worldNormal.xyz, light2Dir), 0 ) / light2Dist;
	halfway = normalize(light2Dir + cameraDir);
	float3 specularLight2 = diffuseLight2 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
	float3 diffuseLight = AmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;


	////////////////////
	// Combine colours 
	
	// Combine maps and lighting for final pixel colour
	float4 combinedColour;
	combinedColour.rgb = DiffuseColour * diffuseLight + SpecularColour * specularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}


// Per-pixel lighting pixel shader using diffuse/specular material colours and combined diffuse/specular map
//
float4 PSPixelLitTex( VS_LIGHTINGTEX_OUTPUT vOut ) : SV_Target
{
	// Can't guarantee the normals are length 1 at this point, because the world matrix may contain scaling and because interpolation
	// from vertex shader to pixel shader will also rescale normal. So renormalise the normals we receive
	float3 worldNormal = normalize(vOut.WorldNormal); 


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 cameraDir = normalize(CameraPos - vOut.WorldPos.xyz); // Position of camera - position of current pixel (in world space)
	
	//// LIGHT 1
	float3 light1Dir = normalize(Light1Pos - vOut.WorldPos.xyz);   // Direction for each light is different
	float3 light1Dist = length(Light1Pos - vOut.WorldPos.xyz); 
	float3 diffuseLight1 = Light1Colour * max( dot(worldNormal.xyz, light1Dir), 0 ) / light1Dist;
	float3 halfway = normalize(light1Dir + cameraDir);
	float3 specularLight1 = diffuseLight1 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	//// LIGHT 2
	float3 light2Dir = normalize(Light2Pos - vOut.WorldPos.xyz);
	float3 light2Dist = length(Light2Pos - vOut.WorldPos.xyz);
	float3 diffuseLight2 = Light2Colour * max( dot(worldNormal.xyz, light2Dir), 0 ) / light2Dist;
	halfway = normalize(light2Dir + cameraDir);
	float3 specularLight2 = diffuseLight2 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
	float3 diffuseLight = AmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;


	////////////////////
	// Material colour

	// Combine diffuse material colour with diffuse map
	float4 diffuseTex = DiffuseMap.Sample( TrilinearWrap, vOut.UV );
	float4 diffuseMaterial = DiffuseColour * diffuseTex;
	
	// Combine specular material colour with specular map held in diffuse map alpha
	float3 specularMaterial = SpecularColour * diffuseTex.a;
	
	////////////////////
	// Combine colours 
	
	// Combine maps and lighting for final pixel colour
	float4 combinedColour;
	combinedColour.rgb = diffuseMaterial * diffuseLight + specularMaterial * specularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}


// Normal mapping pixel shader combining wih diffuse/specular material colours
//
float4 PSNormalMap( VS_NORMALMAP_OUTPUT vOut ) : SV_Target
{
	///////////////////////
	// Normal Map Extraction

	// Will use the model normal/tangent to calculate matrix for tangent space. The normals for each pixel are *interpolated* from the
	// vertex normals/tangents. This means they will not be length 1, so they need to be renormalised (same as per-pixel lighting issue)
	float3 modelNormal = normalize( vOut.ModelNormal );
	float3 modelTangent = normalize( vOut.ModelTangent );

	// Calculate bi-tangent to complete the three axes of tangent space - then create the *inverse* tangent matrix to convert *from*
	// tangent space into model space
	float3 modelBiTangent = cross( modelNormal, modelTangent );
	float3x3 invTangentMatrix = float3x3(modelTangent, modelBiTangent, modelNormal);
	
	// Get the texture normal from the normal map. The r,g,b pixel values actually store x,y,z components of a normal. However, r,g,b
	// values are stored in the range 0->1, whereas the x, y & z components should be in the range -1->1. So some scaling is needed
	float3 textureNormal = 2.0f * NormalMap.Sample( TrilinearWrap, vOut.UV ) - 1.0f; // Scale from 0->1 to -1->1

	// Now convert the texture normal into model space using the inverse tangent matrix, and then convert into world space using the world
	// matrix. Normalise, because of the effects of texture filtering and in case the world matrix contains scaling
	float3 worldNormal = normalize( mul( mul( textureNormal, invTangentMatrix ), WorldMatrix ) );


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 cameraDir = normalize(CameraPos - vOut.WorldPos.xyz); // Position of camera - position of current vertex (or pixel) (in world space)
	
	//// LIGHT 1
	float3 light1Dir = normalize(Light1Pos - vOut.WorldPos.xyz);   // Direction for each light is different
	float3 light1Dist = length(Light1Pos - vOut.WorldPos.xyz); 
	float3 diffuseLight1 = Light1Colour * max( dot(worldNormal.xyz, light1Dir), 0 ) / light1Dist;
	float3 halfway = normalize(light1Dir + cameraDir);
	float3 specularLight1 = diffuseLight1 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	//// LIGHT 2
	float3 light2Dir = normalize(Light2Pos - vOut.WorldPos.xyz);
	float3 light2Dist = length(Light2Pos - vOut.WorldPos.xyz);
	float3 diffuseLight2 = Light2Colour * max( dot(worldNormal.xyz, light2Dir), 0 ) / light2Dist;
	halfway = normalize(light2Dir + cameraDir);
	float3 specularLight2 = diffuseLight2 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
	float3 diffuseLight = AmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;


	////////////////////
	// Material colour

	// Combine diffuse material colour with diffuse map
	float4 diffuseTex = DiffuseMap.Sample( TrilinearWrap, vOut.UV );
	float4 diffuseMaterial = DiffuseColour * diffuseTex;
	
	// Combine specular material colour with specular map held in diffuse map alpha
	float3 specularMaterial = SpecularColour * diffuseTex.a;

	
	////////////////////
	// Combine colours 
	
	// Combine maps and lighting for final pixel colour
	float4 combinedColour;
	combinedColour.rgb = diffuseMaterial * diffuseLight + specularMaterial * specularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}


// Parallax mapping pixel shader combining wih diffuse/specular material colours
//
float4 PSParallaxMap( VS_NORMALMAP_OUTPUT vOut ) : SV_Target
{
	///////////////////////
	// Normal Map Extraction

	// Will use the model normal/tangent to calculate matrix for tangent space. The normals for each pixel are *interpolated* from the
	// vertex normals/tangents. This means they will not be length 1, so they need to be renormalised (same as per-pixel lighting issue)
	float3 modelNormal = normalize( vOut.ModelNormal );
	float3 modelTangent = normalize( vOut.ModelTangent );

	// Calculate bi-tangent to complete the three axes of tangent space - then create the *inverse* tangent matrix to convert *from*
	// tangent space into model space
	float3 modelBiTangent = cross( modelNormal, modelTangent );
	float3x3 invTangentMatrix = float3x3(modelTangent, modelBiTangent, modelNormal);

	/// Parallax Mapping Extra ///

	// Get normalised vector to camera for parallax mapping and specular equation (this vector was calculated later in previous shaders)
	float3 CameraDir = normalize(CameraPos - vOut.WorldPos.xyz);

	// Transform camera vector from world into model space. Need *inverse* world matrix for this.
	// Only need 3x3 matrix to transform vectors, to invert a 3x3 matrix we transpose it (flip it about its diagonal)
	float3x3 invWorldMatrix = transpose( WorldMatrix );
	float3 cameraModelDir = normalize( mul( CameraDir, invWorldMatrix ) ); // Normalise in case world matrix is scaled

	// Then transform model-space camera vector into tangent space (texture coordinate space) to give the direction to offset texture
	// coordinate, only interested in x and y components. Calculated inverse tangent matrix above, so invert it back for this step
	float3x3 tangentMatrix = transpose( invTangentMatrix ); 
	float2 textureOffsetDir = mul( cameraModelDir, tangentMatrix );
	
	// Get the depth info from the normal map's alpha channel at the given texture coordinate
	// Rescale from 0->1 range to -x->+x range, x determined by ParallaxDepth setting
	float texDepth = ParallaxDepth * (NormalMap.Sample( TrilinearWrap, vOut.UV ).a - 0.5f);
	
	// Use the depth of the texture to offset the given texture coordinate - this corrected texture coordinate will be used from here on
	float2 offsetTexCoord = vOut.UV + texDepth * textureOffsetDir;

	///////////////////////////////

	// Get the texture normal from the normal map. The r,g,b pixel values actually store x,y,z components of a normal. However, r,g,b
	// values are stored in the range 0->1, whereas the x, y & z components should be in the range -1->1. So some scaling is needed
	float3 textureNormal = 2.0f * NormalMap.Sample( TrilinearWrap, offsetTexCoord ) - 1.0f; // Scale from 0->1 to -1->1

	// Now convert the texture normal into model space using the inverse tangent matrix, and then convert into world space using the world
	// matrix. Normalise, because of the effects of texture filtering and in case the world matrix contains scaling
	float3 worldNormal = normalize( mul( mul( textureNormal, invTangentMatrix ), WorldMatrix ) );


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 cameraDir = normalize(CameraPos - vOut.WorldPos.xyz); // Position of camera - position of current vertex (or pixel) (in world space)
	
	//// LIGHT 1
	float3 light1Dir = normalize(Light1Pos - vOut.WorldPos.xyz);   // Direction for each light is different
	float3 light1Dist = length(Light1Pos - vOut.WorldPos.xyz); 
	float3 diffuseLight1 = Light1Colour * max( dot(worldNormal.xyz, light1Dir), 0 ) / light1Dist;
	float3 halfway = normalize(light1Dir + cameraDir);
	float3 specularLight1 = diffuseLight1 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	//// LIGHT 2
	float3 light2Dir = normalize(Light2Pos - vOut.WorldPos.xyz);
	float3 light2Dist = length(Light2Pos - vOut.WorldPos.xyz);
	float3 diffuseLight2 = Light2Colour * max( dot(worldNormal.xyz, light2Dir), 0 ) / light2Dist;
	halfway = normalize(light2Dir + cameraDir);
	float3 specularLight2 = diffuseLight2 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
	float3 diffuseLight = AmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;


	////////////////////
	// Material colour

	// Combine diffuse material colour with diffuse map
	float4 diffuseTex = DiffuseMap.Sample( TrilinearWrap, offsetTexCoord );
	float4 diffuseMaterial = DiffuseColour * diffuseTex;
	
	// Combine specular material colour with specular map held in diffuse map alpha
	float3 specularMaterial = SpecularColour * diffuseTex.a;

	
	////////////////////
	// Combine colours 
	
	// Combine maps and lighting for final pixel colour
	float4 combinedColour;
	combinedColour.rgb = diffuseMaterial * diffuseLight + specularMaterial * specularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}


//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------

RasterizerState CullNone  // Cull none of the polygons, i.e. show both sides
{
	CullMode = None;
};
RasterizerState CullBack  // Cull back side of polygon - normal behaviour, only show front of polygons
{
	CullMode = Back;
};


DepthStencilState DepthWritesOff // Don't write to the depth buffer - polygons rendered will not obscure other polygons
{
	DepthWriteMask = ZERO;
};
DepthStencilState DepthWritesOn  // Write to the depth buffer - normal behaviour 
{
	DepthWriteMask = ALL;
};
DepthStencilState DisableDepth   // Disable depth buffer entirely
{
	DepthFunc      = ALWAYS;
	DepthWriteMask = ZERO;
};


BlendState NoBlending // Switch off blending - pixels will be opaque
{
    BlendEnable[0] = FALSE;
};

BlendState AlphaBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
};


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

// Techniques are used to render models in our scene. They select a combination of vertex, geometry and pixel shader from those provided above. Can also set states.
// Different material render methods select different techniques

// Diffuse material colour only
technique10 PlainColour
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSTransformOnly() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PSPlainColour() ) );

		// Switch off blending states
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
     }
}


// Texture tinted with diffuse material colour
technique10 TexColour
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSTransformTex() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PSTexColour() ) );

		// Switch off blending states
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
     }
}


// Pixel lighting with diffuse texture
technique10 PixelLit
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSPixelLit() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PSPixelLit() ) );

		// Switch off blending states
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

// Pixel lighting with diffuse texture
technique10 PixelLitTex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSPixelLitTex() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PSPixelLitTex() ) );

		// Switch off blending states
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

// Normal Mapping
technique10 NormalMapping
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSNormalMap() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PSNormalMap() ) );

		// Switch off blending states
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

// Parallax Mapping
technique10 ParallaxMapping
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSNormalMap() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PSParallaxMap() ) );

		// Switch off blending states
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


//**|PPPOLY|****************************************************************************
// Polygon post-processing materials (shaders & techniques)
//**************************************************************************************

// Post-processing shader that tints the scene texture to a given colour
float4 PPTintShader( VS_BASIC_OUTPUT vOut ) : SV_Target
{
	// This shader plots post-processed pixels, here just tinting the scene. As this is a post-process, the scene is already rendered in a texture.
	// We need UV coordinates to access the scene texture and get the pixel in the location that this shader is to tint. The (standard) vertex shader
	// that has occured before this pixel shader output coordinates in projection space, and under DirectX10 and above these coordinates are available
	// to the pixel shader. However,  by this stage they are converted into screen space, which is convenient: the variable vOut.ProjPos is just a screen
	// pixel coordinate, e.g. (224, 120). In fact strictly, it has 0.5 added (224.5,120.5), but that does not concern us here. All we need do to convert
	// this screen coordinate (0->width, 0->height) into a UV coordinate (0->1, 0->1) is divide by the viewport width and height. Those values are 
	// available as variables at the top of this file.
	//
	float2 UVScene = float2(vOut.ProjPos.x / ViewportWidth, vOut.ProjPos.y / ViewportHeight);
	float3 Colour = SceneTexture.Sample(PointClamp, UVScene) * DiffuseColour;
	return float4(Colour, 1.0f);
}


float4 PPGreyscaleShader(VS_BASIC_OUTPUT vOut) : SV_Target
{
	// This shader plots post-processed pixels, here just tinting the scene. As this is a post-process, the scene is already rendered in a texture.
	// We need UV coordinates to access the scene texture and get the pixel in the location that this shader is to tint. The (standard) vertex shader
	// that has occured before this pixel shader output coordinates in projection space, and under DirectX10 and above these coordinates are available
	// to the pixel shader. However,  by this stage they are converted into screen space, which is convenient: the variable vOut.ProjPos is just a screen
	// pixel coordinate, e.g. (224, 120). In fact strictly, it has 0.5 added (224.5,120.5), but that does not concern us here. All we need do to convert
	// this screen coordinate (0->width, 0->height) into a UV coordinate (0->1, 0->1) is divide by the viewport width and height. Those values are 
	// available as variables at the top of this file.
	//
    float2 UVScene = float2(vOut.ProjPos.x / ViewportWidth, vOut.ProjPos.y / ViewportHeight);
    float3 Colour = SceneTexture.Sample(PointClamp, UVScene);
    // averages the value of RGB
    float greyCol = (Colour.r + Colour.g + Colour.b) / 3;

    return float4(greyCol, greyCol, greyCol, 1.0f);
}

float4 PPNegativeShader(VS_BASIC_OUTPUT vOut) : SV_Target
{
    float2 UVScene = float2(vOut.ProjPos.x / ViewportWidth, vOut.ProjPos.y / ViewportHeight);
    float3 Colour = SceneTexture.Sample(PointClamp, UVScene);

    return float4(1.0f - Colour.r, 1.0f - Colour.g, 1.0f - Colour.b, 1.0f);
}

float4 PPHighContrastShader(VS_BASIC_OUTPUT vOut) : SV_Target
{
    float2 UVScene = float2(vOut.ProjPos.x / ViewportWidth, vOut.ProjPos.y / ViewportHeight);
    float3 Colour = SceneTexture.Sample(PointClamp, UVScene);

    float Lum = dot(Colour.rgb, float3(0.299f, 0.587f, 0.114f));
    if (Lum < 0.3f)
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}


// Cut glass post-processing shader. Parallax mapped texture blended with distorted copy of the scene behind.
float4 PPCutGlassShader( VS_NORMALMAP_OUTPUT vOut ) : SV_Target
{
	const float RefractionStrength = 0.2f;

	/////////////////////////
	// Normal Map Extraction

	// Will use the model normal/tangent to calculate matrix for tangent space. The normals for each pixel are *interpolated* from the
	// vertex normals/tangents. This means they will not be length 1, so they need to be renormalised (same as per-pixel lighting issue)
	float3 modelNormal = normalize( vOut.ModelNormal );
	float3 modelTangent = normalize( vOut.ModelTangent );

	// Calculate bi-tangent to complete the three axes of tangent space - then create the *inverse* tangent matrix to convert *from*
	// tangent space into model space
	float3 modelBiTangent = cross( modelNormal, modelTangent );
	float3x3 invTangentMatrix = float3x3(modelTangent, modelBiTangent, modelNormal);

	/// Parallax Mapping Extra ///

	// Get normalised vector to camera for parallax mapping and specular equation (this vector was calculated later in previous shaders)
	float3 CameraDir = normalize(CameraPos - vOut.WorldPos.xyz);

	// Transform camera vector from world into model space. Need *inverse* world matrix for this.
	// Only need 3x3 matrix to transform vectors, to invert a 3x3 matrix we transpose it (flip it about its diagonal)
	float3x3 invWorldMatrix = transpose( WorldMatrix );
	float3 cameraModelDir = normalize( mul( CameraDir, invWorldMatrix ) ); // Normalise in case world matrix is scaled

	// Then transform model-space camera vector into tangent space (texture coordinate space) to give the direction to offset texture
	// coordinate, only interested in x and y components. Calculated inverse tangent matrix above, so invert it back for this step
	float3x3 tangentMatrix = transpose( invTangentMatrix ); 
	float2 textureOffsetDir = mul( cameraModelDir, tangentMatrix );
	
	// Get the depth info from the normal map's alpha channel at the given texture coordinate
	// Rescale from 0->1 range to -x->+x range, x determined by ParallaxDepth setting
	float texDepth = ParallaxDepth * (NormalMap.Sample( TrilinearWrap, vOut.UV ).a - 0.5f);
	
	// Use the depth of the texture to offset the given texture coordinate - this corrected texture coordinate will be used from here on
	float2 offsetTexCoord = vOut.UV + texDepth * textureOffsetDir;

	///////////////////////////////

	// Get the texture normal from the normal map. The r,g,b pixel values actually store x,y,z components of a normal. However, r,g,b
	// values are stored in the range 0->1, whereas the x, y & z components should be in the range -1->1. So some scaling is needed
	float3 textureNormal = 2.0f * NormalMap.Sample( TrilinearWrap, offsetTexCoord ) - 1.0f; // Scale from 0->1 to -1->1

	// Now convert the texture normal into model space using the inverse tangent matrix, and then convert into world space using the world
	// matrix. Normalise, because of the effects of texture filtering and in case the world matrix contains scaling
	float3 worldNormal = normalize( mul( mul( textureNormal, invTangentMatrix ), WorldMatrix ) );


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 cameraDir = normalize(CameraPos - vOut.WorldPos.xyz); // Position of camera - position of current vertex (or pixel) (in world space)
	
	//// LIGHT 1
	float3 light1Dir = normalize(Light1Pos - vOut.WorldPos.xyz);   // Direction for each light is different
	float3 light1Dist = length(Light1Pos - vOut.WorldPos.xyz); 
	float3 diffuseLight1 = Light1Colour * max( dot(worldNormal.xyz, light1Dir), 0 ) / light1Dist;
	float3 halfway = normalize(light1Dir + cameraDir);
	float3 specularLight1 = diffuseLight1 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	//// LIGHT 2
	float3 light2Dir = normalize(Light2Pos - vOut.WorldPos.xyz);
	float3 light2Dist = length(Light2Pos - vOut.WorldPos.xyz);
	float3 diffuseLight2 = Light2Colour * max( dot(worldNormal.xyz, light2Dir), 0 ) / light2Dist;
	halfway = normalize(light2Dir + cameraDir);
	float3 specularLight2 = diffuseLight2 * pow( max( dot(worldNormal.xyz, halfway), 0 ), SpecularPower );

	// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
	float3 diffuseLight = AmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;



	//***********************************
	// Get scene texture colour (with distortion) to blend with the cut glass texture

	//***TODO - Fill in the float2 UVScene with the UV coordinates of this pixel (same as start part of Tint code you filled in above)
    float2 sceneUV = float2(vOut.ProjPos.x / ViewportWidth, vOut.ProjPos.y / ViewportHeight); 

	// Offset UVs in scene texture to emulate refraction. Transform surface normal (from normal mapping) into camera space,
	// and use this as a direction to offset UV calculated above. This is just a simple effect - not correct refraction
	// Effect is reduced based on z distance
	float3 cameraNormal = mul( worldNormal, ViewMatrix );
	float2 offsetSceneCoord  = sceneUV - RefractionStrength * cameraNormal.xy * (1 - vOut.ProjPos.z);

	// Sample scene texture in offset position to distort the scene visible through the material
	float4 sceneColour = SceneTexture.Sample( PointClamp, offsetSceneCoord );

	//***********************************


	////////////////////
	// Material colour

	// Combine diffuse material colour with diffuse map
	float4 diffuseTex = DiffuseMap.Sample( TrilinearWrap, offsetTexCoord );
	float4 diffuseMaterial = DiffuseColour * diffuseTex;
	
	// Combine specular material colour with specular map held in diffuse map alpha
	float3 specularMaterial = SpecularColour * diffuseTex.a;

	
	////////////////////
	// Combine colours 

	// Combine various textures (scene, diffuse and specular) together with lighting to get final pixel colour
	float3 ppColour = lerp( diffuseMaterial, sceneColour, diffuseTex.a ) * diffuseLight + specularMaterial * specularLight;

	return float4(ppColour, 1.0f);
}


// Polygon post-processing - tint effect
technique10 PPTintPoly
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSTransformOnly() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPTintShader() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOff, 0 );
     }
}


// Polygon post-processing - cut glass effect
technique10 PPCutGlassPoly
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSNormalMap() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, PPCutGlassShader() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOff, 0 );
     }
}

// Polygon post-processing - greyscale effect
technique10 PPGreyscalePoly
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VSNormalMap()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPGreyscaleShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullBack);
        SetDepthStencilState(DepthWritesOff, 0);
    }
}

technique10 PPNegativePoly
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VSNormalMap()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPNegativeShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullBack);
        SetDepthStencilState(DepthWritesOff, 0);
    }
}

technique10 PPContrastPoly
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VSNormalMap()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPHighContrastShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullBack);
        SetDepthStencilState(DepthWritesOff, 0);
    }
}
//**************************************************************************************
