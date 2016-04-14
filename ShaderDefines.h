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

#ifndef SHADER_DEFINES_H
#define SHADER_DEFINES_H

#define MAX_LIGHTS_POWER 10
#define MAX_LOOP_COUNT   100
#define MAX_LIGHTS (1<<MAX_LIGHTS_POWER)

// This determines the tile size for light binning and associated tradeoffs
#define COMPUTE_SHADER_TILE_GROUP_DIM 32
#define COMPUTE_SHADER_TILE_GROUP_SIZE (COMPUTE_SHADER_TILE_GROUP_DIM*COMPUTE_SHADER_TILE_GROUP_DIM)

// If enabled, defers scheduling of per-pixel-shaded pixels until after top-left pixel 
// has been shaded across the whole tile. This allows better SIMD packing and scheduling.
#define DEFER_PER_PIXEL 1

#endif
