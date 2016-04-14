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

#ifndef BASIC_LOOP_HLSL
#define BASIC_LOOP_HLSL

#include "GBuffer.hlsl"

//--------------------------------------------------------------------------------------
float4 BasicLoop(FullScreenTriangleVSOut input, uint sampleIndex)
{
    // How many total lights?
    uint totalLights, dummy;
    gLight.GetDimensions(totalLights, dummy);
    
    float3 lit = float3(0.0f, 0.0f, 0.0f);

    [branch] if (mUI.visualizeLightCount) {
        lit = (float(totalLights) * rcp(255.0f)).xxx;
    } else {
        SurfaceData surface = ComputeSurfaceDataFromGBufferSample(uint2(input.positionViewport.xy), sampleIndex);

        // Avoid shading skybox/background pixels
        if (surface.positionView.z < mCameraNearFar.y) {
            for (uint lightIndex = 0; lightIndex < totalLights; ++lightIndex) {
                PointLight light = gLight[lightIndex];
                AccumulateBRDF(surface, light, lit);
            }
        }
    }

    return float4(lit, 1.0f);
}

float4 BasicLoopPS(FullScreenTriangleVSOut input) : SV_Target
{
    // Shade only sample 0
    return BasicLoop(input, 0);
}

float4 BasicLoopPerSamplePS(FullScreenTriangleVSOut input, uint sampleIndex : SV_SampleIndex) : SV_Target
{
    float4 result;
    if (mUI.visualizePerSampleShading) {
        result = float4(1, 0, 0, 1);
    } else {
        result = BasicLoop(input, sampleIndex);
    }
    return result;
}

#endif // BASIC_LOOP_HLSL
