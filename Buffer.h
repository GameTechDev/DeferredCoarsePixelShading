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

#include <d3d11.h>
#include <vector>

// NOTE: Ensure that T is exactly the same size/layout as the shader structure!
template <typename T>
class StructuredBuffer
{
public:
    // Construct a structured buffer
    StructuredBuffer(ID3D11Device* d3dDevice, int elements,
                     UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
                     bool dynamic = false);
    
    ~StructuredBuffer();

    ID3D11Buffer* GetBuffer() { return mBuffer; }
    ID3D11UnorderedAccessView* GetUnorderedAccess() { return mUnorderedAccess; }
    ID3D11ShaderResourceView* GetShaderResource() { return mShaderResource; }

    // Only valid for dynamic buffers
    // TODO: Support NOOVERWRITE ring buffer?
    T* MapDiscard(ID3D11DeviceContext* d3dDeviceContext);
    void Unmap(ID3D11DeviceContext* d3dDeviceContext);

private:
    // Not implemented
    StructuredBuffer(const StructuredBuffer&);
    StructuredBuffer& operator=(const StructuredBuffer&);

    int mElements;
    ID3D11Buffer* mBuffer;
    ID3D11ShaderResourceView* mShaderResource;
    ID3D11UnorderedAccessView* mUnorderedAccess;
};


template <typename T>
StructuredBuffer<T>::StructuredBuffer(ID3D11Device* d3dDevice, int elements,
                                      UINT bindFlags, bool dynamic)
    : mElements(elements), mShaderResource(0), mUnorderedAccess(0)
{
    CD3D11_BUFFER_DESC desc(sizeof(T) * elements, bindFlags,
        dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
        dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
        D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
        sizeof(T));

	d3dDevice->CreateBuffer(&desc, 0, &mBuffer);

    if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
        d3dDevice->CreateUnorderedAccessView(mBuffer, 0, &mUnorderedAccess);
    }

    if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
        d3dDevice->CreateShaderResourceView(mBuffer, 0, &mShaderResource);
    }
}


template <typename T>
StructuredBuffer<T>::~StructuredBuffer()
{
    if (mUnorderedAccess) mUnorderedAccess->Release();
    if (mShaderResource) mShaderResource->Release();
    mBuffer->Release();
}


template <typename T>
T* StructuredBuffer<T>::MapDiscard(ID3D11DeviceContext* d3dDeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    d3dDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    return static_cast<T*>(mappedResource.pData);
}


template <typename T>
void StructuredBuffer<T>::Unmap(ID3D11DeviceContext* d3dDeviceContext)
{
    d3dDeviceContext->Unmap(mBuffer, 0);
}


// TODO: Constant buffers
