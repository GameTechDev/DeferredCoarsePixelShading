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
#include <d3dx11.h>
#include <assert.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

// Templated shader factory utilities
namespace ShaderFactoryUtil
{
    // Reads file from disk. Doesn't handle memory de-allocation.
    DWORD ReadFileFromDisk(LPCTSTR pFile, void** buffer);

    template <typename T> LPCSTR GetShaderProfileString();
    template <typename T> T* CreateShader(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength);
    template <typename T> T* CreateShader(ID3D11Device* d3dDevice, LPCTSTR objFile, ID3D11ShaderReflection** reflector);

    // Vertex shader
    template <> inline LPCSTR GetShaderProfileString<ID3D11VertexShader>() { return "vs_5_0"; }
    template <> inline 
    ID3D11VertexShader* CreateShader<ID3D11VertexShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
    {
        ID3D11VertexShader *shader = 0;
        HRESULT hr = d3dDevice->CreateVertexShader(shaderBytecode, bytecodeLength, 0, &shader);
        if (FAILED(hr)) {
            // This shouldn't produce errors given proper bytecode, so a simple assert is fine
            assert(false);
        }
        return shader;
    }

    // Geometry shader
    template <> inline LPCSTR GetShaderProfileString<ID3D11GeometryShader>() { return "gs_5_0"; }
    template <> inline 
    ID3D11GeometryShader* CreateShader<ID3D11GeometryShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
    {
        ID3D11GeometryShader *shader = 0;
        HRESULT hr = d3dDevice->CreateGeometryShader(shaderBytecode, bytecodeLength, 0, &shader);
        if (FAILED(hr)) {
            // This shouldn't produce errors given proper bytecode, so a simple assert is fine
            assert(false);
        }
        return shader;
    }

    // Pixel shader
    template <> inline LPCSTR GetShaderProfileString<ID3D11PixelShader>() { return "ps_5_0"; }
    template <> inline 
    ID3D11PixelShader* CreateShader<ID3D11PixelShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
    {
        ID3D11PixelShader *shader = 0;
        HRESULT hr = d3dDevice->CreatePixelShader(shaderBytecode, bytecodeLength, 0, &shader);
        if (FAILED(hr)) {
            // This shouldn't produce errors given proper bytecode, so a simple assert is fine
            assert(false);
        }
        return shader;
    }

    template <> inline 
    ID3D11PixelShader * CreateShader<ID3D11PixelShader>(ID3D11Device* d3dDevice, LPCTSTR objFile, ID3D11ShaderReflection** reflector)
    {
        ID3D11PixelShader *shader = 0;
        void* fileBuffer;
        DWORD fileSize = ReadFileFromDisk(objFile, &fileBuffer);

        // Create shader and reflector
        HRESULT hr;
        hr = d3dDevice->CreatePixelShader(fileBuffer, fileSize, 0, &shader);
        assert(SUCCEEDED(hr));

        if (reflector) {
            D3DReflect(fileBuffer, fileSize, IID_ID3D11ShaderReflection, (void**) reflector);
        }

        free(fileBuffer);
        return shader;
    }



    // Compute shader
    template <> inline LPCSTR GetShaderProfileString<ID3D11ComputeShader>() { return "cs_5_0"; }
    template <> inline 
    ID3D11ComputeShader* CreateShader<ID3D11ComputeShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
    {
        ID3D11ComputeShader *shader = 0;
        HRESULT hr = d3dDevice->CreateComputeShader(shaderBytecode, bytecodeLength, 0, &shader);
        if (FAILED(hr)) {
            // This shouldn't produce errors given proper bytecode, so a simple assert is fine
            assert(false);
        }
        return shader;
    }

    template <> inline 
    ID3D11ComputeShader* CreateShader<ID3D11ComputeShader>(ID3D11Device* d3dDevice, LPCTSTR objFile, ID3D11ShaderReflection** reflector)
    {
        ID3D11ComputeShader *shader = 0;
        void* fileBuffer;
        DWORD fileSize = ReadFileFromDisk(objFile, &fileBuffer);

        // Create shader and reflector
        HRESULT hr;
        hr = d3dDevice->CreateComputeShader(fileBuffer, fileSize, 0, &shader);
        assert(SUCCEEDED(hr));

        if (reflector) {
            D3DReflect(fileBuffer, fileSize, IID_ID3D11ShaderReflection, (void**) reflector);
        }

        free(fileBuffer);
        return shader;
    }
}

// Templated (on shader type) shader wrapper to wrap basic functionality
// TODO: Support optional lazy compile
template <typename T>
class Shader
{
public:
    Shader(ID3D11Device* d3dDevice, LPCTSTR srcFile, LPCSTR functionName, CONST D3D10_SHADER_MACRO *defines = 0)
    {
        // TODO: Support profile selection from the application? Probably not necessary as we don't
        // support down-level hardware at the moment anyways.
        LPCSTR profile = ShaderFactoryUtil::GetShaderProfileString<T>();

        UINT shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

        ID3D10Blob *bytecode = 0;
        ID3D10Blob *errors = 0;
        HRESULT hr = D3DX11CompileFromFile(srcFile, defines, 0, functionName, profile, shaderFlags, 0, 0, &bytecode, &errors, 0);
        
        if (errors) {
            OutputDebugStringA(static_cast<const char *>(errors->GetBufferPointer()));
        }

        if (FAILED(hr)) {
            // TODO: Define exception type and improve this error string, but the ODS will do for now
            throw std::runtime_error("Error compiling shader");
        }

        mShader = ShaderFactoryUtil::CreateShader<T>(d3dDevice, bytecode->GetBufferPointer(), bytecode->GetBufferSize());

        bytecode->Release();
    }


    Shader(ID3D11Device* d3dDevice, ID3D11ShaderReflection** reflector, LPCTSTR objFile)
    {
        mShader = ShaderFactoryUtil::CreateShader<T>(d3dDevice, objFile, reflector);
    }

    ~Shader() { mShader->Release(); }

    T* GetShader() { return mShader; }

private:
    // Not implemented
    Shader(const Shader&);
    Shader& operator=(const Shader&);

    T* mShader;
};


typedef Shader<ID3D11VertexShader> VertexShader;
typedef Shader<ID3D11GeometryShader> GeometryShader;
typedef Shader<ID3D11PixelShader> PixelShader;
typedef Shader<ID3D11ComputeShader> ComputeShader;