/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

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