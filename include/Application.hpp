#pragma once

#include <Texture2D.hpp>
#include <Move.hpp>

class Piece;
enum class PieceFlag : uint8_t;

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
    static auto Run(HINSTANCE hInstance) -> int;

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
    static auto LoadPieceTextures() -> void;

    /// <summary>
    /// Creates a <c>Texture2D</c> object from a filepath
    /// </summary>
    /// <param name="filename"><c>wstring</c> The path to the file to load</param>
    /// <returns><c>Texture2D</c> The created texture object</returns>
    static auto CreateTexture(std::wstring_view filename) -> Texture2D;

    /// <summary>
    /// Initializes the graphics device and creates a <c>IDXGISwapChain</c>
    /// </summary>
    /// <param name="hWnd"><c>HWND</c> Window handle for displaying images</param>
    static auto InitGraphicsDevice(HWND hWnd) -> void;

    /// <summary>
    /// Creates RTVs and DSVs
    /// </summary>
    static auto CreateFrameResources(uint32_t width, uint32_t height) -> void;

    /// <summary>
    /// Creates Vertex, Index, and Constant buffers
    /// </summary>
    static auto CreateBuffers() -> void;

    /// <summary>
    /// Creates InputLayouts, DepthStencilStates, and RasterizerStates
    /// </summary>
    static auto InitDrawingState() -> void;

    /// <summary>
    /// Compiles shaders to shader objects
    /// </summary>
    static auto CompileShaders() -> void;

    /// <summary>
    /// Draws a single Chess Piece
    /// </summary>
    /// <param name="piece"><c>Piece</c> The Piece to draw</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The current constant buffer</param>
    static auto DrawChessPiece(const Piece& piece, ConstantBufferData& cbuffer) -> void;

    /// <summary>
    /// Draws a single highlight
    /// </summary>
    /// <param name="pos"><c>Vector2</c> The position of the highlight</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The current constant buffer</param>
	static auto DrawHighlight(const Vector2& pos, ConstantBufferData& cbuffer) -> void;

    /// <summary>
    /// Updates constant buffer data to the GPU
    /// </summary>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The data to update</param>
    static auto UpdateConstantBuffer(const ConstantBufferData& cbuffer) -> void;

    /// <summary>
    /// Transforms a point from screen space into world space
    /// </summary>
    /// <param name="screen"><c>Vector2</c> The position on screen</param>
    /// <param name="screenMetrics"><c>Vector2</c> The screen width and height</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The matrix data to use with transforms</param>
    static auto ScreenToWorldPoint(const Vector2& screen, const Vector2& screenMetrics, const ConstantBufferData& cbuffer) -> Vector2;

    /// <summary>
    /// Transforms a point from screen space into world space
    /// </summary>
    /// <param name="sx"><c>int</c> The x position on screen</param>
    /// <param name="sy"><c>int</c> The y position on screen</param>
    /// <param name="smx"><c>int</c> The width of the screen</param>
    /// <param name="smy"><c>int</c> The height of the screen</param>
    /// <param name="cbuffer"><c>ConstantBufferData</c> The matrix data to use with transforms</param>
    static auto ScreenToWorldPoint(int sx, int sy, int smx, int smy, const ConstantBufferData& cbuffer) -> Vector2;

    /// <summary>
    /// Renders the scene to the screen
    /// </summary>
    static auto Render() -> void;

    /// <summary>
    /// Initializes a window
    /// </summary>
    /// <param name="hInstance"><c>HINSTANCE</c> The windows process handle</param>
    /// <param name="windowClass"><c>LPCWSTR</c> The windows window class of the window</param>
    /// <param name="windowName"><c>LPCWSTR</c> The display name of the window</param>
    /// <returns><c>HWND</c> A handle to the window</returns>
    static auto InitWindow(HINSTANCE hInstance, ATOM windowClass, LPCWSTR windowName) -> HWND;

    /// <summary>
    /// Creates a window class with the name "Main"
    /// </summary>
    /// <param name="hInstance"><c>HINSTANCE</c> The windows process handle</param>
    static auto CreateWindowClass(HINSTANCE hInstance) -> ATOM;

    /// <summary>
    /// The window procedure callback for WIN32 message handling
    /// </summary>
    /// <param name="hWnd"><c>HWND</c> The handle to the window that receieved the message</param>
    /// <param name="uMsg"><c>UINT</c> The message that was receieved</param>
    /// <param name="wParam"><c>WPARAM</c> Additional data for the message</param>
    /// <param name="lParam"><c>LPARAM</c> Additional data for the message</param>
    /// <returns><c>LRESULT</c> A result code for handling the message</returns>
    static auto CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT;

    /// <summary>
    /// Tracks mouse clicks
    /// </summary>
    inline static Mouse::ButtonStateTracker s_MouseState;

    /// <summary>
    /// Contains vertices for a quad with UV coordinates
    /// </summary>
    static constexpr std::array<Vertex, 4> quad {
        {
            { { 0.0F, 0.0F }, { 0.0F, 0.0F } },
            { { 1.0F, 0.0F }, { 1.0F, 0.0F } },
            { { 1.0F, 1.0F }, { 1.0F, 1.0F } },
            { { 0.0F, 1.0F }, { 0.0F, 1.0F } },
        }
    };

    /// <summary>
    /// Contains indices for a quad
    /// </summary>
    static constexpr std::array<uint32_t, 6> indices {
        0, 1, 2, 0, 2, 3
    };

    inline static HWND s_Window;

    inline static std::vector<Move> s_Moves;

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
    inline static Vector2 s_PickupPos;

    // Shaders
    inline static ComPtr<ID3D11VertexShader> s_BoardShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_BoardShaderPixel;

    inline static ComPtr<ID3D11VertexShader> s_PieceShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_PieceShaderPixel;

	inline static ComPtr<ID3D11VertexShader> s_HighlightShaderVertex;
    inline static ComPtr<ID3D11PixelShader> s_HighlightShaderPixel;
};