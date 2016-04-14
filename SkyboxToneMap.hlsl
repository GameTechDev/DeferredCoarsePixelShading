// Copyright 2010 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#ifndef SKYBOX_TONE_MAP_HLSL
#define SKYBOX_TONE_MAP_HLSL

// Currently need GBuffer for albedo... used in ambient
#include "GBuffer.hlsl"
#include "FramebufferFlat.hlsl"

//--------------------------------------------------------------------------------------
// Tone mapping, post processing, skybox, etc.
// Rendered using skybox geometry, hence the naming
//--------------------------------------------------------------------------------------
TextureCube<float4> gSkyboxTexture : register(t5);
#if MSAA_SAMPLES > 1
Texture2DMS<float, MSAA_SAMPLES> gDepthTexture : register(t6);
#else
Texture2D<float> gDepthTexture : register(t6);
#endif // MSAA_SAMPLES > 1

// This is the regular multisampled lit texture
Texture2DMS<float4, MSAA_SAMPLES> gLitTexture : register(t7);
// Since compute shaders cannot write to multisampled UAVs, this texture is used by
// the CS paths. It stores each sample separately in rows (i.e. y).
StructuredBuffer<uint2> gLitTextureFlat : register(t8);

struct SkyboxVSOut
{
    float4 positionViewport : SV_Position;
    float3 skyboxCoord : skyboxCoord;
};

SkyboxVSOut SkyboxVS(GeometryVSIn input)
{
    SkyboxVSOut output;
    
    // NOTE: Don't translate skybox and make sure depth == 1 (no clipping)
    output.positionViewport = mul(float4(input.position, 0.0f), mCameraViewProj).xyww;
    output.skyboxCoord = input.position;
    
    return output;
}

float4 SkyboxPS(SkyboxVSOut input) : SV_Target0
{
    // Use the flattened MSAA lit buffer if provided
    uint2 dims;
    gLitTextureFlat.GetDimensions(dims.x, dims.y);
    bool useFlatLitBuffer = dims.x > 0;
    
    uint2 coords = input.positionViewport.xy;

    float3 lit = float3(0.0f, 0.0f, 0.0f);
    float skyboxSamples = 0.0f;
    #if MSAA_SAMPLES <= 1
    [unroll]
    #endif
    for (unsigned int sampleIndex = 0; sampleIndex < MSAA_SAMPLES; ++sampleIndex) {
#if MSAA_SAMPLES > 1
        float depth = gDepthTexture.Load(coords, sampleIndex);
#else
        float depth = gDepthTexture[coords];
#endif // MSAA_SAMPLES > 1

        // Check for skybox case (NOTE: complementary Z!)
        if (depth <= 0.0f && !mUI.visualizeLightCount) {
            ++skyboxSamples;
        } else {
            float3 sampleLit;
            [branch] if (useFlatLitBuffer) {
                sampleLit = UnpackRGBA16(gLitTextureFlat[GetFramebufferSampleAddress(coords, sampleIndex)]).xyz;
            } else {
                sampleLit = gLitTexture.Load(coords, sampleIndex).xyz;
            }

            // Tone map each sample separately (identity for now) and accumulate
            lit += sampleLit;
        }
    }

    // If necessary, add skybox contribution
    [branch] if (skyboxSamples > 0) {
        float3 skybox = gSkyboxTexture.Sample(gDiffuseSampler, input.skyboxCoord).xyz;
        // Tone map and accumulate
        lit += skyboxSamples * skybox;
    }

    // Resolve MSAA samples (simple box filter)
    return float4(lit * rcp(MSAA_SAMPLES), 1.0f);
}


#endif // SKYBOX_TONE_MAP_HLSL
