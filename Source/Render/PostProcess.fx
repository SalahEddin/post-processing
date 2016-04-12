//--------------------------------------------------------------------------------------
//	File: PostProcess.fx
//
//	Full screen post processing
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------

RasterizerState CullNone // Cull none of the polygons, i.e. show both sides
{
    CullMode = None;
};

DepthStencilState DepthWritesOff // Don't write to the depth buffer - polygons rendered will not obscure other polygons
{
    DepthWriteMask = ZERO;
};
DepthStencilState DepthWritesOn // Write to the depth buffer - normal behaviour 
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};
DepthStencilState DisableDepth // Disable depth buffer entirely
{
    DepthFunc = ALWAYS;
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
// Global Variables
//--------------------------------------------------------------------------------------

// Texture maps
Texture2D SceneTexture; // Texture containing the scene to copy to the full screen quad
Texture2D PostProcessMap; // Second map for special purpose textures used during post-processing
Texture2D DepthOfFieldTexture;
Texture2D BloomTexture;
Texture2D LumTexture;
// Samplers to use with the above texture maps. Specifies texture filtering and addressing mode to use when accessing texture pixels
// Use point sampling for the scene texture (i.e. no bilinear/trilinear blending) since don't want to blur it in the copy process
SamplerState PointSample
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// Use the usual filtering for the special purpose post-processing textures (e.g. the noise map)
SamplerState TrilinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Other variables used for individual post-processes
float3 TintColour;
float2 NoiseScale;
float2 NoiseOffset;
float DistortLevel;
float BurnLevel;
float Wiggle;
float2 ShockOffset;

float PixelX, PixelY;

int BlurRange;
float BlurStrength;

float4 GaussTemp[16];
float4 GaussianFilter[64];


// Variables for Distance of Field
float DoFDistance;
float DoFRange;
float DoFNear;
float DoFFar;


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

// This vertex shader input uses a special input type, the vertex ID. This value is automatically generated and does not come from a vertex buffer.
// The value starts at 0 and increases by one with each vertex processed. As this is the only input for post processing - **no vertex/index buffers are required**
struct VS_POSTPROCESS_INPUT
{
    uint vertexId : SV_VertexID;
};

// Simple vertex shader output / pixel shader input for the main post processing step
struct PS_POSTPROCESS_INPUT
{
    float4 ProjPos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

// This rather unusual shader generates its own vertices - the input data is merely the vertex ID - an automatically generated increasing index.
// No vertex or index buffer required, so convenient on the C++ side. Probably not so efficient, but fine for a full-screen quad.
PS_POSTPROCESS_INPUT FullScreenQuad(VS_POSTPROCESS_INPUT vIn)
{
    PS_POSTPROCESS_INPUT vOut;

    float4 QuadPos[4] =
    {
        float4(-1.0, 1.0, 0.0, 1.0),
		float4(-1.0, -1.0, 0.0, 1.0),
		float4(1.0, 1.0, 0.0, 1.0),
		float4(1.0, -1.0, 0.0, 1.0)
    };
    float2 QuadUV[4] =
    {
        float2(0.0, 0.0),
		float2(0.0, 1.0),
		float2(1.0, 0.0),
		float2(1.0, 1.0)
    };

    vOut.ProjPos = QuadPos[vIn.vertexId];
    vOut.UV = QuadUV[vIn.vertexId];

    return vOut;
}

//////////////////////////////
// Copy
//////////////

// Post-processing shader that simply outputs the scene texture, i.e. no post-processing. A waste of processing, but illustrative
float4 PPCopyShader(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppColour = SceneTexture.Sample(PointSample, ppIn.UV);
    return float4(ppColour, 1.0f);
}

// Simple copy technique - no post-processing (pointless but illustrative)
technique10 PPCopy
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPCopyShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}

//////////////////////////////
// Tint Effect
//////////////

// Post-processing shader that tints the scene texture to a given colour
float4 PPTintShader(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
	// Sample the texture colour (look at shader above) and multiply it with the tint colour (variables near top)
    float3 ppColour = SceneTexture.Sample(PointSample, ppIn.UV) * TintColour;
    return float4(ppColour, 1.0f);
}

// Tint the scene to a colour
technique10 PPTint
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPTintShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}

//////////////////////////////
// Shockwave effect
////////////

float4 PPShockwaveShader(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppColour = SceneTexture.Sample(PointSample, ppIn.UV + float3(ShockOffset, 0.0));
    return float4(ppColour, 1.0f);
}

technique10 PPShockwave
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPShockwaveShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}

//////////////////////////////
// Two-way Pass Gaussian
///////////////

///////////////
// HORIZONTAL BLUR SHADER
float4 PPBlurShaderH(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
	///////////////
	// Gaussian  blur
	// get the pixels from both sides of thr UV pixel, multiple by its weight and add it to the accumulated colour

    float3 ppCol = float3(0.0f, 0.0f, 0.0f);
    int halfBlurRange = BlurRange / 2;

    float weightTotal = 0;

    for (int i = 0; i < BlurRange; i++)
    {
        float xOffset = (i - halfBlurRange) * PixelX;
        float weight = ((float[4]) (GaussianFilter[i / 4]))[i % 4];
        weightTotal += weight;
        ppCol += SceneTexture.Sample(PointSample, ppIn.UV + float2(xOffset, 0.0f)) * weight;
    }

    ppCol /= weightTotal;
    return float4(ppCol, 0.1f);
}

