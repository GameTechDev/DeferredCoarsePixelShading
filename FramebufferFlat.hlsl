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

#ifndef FRAMEBUFFER_FLAT_HLSL
#define FRAMEBUFFER_FLAT_HLSL

// - RGBA 16-bit per component packed into a uint2 per texel
float4 UnpackRGBA16(uint2 e)
{
    return float4(f16tof32(e), f16tof32(e >> 16));
}
uint2 PackRGBA16(float4 c)
{
    return f32tof16(c.rg) | (f32tof16(c.ba) << 16);
}

// Linearize the given 2D address + sample index into our flat framebuffer array
uint GetFramebufferSampleAddress(uint2 coords, uint sampleIndex)
{
    // Major ordering: Row (x), Col (y), MSAA sample
    return (sampleIndex * mFramebufferDimensions.y + coords.y) * mFramebufferDimensions.x + coords.x;
}

#endif // FRAMEBUFFER_FLAT_HLSL
