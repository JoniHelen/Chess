#pragma once

#ifndef UNICODE
    #define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include <WICTextureLoader.h>
#include <memory>

using Microsoft::WRL::ComPtr;

class Application final {
public:
    static int Run(HINSTANCE hInstance);

private:
    static void LoadPieceTextures();
    static void CreateTexture(const std::wstring& filename, ComPtr<ID3D11Texture2D1>& tex, ComPtr<ID3D11ShaderResourceView>& srv);
    static void InitGraphicsDevice(HWND hWnd);
    static void CreateFrameResources();
    static void CreateBuffers();
    static void InitDrawingState();
    static void CompileShaders();
    static void Render();
    static HWND InitWindow(HINSTANCE hInstance, LPCWSTR className, LPCWSTR windowName);
    static void CreateWindowClass(HINSTANCE hInstance, LPCWSTR name);
    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    struct Vertex {
        DirectX::XMFLOAT2 position;
        DirectX::XMFLOAT2 uv;
    };

    struct ConstantBufferData {
        DirectX::XMFLOAT4X4 ModelMatrix;
        DirectX::XMFLOAT4X4 ViewMatrix;
        DirectX::XMFLOAT4X4 ProjectionMatrix;
    };

    static constexpr Vertex quad[4u] {
        { { 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
        { { 0.0f, 1.0f }, { 0.0f, 1.0f } },
    };

    static constexpr uint32_t indices[6u] {
        0, 1, 2, 0, 2, 3
    };

    // Rendering components
    static ComPtr<IDXGIFactory7> s_Factory;
    static ComPtr<IDXGIAdapter4> s_Adapter;
    static ComPtr<ID3D11Device5> s_Device;
    static ComPtr<ID3D11DeviceContext4> s_DeviceContext;
    static ComPtr<IDXGISwapChain4> s_SwapChain;
    static ComPtr<ID3D11RenderTargetView> s_RTV;
    static ComPtr<ID3D11Texture2D1> s_DepthTexture;
    static ComPtr<ID3D11DepthStencilView> s_DSV;
    static ComPtr<ID3D11DepthStencilState> s_DepthStencilState;
    static ComPtr<ID3D11Buffer> s_VertexBuffer;
    static ComPtr<ID3D11Buffer> s_IndexBuffer;
    static ComPtr<ID3D11Buffer> s_ConstantBuffer;
    static ComPtr<ID3D11RasterizerState2> s_RasterizerState;

    static ComPtr<ID3D11SamplerState> s_SamplerState;

    // Texture test
    static ComPtr<ID3D11Texture2D1> s_QueenTex;
    static ComPtr<ID3D11ShaderResourceView> s_QueenSRV;

    // Shaders
    static ComPtr<ID3D11VertexShader> s_BoardShaderVertex;
    static ComPtr<ID3D11PixelShader> s_BoardShaderPixel;

    static ComPtr<ID3D11VertexShader> s_PieceShaderVertex;
    static ComPtr<ID3D11PixelShader> s_PieceShaderPixel;
};