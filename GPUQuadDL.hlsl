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

#ifndef GPU_QUAD_DL_HLSL
#define GPU_QUAD_DL_HLSL

#include "GPUQuad.hlsl"

// This is a "deferred lighting" implementation of GPU quad which reduces the bandwidth
// required during the accumulation passes by accumulating diffuse and (monchromatic) specular
// components only during the lighting phase.

// Monochromatic specular color implemented as suggested by Naty Hoffman at
//   http://www.realtimerendering.com/blog/deferred-lighting-approaches/

Texture2DMS<float4, MSAA_SAMPLES> gDeferredLightingAccumTexture : register(t7);

float RGBToLuminance(float3 color)
{
    return dot(color, float3(0.2126f, 0.7152f, 0.0722f));
}

// Only the pixel shader changes... quad generation is the same
float4 GPUQuadDL(GPUQuadGSOut input, uint sampleIndex)
{    
    float4 result;

    [branch] if (mUI.visualizeLightCount) {
        result = rcp(255.0f).xxxx;
    } else {
        SurfaceData surface = ComputeSurfaceDataFromGBufferSample(uint2(input.positionViewport.xy), sampleIndex);

        // Avoid shading skybox/background pixels
        // NOTE: Compiler doesn't quite seem to move all the unrelated surface computations inside here
        // We could force it to by restructuring the code a bit, but the "all skybox" case isn't useful for
        // our benchmarking anyways.
        float3 litDiffuse = float3(0.0f, 0.0f, 0.0f);
        float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
        if (surface.positionView.z < mCameraNearFar.y) {
            PointLight light = gLight[input.lightIndex.x];
            AccumulateBRDFDiffuseSpecular(surface, light, litDiffuse, litSpecular);
        }

        // Convert to monochromatic specular for accumulation
        float specularLum = RGBToLuminance(litSpecular);
        result = float4(litDiffuse, specularLum);
    }

    return result;
}

float4 GPUQuadDLPS(GPUQuadGSOut input) : SV_Target
{
    // Shade only sample 0
    return GPUQuadDL(input, 0);
}

float4 GPUQuadDLPerSamplePS(GPUQuadGSOut input, uint sampleIndex : SV_SampleIndex) : SV_Target
{
    return GPUQuadDL(input, sampleIndex);
}

// Resolve separate diffuse/specular components stage
float4 GPUQuadDLResolve(FullScreenTriangleVSOut input, uint sampleIndex)
{
    // Read surface data and accumulated light data
    uint2 coords = uint2(input.positionViewport.xy);
    SurfaceData surface = ComputeSurfaceDataFromGBufferSample(coords, sampleIndex);
    float4 accumulated = gDeferredLightingAccumTexture.Load(coords, sampleIndex);

    float3 lit = float3(0.0f, 0.0f, 0.0f);

    [branch] if (mUI.visualizeLightCount) {
        lit = accumulated.xxx;
    } else {
        // Resolve accumulated lighting
        float diffuseLum = RGBToLuminance(accumulated.xyz);

        // Prevent divide by zero
        const float epsilon = 0.000001f;
        lit = surface.albedo.xyz * (accumulated.xyz + surface.specularAmount * accumulated.xyz * (accumulated.w / (diffuseLum + epsilon)));
    }
    
    return float4(lit, 1.0f);
}

float4 GPUQuadDLResolvePS(FullScreenTriangleVSOut input) : SV_Target
{
    // Shade only sample 0
    return GPUQuadDLResolve(input, 0);
}

float4 GPUQuadDLResolvePerSamplePS(FullScreenTriangleVSOut input, uint sampleIndex : SV_SampleIndex) : SV_Target
{
    float4 result;
    if (mUI.visualizePerSampleShading) {
        result = float4(1, 0, 0, 1);
    } else {
        result = GPUQuadDLResolve(input, sampleIndex);
    }
    return result;
}

#endif // GPU_QUAD_DL_HLSL
