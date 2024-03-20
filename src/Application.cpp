#include <pch.hpp>
#include <Application.hpp>
#include <Piece.hpp>
#include <Board.hpp>

constexpr uint32_t Width = 1920u;
constexpr uint32_t Height = 1080u;

int Application::Run(HINSTANCE hInstance) {
    // Initilaize COM Library
    DirectX::ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    // Mouse singleton
    const auto mouse = std::make_unique<Mouse>();

    // Create window
    CreateWindowClass(hInstance);
    const HWND hWindow = InitWindow(hInstance, L"Chess");

    // Iniltialize renderer
    InitGraphicsDevice(hWindow);
    CreateFrameResources();
    CreateBuffers();
    InitDrawingState();
    CompileShaders();
    LoadPieceTextures();

    // Set board state to starting position
    Board::SetState("1n2q3/1PBPpbK1/1N1pR1N1/2p3pP/rpPP2Bn/p6P/4pQPb/1k6 w - - 0 1");

    ShowWindow(hWindow, SW_SHOW);

    // Application loop
    MSG message;
    while(PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    // Shut down
    DestroyWindow(hWindow);
    UnregisterClass(L"Main", hInstance);
    return 0;
}

void Application::LoadPieceTextures() {
    s_PieceTextures.insert({ PieceFlag::Pawn | PieceFlag::Black, CreateTexture(L"textures/bPawn.png") });
    s_PieceTextures.insert({ PieceFlag::Rook | PieceFlag::Black, CreateTexture(L"textures/bRook.png") });
    s_PieceTextures.insert({ PieceFlag::Knight | PieceFlag::Black, CreateTexture(L"textures/bKnight.png") });
    s_PieceTextures.insert({ PieceFlag::Bishop | PieceFlag::Black, CreateTexture(L"textures/bBishop.png") });
    s_PieceTextures.insert({ PieceFlag::King | PieceFlag::Black, CreateTexture(L"textures/bKing.png") });
    s_PieceTextures.insert({ PieceFlag::Queen | PieceFlag::Black, CreateTexture(L"textures/bQueen.png") });
    s_PieceTextures.insert({ PieceFlag::Pawn | PieceFlag::White, CreateTexture(L"textures/wPawn.png") });
    s_PieceTextures.insert({ PieceFlag::Rook | PieceFlag::White, CreateTexture(L"textures/wRook.png") });
    s_PieceTextures.insert({ PieceFlag::Knight | PieceFlag::White, CreateTexture(L"textures/wKnight.png") });
    s_PieceTextures.insert({ PieceFlag::Bishop | PieceFlag::White, CreateTexture(L"textures/wBishop.png") });
    s_PieceTextures.insert({ PieceFlag::King | PieceFlag::White, CreateTexture(L"textures/wKing.png") });
    s_PieceTextures.insert({ PieceFlag::Queen | PieceFlag::White, CreateTexture(L"textures/wQueen.png") });
}

Texture2D Application::CreateTexture(const std::wstring& filename) {
    Texture2D tex;
    // DirectXTK WICLoader
    DirectX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(s_Device.Get(), s_DeviceContext.Get(),
        filename.c_str(), &tex.Resource(), &tex.ResourceView()));
    return tex;
}

