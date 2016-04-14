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

#pragma once

// NOTE: Must match shader equivalent structure
__declspec(align(16))
struct UIConstants
{
    unsigned int lightingOnly;
    unsigned int faceNormals;
    unsigned int visualizeLightCount;
    unsigned int lightsPerPass;
    unsigned int lightCullTechnique;
};

// NOTE: Must match shader equivalent structure
struct PointLight
{
    D3DXVECTOR3 positionView;
    float attenuationBegin;
    D3DXVECTOR3 color;
    float attenuationEnd;
};