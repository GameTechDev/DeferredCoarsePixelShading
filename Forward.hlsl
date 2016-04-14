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

#ifndef FORWARD_HLSL
#define FORWARD_HLSL

#include "Rendering.hlsl"

//--------------------------------------------------------------------------------------
float4 ForwardPS(GeometryVSOut input) : SV_Target
{
    // How many total lights?
    uint totalLights, dummy;
    gLight.GetDimensions(totalLights, dummy);

    float3 lit = float3(0.0f, 0.0f, 0.0f);

    [branch] if (mUI.visualizeLightCount) {
        lit = (float(totalLights) * rcp(255.0f)).xxx;
    } else {
        SurfaceData surface = ComputeSurfaceDataFromGeometry(input);
        for (uint lightIndex = 0; lightIndex < totalLights; ++lightIndex) {
            PointLight light = gLight[lightIndex];
            AccumulateBRDF(surface, light, lit);
        }
    }

    return float4(lit, 1.0f);
}

float4 ForwardAlphaTestPS(GeometryVSOut input) : SV_Target
{
    // Always use face normal for alpha tested stuff since it's double-sided
    input.normal = ComputeFaceNormal(input.positionView);

    // Alpha test: dead code and CSE will take care of the duplication here
    SurfaceData surface = ComputeSurfaceDataFromGeometry(input);
    clip(surface.albedo.a - 0.3f);

    // Otherwise run the normal shader
    return ForwardPS(input);
}

// Does ONLY alpha test, not color. Useful for pre-z pass
void ForwardAlphaTestOnlyPS(GeometryVSOut input)
{
    // Alpha test: dead code and CSE will take care of the duplication here
    SurfaceData surface = ComputeSurfaceDataFromGeometry(input);
    clip(surface.albedo.a - 0.3f);
}

#endif // FORWARD_HLSL