void Application::InitGraphicsDevice(const HWND hWnd) {
    // Create dxgi factory
    DirectX::ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&s_Factory)));
    DirectX::ThrowIfFailed(s_Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&s_Adapter)));

    // Create device, context and swapchain
    const DXGI_SWAP_CHAIN_DESC scDesc {
        {
            Width, Height, { 0, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED
        },
        { 1, 0 },
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        1, hWnd, TRUE,
        DXGI_SWAP_EFFECT_DISCARD,
        0
    };

    constexpr D3D_FEATURE_LEVEL FeatureLevels[2] { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

    ComPtr<ID3D11Device> t_Device;
    ComPtr<ID3D11DeviceContext> t_DeviceContext;
    ComPtr<IDXGISwapChain> t_SwapChain;

    DirectX::ThrowIfFailed(D3D11CreateDeviceAndSwapChain(s_Adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT, FeatureLevels, 2,
        D3D11_SDK_VERSION, &scDesc, &t_SwapChain, &t_Device, nullptr, &t_DeviceContext));

    DirectX::ThrowIfFailed(t_Device.As(&s_Device));
    DirectX::ThrowIfFailed(t_DeviceContext.As(&s_DeviceContext));
    DirectX::ThrowIfFailed(t_SwapChain.As(&s_SwapChain));
}

void Application::CreateFrameResources() {
    // Create render target view
    ComPtr<ID3D11Texture2D1> BackbufferTex;
    DirectX::ThrowIfFailed(s_SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackbufferTex)));
    DirectX::ThrowIfFailed(s_Device->CreateRenderTargetView(BackbufferTex.Get(), nullptr, &s_RTV));

    // Create depth stencil texture and depth stencil view
    constexpr D3D11_TEXTURE2D_DESC1 depthTextureDesc {
        Width, Height, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT,
        { 1, 0 }, D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL,
        0, 0, D3D11_TEXTURE_LAYOUT_UNDEFINED
    };

    DirectX::ThrowIfFailed(s_Device->CreateTexture2D1(&depthTextureDesc, nullptr, &s_DepthTexture));

    constexpr D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc {
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        D3D11_DSV_DIMENSION_TEXTURE2D
    };

    DirectX::ThrowIfFailed(s_Device->CreateDepthStencilView(s_DepthTexture.Get(), &depthStencilViewDesc, &s_DSV));
    s_DeviceContext->OMSetRenderTargets(1, s_RTV.GetAddressOf(), s_DSV.Get());

    // Set viewport
    constexpr D3D11_VIEWPORT viewport { 0, 0, Width, Height, 0, 1 };
    s_DeviceContext->RSSetViewports(1, &viewport);

    // Create linear sampler for piece texture sampling
    constexpr D3D11_SAMPLER_DESC samplerDesc {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP, 0, 1, D3D11_COMPARISON_ALWAYS,
        1, 1, 1, 1, 0, 0
    };

    DirectX::ThrowIfFailed(s_Device->CreateSamplerState(&samplerDesc, &s_SamplerState));
    s_DeviceContext->PSSetSamplers(0, 1, s_SamplerState.GetAddressOf());
}

void Application::CreateBuffers() {
    // Primitive topology triangles
    s_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create and bind vertex buffer
    constexpr D3D11_BUFFER_DESC vertexBufferDesc {
        sizeof(quad), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0
    };
    constexpr D3D11_SUBRESOURCE_DATA vertexData { quad, 0, 0 };
    DirectX::ThrowIfFailed(s_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &s_VertexBuffer));
    constexpr unsigned stride = sizeof(Vertex);
    constexpr unsigned offset = 0;
    s_DeviceContext->IASetVertexBuffers(0, 1, s_VertexBuffer.GetAddressOf(), &stride, &offset);

    // Create and bind index buffer
    constexpr D3D11_BUFFER_DESC indexBufferDesc {
        sizeof(indices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER,
        0, 0, 0
    };
    constexpr D3D11_SUBRESOURCE_DATA indexData { indices, 0, 0 };
    DirectX::ThrowIfFailed(s_Device->CreateBuffer(&indexBufferDesc, &indexData, &s_IndexBuffer));
    s_DeviceContext->IASetIndexBuffer(s_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Create constant buffer
    constexpr D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(ConstantBufferData), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, 0
    };
    DirectX::ThrowIfFailed(s_Device->CreateBuffer(&constantBufferDesc, nullptr, &s_ConstantBuffer));
}

void Application::InitDrawingState() {
    // Create and set input layout
    ComPtr<ID3D11InputLayout> InputLayout;
    constexpr D3D11_INPUT_ELEMENT_DESC polygonLayout[2] { {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            0, D3D11_INPUT_PER_VERTEX_DATA, 0
        }, {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0
        }
    };

    ComPtr<ID3DBlob> inputShaderBlob;
    DirectX::ThrowIfFailed(D3DCompileFromFile(L"board.hlsl", nullptr, nullptr, "vert",
        "vs_5_0", 0, 0, &inputShaderBlob, nullptr));
    DirectX::ThrowIfFailed(s_Device->CreateInputLayout(polygonLayout, 2,
        inputShaderBlob->GetBufferPointer(), inputShaderBlob->GetBufferSize(), &InputLayout));
    s_DeviceContext->IASetInputLayout(InputLayout.Get());

    // Create depth stencil states
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc {
        true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS,
        false, 0xFF, 0xFF,
        { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR,
            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS
        }, { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR,
            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS
        }
    };
    DirectX::ThrowIfFailed(s_Device->CreateDepthStencilState(&depthStencilDesc, &s_DepthStencilStateBoard));
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    DirectX::ThrowIfFailed(s_Device->CreateDepthStencilState(&depthStencilDesc, &s_DepthStencilStatePiece));

    // Create and set rasterizer state
    constexpr D3D11_RASTERIZER_DESC2 rasterDesc {
        D3D11_FILL_SOLID, D3D11_CULL_BACK, true, 0, 0.0f,
        0.0f, true, false, false, false,
        0, D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };
    DirectX::ThrowIfFailed(s_Device->CreateRasterizerState2(&rasterDesc, &s_RasterizerState));
    s_DeviceContext->RSSetState(s_RasterizerState.Get());

    // Create and set blend state
    constexpr D3D11_BLEND_DESC1 blendDesc {
        false, false,
        { {
                true, false, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA,
                D3D11_BLEND_OP_ADD, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA,
                D3D11_BLEND_OP_ADD, D3D11_LOGIC_OP_NOOP, D3D11_COLOR_WRITE_ENABLE_ALL
            }
        }
    };
    DirectX::ThrowIfFailed(s_Device->CreateBlendState1(&blendDesc, &s_BlendState));
    s_DeviceContext->OMSetBlendState(s_BlendState.Get(), nullptr, 0xffffffff);
}

