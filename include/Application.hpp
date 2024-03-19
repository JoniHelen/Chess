#pragma once
#include "Piece.hpp"

class Piece;

class Application final {
public:
    static int Run(HINSTANCE hInstance);

private:
    struct Vertex final {
        Vector2 position;
        Vector2 uv;
    };

    struct ConstantBufferData final {
        Matrix ModelMatrix;
        Matrix ViewMatrix;
        Matrix ProjectionMatrix;
    };

    static void LoadPieceTextures();
    static void CreateTexture(const std::wstring& filename, ComPtr<ID3D11Texture2D1>& tex, ComPtr<ID3D11ShaderResourceView>& srv);
    static void InitGraphicsDevice(HWND hWnd);
    static void CreateFrameResources();
    static void CreateBuffers();
    static void InitDrawingState();
    static void CompileShaders();
    static void DrawChessPiece(const Piece& piece, ConstantBufferData& cbuffer);
    static void UpdateConstantBuffer(const ConstantBufferData& cbuffer);
    static Vector2 ScreenToWorldPoint(const Vector2& screen, const ConstantBufferData& cbuffer);
    static Vector2 ScreenToWorldPoint(const int& sx, const int& sy, const ConstantBufferData& cbuffer);
    static void Render();
    static HWND InitWindow(HINSTANCE hInstance, LPCWSTR windowName);
    static void CreateWindowClass(HINSTANCE hInstance);
    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    inline static Mouse::ButtonStateTracker s_MouseState;

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
    inline static ComPtr<IDXGIFactory7> s_Factory;
    inline static ComPtr<IDXGIAdapter4> s_Adapter;
    inline static ComPtr<ID3D11Device5> s_Device;
    inline static ComPtr<ID3D11DeviceContext4> s_DeviceContext;
    inline static ComPtr<IDXGISwapChain4> s_SwapChain;
    inline static ComPtr<ID3D11RenderTargetView> s_RTV;
    inline static ComPtr<ID3D11Texture2D1> s_DepthTexture;
    inline static ComPtr<ID3D11DepthStencilView> s_DSV;
    inline static ComPtr<ID3D11DepthStencilState> s_DepthStencilState;
    inline static ComPtr<ID3D11Buffer> s_VertexBuffer;
    inline static ComPtr<ID3D11Buffer> s_IndexBuffer;
    inline static ComPtr<ID3D11Buffer> s_ConstantBuffer;
    inline static ComPtr<ID3D11RasterizerState2> s_RasterizerState;
    inline static ComPtr<ID3D11BlendState1> s_BlendState;
    inline static ComPtr<ID3D11SamplerState> s_SamplerState;

    // Texture test
    inline static ComPtr<ID3D11Texture2D1> s_QueenTex;
    inline static ComPtr<ID3D11ShaderResourceView> s_QueenSRV;
    inline static Piece s_Piece = Piece(Piece::Type::Queen);
    inline static Piece* s_SelectedPiece;

    // Shaders
    inline static ComPtr<ID3D11VertexShader> s_BoardShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_BoardShaderPixel;

    inline static ComPtr<ID3D11VertexShader> s_PieceShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_PieceShaderPixel;
};