/////////////////////////////////////////////////////////////////////
// VERTICAL BLUR SHADER
float4 PPBlurShaderV(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppCol = float3(0.0f, 0.0f, 0.0f);
    int halfBlurRange = BlurRange / 2;

    float weightTotal = 0;

    for (int i = 0; i < BlurRange; i++)
    {
        float xOffset = (i - halfBlurRange) * PixelY;
        float weight = ((float[4]) (GaussianFilter[i / 4]))[i % 4];
        weightTotal += weight;
        ppCol += SceneTexture.Sample(PointSample, ppIn.UV + float2(0.0f, xOffset)) * weight;
    }

    ppCol /= weightTotal;
    return float4(ppCol, 0.1f);
}

technique10 PPGaussian
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPBlurShaderH()));

        SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPBlurShaderV()));

        SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}

//////////////////////////////
// Depth of Field
///////////////
SamplerState BilinearClamp
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    MaxLOD = 0.0f;
};

struct VS_BASIC_OUTPUT
{
    float4 ProjPos : SV_POSITION; // 2D "projected" position for vertex (required output for vertex shader)
    float2 UV : TEXCOORD0;
};

float4 PixelDepth(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
	// Output the value that would go in the depth buffer to the pixel colour (greyscale)
    float zVal = ppIn.ProjPos.z/ ppIn.ProjPos.w;
    return zVal.xxxx;
}

// Depth of Field Post Process - Objects in the distance are blurred and the ones
// closer to the camera are sharper
float4 PPDepthOfFieldShader(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
	//Get the normal scene texture
    float4 NormalScene = SceneTexture.Sample(PointSample, ppIn.UV);

	//Get the blurred scene texture
    float4 BlurScene = BloomTexture.Sample(PointSample, ppIn.UV);

	//Get the depth of field scene texture
    float DoFScene = DepthOfFieldTexture.Sample(PointSample, ppIn.UV).r;

	//DoFScene = 1 - DoFScene;
    if (DoFScene < 0.0001)
    {
        return BlurScene;
    }
    else
    {
        return NormalScene;
    }

    float sceneZ = (-DoFNear * DoFFar) / (DoFScene - DoFFar);
    float blurFactor = saturate(abs(sceneZ - DoFDistance) / DoFRange);

    return BlurScene;
    return lerp(NormalScene, BlurScene, blurFactor);
}

// Depth of field technique the whole image - vertically
technique10 PPDepthOfField
{
// Rendering a depth map. Only outputs the depth of each pixel
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PixelDepth()));

		// Switch off blending states
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DepthWritesOn, 0);
    }
// blending the depth with blurriness
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPDepthOfFieldShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}


////////////////////////
// Bloom
///////////

float4 PPBright(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    const float lum_threshold = 0.55f;
    float3 ppColour = BloomTexture.Sample(PointSample, ppIn.UV);

	// Luminance calculation
    float luminance = dot(ppColour.rgb, float3(0.299f, 0.587f, 0.114f));
    if (luminance > lum_threshold)
    {
        return float4(ppColour, 1.0f);
    }
    else
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
///////////////
// HORIZONTAL BLOOM SHADER
float4 PPBloomShaderH(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppCol = float3(0.0f, 0.0f, 0.0f);
    int halfBlurRange = BlurRange / 2;

    float weightTotal = 0;

    for (int i = 0; i < BlurRange; i++)
    {
        float xOffset = (i - halfBlurRange) * PixelX;
        float weight = ((float[4]) (GaussianFilter[i / 4]))[i % 4];
        weightTotal += weight;
        ppCol += BloomTexture.Sample(PointSample, ppIn.UV + float2(xOffset, 0.0f)) * weight;
    }

    ppCol /= weightTotal;
    return float4(ppCol, 1.0f);
}
///////////////
// VERTICAL BLOOM SHADER
float4 PPBloomShaderV(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppCol = float3(0.0f, 0.0f, 0.0f);
    int halfBlurRange = BlurRange / 2;

    float weightTotal = 0;

    for (int i = 0; i < BlurRange; i++)
    {
        float xOffset = (i - halfBlurRange) * PixelY;
        float weight = ((float[4]) (GaussianFilter[i / 4]))[i % 4];
        weightTotal += weight;
        ppCol += BloomTexture.Sample(PointSample, ppIn.UV + float2(0.0f, xOffset)) * weight;
    }

    ppCol /= weightTotal;
    return float4(ppCol, 1.0f);
}

float4 PPBloomBlend(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppColour = SceneTexture.Sample(PointSample, ppIn.UV);
    float3 ppBright = BloomTexture.Sample(PointSample, ppIn.UV);
    
    return float4(ppColour + ppBright, 1.0f);

}

technique10 PPBloom
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPBright()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPBloomShaderH()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
    pass P2
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPBloomShaderV()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
    pass P3
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPBloomBlend()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}

/////////////////
// HDR

float4 PPHDRShader(PS_POSTPROCESS_INPUT ppIn) : SV_Target
{
    float3 ppColour = SceneTexture.Sample(PointSample, ppIn.UV);
    return float4(ppColour, 1.0f);

	// Luminance calculation
    float luminance = dot(ppColour.rgb, float3(0.299f, 0.587f, 0.114f));
}


// Depth of field technique the whole image - vertically
technique10 PPHDR
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPHDRShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}

//////////////////
// Lumiance


technique10 PPLumiance
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, FullScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PPHDRShader()));

        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(CullNone);
        SetDepthStencilState(DisableDepth, 0);
    }
}