void Application::CompileShaders() {
    // Compile shaders from file into blobs
    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;

    // Board
    DirectX::ThrowIfFailed(D3DCompileFromFile(L"board.hlsl", nullptr, nullptr, "vert",
        "vs_5_0", 0, 0, &vertexShaderBlob, nullptr));
    DirectX::ThrowIfFailed(D3DCompileFromFile(L"board.hlsl", nullptr, nullptr, "frag",
        "ps_5_0", 0, 0, &pixelShaderBlob, nullptr));
    DirectX::ThrowIfFailed(s_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(), nullptr, &s_BoardShaderVertex));
    DirectX::ThrowIfFailed(s_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(), nullptr, &s_BoardShaderPixel));

    // Piece
    DirectX::ThrowIfFailed(D3DCompileFromFile(L"piece.hlsl", nullptr, nullptr, "vert",
        "vs_5_0", 0, 0, &vertexShaderBlob, nullptr));
    DirectX::ThrowIfFailed(D3DCompileFromFile(L"piece.hlsl", nullptr, nullptr, "frag",
        "ps_5_0", 0, 0, &pixelShaderBlob, nullptr));
    DirectX::ThrowIfFailed(s_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(), nullptr, &s_PieceShaderVertex));
    DirectX::ThrowIfFailed(s_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(), nullptr, &s_PieceShaderPixel));
}

void Application::DrawChessPiece(const Piece &piece, ConstantBufferData &cbuffer) {
    // Update transform, set texture, and draw
    cbuffer.ModelMatrix = piece.GetModelMatrix();
    UpdateConstantBuffer(cbuffer);
    s_DeviceContext->PSSetShaderResources(0, 1, s_PieceTextures[piece.GetType()].ResourceView().GetAddressOf());
    s_DeviceContext->DrawIndexed(6, 0, 0);
}

