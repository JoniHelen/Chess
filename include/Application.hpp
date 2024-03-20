#pragma once

#include <Texture2D.hpp> // TextureMap needs the full implementation of Texture2D

class Piece;
enum class PieceFlag;

using TextureMap = std::unordered_map<PieceFlag, Texture2D>;


/// <summary>
/// A static class that handles the execution of the program
/// </summary>
class Application final {
public:

    /// <summary>
    /// Runs the application
    /// </summary>
    /// <param name="hInstance"><c>HINSTANCE</c> The windows process handle</param>
    /// <returns><c>int</c> Exit code</returns>
    static int Run(HINSTANCE hInstance);

    /// <summary>
    /// A texture collection that can be indexed with <c>PieceFlag</c>
    /// </summary>
    inline static TextureMap s_PieceTextures;

private:

    // Rendering structs

    /// <summary>
    /// A Vertex struct that contains position and uv
    /// </summary>
    struct Vertex final {
        Vector2 position;
        Vector2 uv;
    };

    /// <summary>
    /// A buffer struct that contains matrix data
    /// </summary>
    struct ConstantBufferData final {
        Matrix ModelMatrix;
        Matrix ViewMatrix;
        Matrix ProjectionMatrix;
    };


    /// <summary>
    /// Loads piece textures into a <c>TextureMap</c>
    /// </summary>
    static void LoadPieceTextures();

    /// <summary>
    /// Creates a <c>Texture2D</c> object from a filepath
    /// </summary>
    /// <param name="filename"><c>wstring</c> The path to the file to load</param>
    /// <returns><c>Texture2D</c> The created texture object</returns>
    static Texture2D CreateTexture(const std::wstring& filename);

    /// <summary>
    /// Initializes the graphics device and creates a <c>IDXGISwapChain</c>
    /// </summary>
    /// <param name="hWnd"><c>HWND</c> Window handle for displaying images</param>
    static void InitGraphicsDevice(HWND hWnd);

    /// <summary>
    /// Creates RTVs, DSVs, and samplers
    /// </summary>
    static void CreateFrameResources();

    /// <summary>
    /// Creates Vertex, Index, and Constant buffers
    /// </summary>
    static void CreateBuffers();

    /// <summary>
    /// Creates InputLayouts, DepthStencilStates, and RasterizerStates
    /// </summary>
    static void InitDrawingState();

    /// <summary>
    /// Compiles shaders to shader objects
    /// </summary>
    static void CompileShaders();

    /// <summary>
    /// Draws a single Chess Piece
    /// </summary>
    /// <param name="piece"><c>Piece</c> The Piece to draw</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The current constant buffer</param>
    static void DrawChessPiece(const Piece& piece, ConstantBufferData& cbuffer);

    /// <summary>
    /// Updates constant buffer data to the GPU
    /// </summary>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The data to update</param>
    static void UpdateConstantBuffer(const ConstantBufferData& cbuffer);

    /// <summary>
    /// Transforms a point from screen space into world space
    /// </summary>
    /// <param name="screen"><c>Vector2</c> The position on screen</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The matrix data to use with transforms</param>
    static Vector2 ScreenToWorldPoint(const Vector2& screen, const ConstantBufferData& cbuffer);

    /// <summary>
    /// Transforms a point from screen space into world space
    /// </summary>
    /// <param name="sx"><c>int</c> The x position on screen</param>
    /// <param name="sy"><c>int</c> The y position on screen</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The matrix data to use with transforms</param>
    static Vector2 ScreenToWorldPoint(const int& sx, const int& sy, const ConstantBufferData& cbuffer);

    /// <summary>
    /// Renders the scene to the screen
    /// </summary>
    static void Render();

    /// <summary>
    /// Initializes a window
    /// </summary>
    /// <param name="hInstance"><c>HINSTANCE</c> The windows process handle</param>
    /// <param name="windowName"><c>LPCWSTR</c> The display name of the window</param>
    /// <returns><c>HWND</c> A handle to the window</returns>
    static HWND InitWindow(HINSTANCE hInstance, LPCWSTR windowName);

    /// <summary>
    /// Creates a window class with the name "Main"
    /// </summary>
    /// <param name="hInstance"><c>HINSTANCE</c> The windows process handle</param>
    static void CreateWindowClass(HINSTANCE hInstance);

    /// <summary>
    /// The window procedure callback for WIN32 message handling
    /// </summary>
    /// <param name="hWnd"><c>HWND</c> The handle to the window that receieved the message</param>
    /// <param name="uMsg"><c>UINT</c> The message that was receieved</param>
    /// <param name="wParam"><c>WPARAM</c> Additional data for the message</param>
    /// <param name="lParam"><c>LPARAM</c> Additional data for the message</param>
    /// <returns><c>LRESULT</c> A result code for handling the message</returns>
    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Tracks mouse clicks
    /// </summary>
    inline static Mouse::ButtonStateTracker s_MouseState;

    /// <summary>
    /// Contains vertices for a quad with UV coordinates
    /// </summary>
    static constexpr Vertex quad[4u] {
        { { 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
        { { 0.0f, 1.0f }, { 0.0f, 1.0f } },
    };

    /// <summary>
    /// Contains indices for a quad
    /// </summary>
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
    inline static ComPtr<ID3D11DepthStencilState> s_DepthStencilStateBoard;
    inline static ComPtr<ID3D11DepthStencilState> s_DepthStencilStatePiece;
    inline static ComPtr<ID3D11Buffer> s_VertexBuffer;
    inline static ComPtr<ID3D11Buffer> s_IndexBuffer;
    inline static ComPtr<ID3D11Buffer> s_ConstantBuffer;
    inline static ComPtr<ID3D11RasterizerState2> s_RasterizerState;
    inline static ComPtr<ID3D11BlendState1> s_BlendState;
    inline static ComPtr<ID3D11SamplerState> s_SamplerState;

    inline static Piece* s_SelectedPiece;

    // Shaders
    inline static ComPtr<ID3D11VertexShader> s_BoardShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_BoardShaderPixel;

    inline static ComPtr<ID3D11VertexShader> s_PieceShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_PieceShaderPixel;
};