void Application::UpdateConstantBuffer(const ConstantBufferData &cbuffer) {
    // Map and unmap constant buffer to set data, and then upload
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    DirectX::ThrowIfFailed(s_DeviceContext->Map(s_ConstantBuffer.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
    std::memcpy(mappedResource.pData, &cbuffer, sizeof(ConstantBufferData));
    s_DeviceContext->Unmap(s_ConstantBuffer.Get(), 0);
    s_DeviceContext->VSSetConstantBuffers(0, 1, s_ConstantBuffer.GetAddressOf());
}

Vector2 Application::ScreenToWorldPoint(const Vector2 &screen, const ConstantBufferData &cbuffer) {
    // Transform into ndc, then into view space, and then into world space
    const auto ndc = (Vector2(screen.x / static_cast<float>(Width), (static_cast<float>(Height) - screen.y) / static_cast<float>(Height)) - Vector2(0.5f, 0.5f)) * 2;
    return Vector2::Transform(Vector2::Transform(ndc, cbuffer.ProjectionMatrix.Invert()), cbuffer.ViewMatrix.Invert());
}

Vector2 Application::ScreenToWorldPoint(const int &sx, const int &sy, const ConstantBufferData &cbuffer) {
    return ScreenToWorldPoint(Vector2(static_cast<float>(sx), static_cast<float>(sy)), cbuffer);
}

void Application::Render() {
    // Initialize cbuffer for current config
    ConstantBufferData cbuffer {
        Matrix::CreateScale(8, 8, 1),
        Matrix::CreateTranslation(4, 4, 0).Invert(),
        Matrix::CreateOrthographic(9.0f * 1920.0f / 1080.0f, 9.0f, -1, 1)
    };

    UpdateConstantBuffer(cbuffer);

    // Update mouse state
    const auto mouseState = Mouse::Get().GetState();
    s_MouseState.Update(mouseState);

    // Handle basic piece dragging
    if(s_MouseState.leftButton == ButtonState::PRESSED) {
        for(auto& piece : Board::GetBoard() | std::views::values) {
            if(piece.PointInside(ScreenToWorldPoint(mouseState.x, mouseState.y, cbuffer))) {
                s_SelectedPiece = &piece;
            }
        }
    }

    if(s_MouseState.leftButton == ButtonState::RELEASED) {
        auto pos = ScreenToWorldPoint(mouseState.x, mouseState.y, cbuffer);
        pos.x = floor(pos.x);
        pos.y = floor(pos.y);
        if(s_SelectedPiece) {
            s_SelectedPiece->SetPosition(pos);
            s_SelectedPiece = nullptr;
        }
    }

    if(s_SelectedPiece) {
        s_SelectedPiece->SetPosition(ScreenToWorldPoint(mouseState.x, mouseState.y, cbuffer) - Vector2(0.5f, 0.5f));
    }

    // Clear RT
    constexpr float color[4] { 0.2f, 0.2f, 0.2f, 1.0f };
    s_DeviceContext->ClearRenderTargetView(s_RTV.Get(), color);
    s_DeviceContext->ClearDepthStencilView(s_DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Draw board
    s_DeviceContext->OMSetDepthStencilState(s_DepthStencilStateBoard.Get(), 1);
    s_DeviceContext->VSSetShader(s_BoardShaderVertex.Get(), nullptr, 0);
    s_DeviceContext->PSSetShader(s_BoardShaderPixel.Get(), nullptr, 0);

    s_DeviceContext->DrawIndexed(6, 0, 0);

    // Draw pieces
    s_DeviceContext->OMSetDepthStencilState(s_DepthStencilStatePiece.Get(), 1);
    s_DeviceContext->VSSetShader(s_PieceShaderVertex.Get(), nullptr, 0);
    s_DeviceContext->PSSetShader(s_PieceShaderPixel.Get(), nullptr, 0);

    for(auto& piece : Board::GetBoard() | std::views::values) {
        DrawChessPiece(piece, cbuffer);
    }

    s_SwapChain->Present(0, 0);
}

HWND Application::InitWindow(const HINSTANCE hInstance, const LPCWSTR windowName) {
    // Take into account the top bar of the window
    RECT wndRect { 0, 0, Width, Height };
    AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);

    const LONG w = wndRect.right - wndRect.left;
    const LONG h = wndRect.bottom - wndRect.top;

    // Get info about the monitor to position the window to the center
    MONITORINFOEX mInfo;
    mInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(MonitorFromPoint({ 0,  0}, MONITOR_DEFAULTTOPRIMARY), &mInfo);

    const LONG hmw = (mInfo.rcMonitor.right - mInfo.rcMonitor.left) / 2;
    const LONG hmh = (mInfo.rcMonitor.bottom - mInfo.rcMonitor.top) / 2;

    // Create the actual window
    return CreateWindowEx(
        0, L"Main", windowName,
        WS_OVERLAPPEDWINDOW,
        hmw - w / 2, hmh - h / 2, w, h,
        nullptr, nullptr, hInstance, nullptr
    );
}

void Application::CreateWindowClass(const HINSTANCE hInstance) {
    // Describe and register window class for main window
    WNDCLASSEX wndClass = {
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW,
        WindowProcedure,
        0, 0, hInstance, LoadIcon(nullptr, IDI_WINLOGO),
        LoadCursor(nullptr, IDC_ARROW), nullptr,
        nullptr, L"Main", nullptr
    };

    RegisterClassEx(&wndClass);
}

LRESULT Application::WindowProcedure(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    // Handle different application messages from the main window
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            Render();
            return 0;

        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_INPUT:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_MOUSEHOVER:
            Mouse::ProcessMessage(uMsg, wParam, lParam);
            return 0;

        case WM_MOUSEACTIVATE:
            // When you click to activate the window, we want Mouse to ignore that event.
            return MA_ACTIVATEANDEAT;

        default: {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
}