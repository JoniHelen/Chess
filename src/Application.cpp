#include <Application.hpp>
#include <stdexcept>

int Application::Run(HINSTANCE hInstance) {

    if(FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        throw std::runtime_error("CoInit failed");
    }

    const auto wClassName = L"Main";
    CreateWindowClass(hInstance, wClassName);
    const HWND hWindow = InitWindow(hInstance, wClassName, L"Chess");

    InitGraphicsDevice(hWindow);
    CreateFrameResources();
    CreateBuffers();
    InitDrawingState();
    CompileShaders();
    LoadPieceTextures();

    ShowWindow(hWindow, SW_SHOW);
    MSG message;
    while(PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    DestroyWindow(hWindow);
    UnregisterClass(wClassName, hInstance);
    return 0;
}

void Application::LoadPieceTextures() {

    CreateTexture(L"bQueen.png", s_QueenTex, s_QueenSRV);
}

void Application::CreateTexture(const std::wstring& filename, ComPtr<ID3D11Texture2D1>& tex, ComPtr<ID3D11ShaderResourceView>& srv) {
    ComPtr<ID3D11Resource> texResource;
    if(FAILED(DirectX::CreateWICTextureFromFile(s_Device.Get(), s_DeviceContext.Get(), filename.c_str(), &texResource, &srv))) {
        throw std::runtime_error("Failed to read texture file");
    }

    if(FAILED(texResource->QueryInterface(IID_PPV_ARGS(&tex)))) {
        throw std::runtime_error("Failed to fetch resource interface");
    }
}

void Application::InitGraphicsDevice(const HWND hWnd) {
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&s_Factory)))) {
        throw std::runtime_error("Failed to create DXGI Factory.");
    }

    if (FAILED(s_Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&s_Adapter)))) {
        throw std::runtime_error("Failed to create DXGI Adapter.");
    }

    const DXGI_SWAP_CHAIN_DESC scDesc {
        {
            1280, 720, { 0, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM,
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

    if (FAILED(D3D11CreateDeviceAndSwapChain(s_Adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT, FeatureLevels, 2, D3D11_SDK_VERSION, &scDesc, &t_SwapChain, &t_Device, nullptr, &t_DeviceContext))) {
        throw std::runtime_error("D3D11 Device and SwapChain creation failed.");
    }

    if (FAILED(t_Device->QueryInterface(IID_PPV_ARGS(&s_Device)))) {throw std::runtime_error("Device query failed."); }
    if (FAILED(t_DeviceContext->QueryInterface(IID_PPV_ARGS(&s_DeviceContext)))) { throw std::runtime_error("Device context query failed."); }
    if (FAILED(t_SwapChain->QueryInterface(IID_PPV_ARGS(&s_SwapChain)))) { throw std::runtime_error("SwapChain query failed."); }
}

void Application::CreateFrameResources() {
    ComPtr<ID3D11Texture2D1> BackbufferTex;
    if (FAILED(s_SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackbufferTex)))) {
        throw std::runtime_error("Failed to fetch backbuffer.");
    }

    if (FAILED(s_Device->CreateRenderTargetView(BackbufferTex.Get(), nullptr, &s_RTV))) {
        throw std::runtime_error("Failed to create RTV");
    }

    D3D11_TEXTURE2D_DESC1 depthTextureDesc {
        1280, 720, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT,
        { 1, 0 }, D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL,
        0, 0, D3D11_TEXTURE_LAYOUT_UNDEFINED
    };

    if (FAILED(s_Device->CreateTexture2D1(&depthTextureDesc, nullptr, &s_DepthTexture))) {
        throw std::runtime_error("Failed to create depth texture");
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc {
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        D3D11_DSV_DIMENSION_TEXTURE2D
    };

    if (FAILED(s_Device->CreateDepthStencilView(s_DepthTexture.Get(), &depthStencilViewDesc, &s_DSV))) {
        throw std::runtime_error("Failed to create Depth Stencil View");
    }

    ID3D11RenderTargetView* RTVs[] { s_RTV.Get() };

    s_DeviceContext->OMSetRenderTargets(1, RTVs, s_DSV.Get());

    D3D11_VIEWPORT viewport = {
        0, 0, 1280, 720, 0, 1
    };

    s_DeviceContext->RSSetViewports(1, &viewport);

    D3D11_SAMPLER_DESC samplerDesc {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        0, 1, D3D11_COMPARISON_ALWAYS,
        1, 1, 1, 1,
        0, 0
    };

    if(FAILED(s_Device->CreateSamplerState(&samplerDesc, &s_SamplerState))) {
        throw std::runtime_error("Failed to create sampler state");
    }

    ID3D11SamplerState* states[] { s_SamplerState.Get() };

    s_DeviceContext->PSSetSamplers(0, 1, states);
}

void Application::CreateBuffers() {
    D3D11_BUFFER_DESC vertexBufferDesc {
        sizeof(quad),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0
    };

    D3D11_SUBRESOURCE_DATA vertexData {
        quad, 0, 0
    };

    if (FAILED(s_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &s_VertexBuffer))) {
        throw std::runtime_error("Failed to create Vertex Buffer");
    }

    D3D11_BUFFER_DESC indexBufferDesc {
        sizeof(indices),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0, 0, 0
    };

    D3D11_SUBRESOURCE_DATA indexData {
        indices, 0, 0
    };

    if (FAILED(s_Device->CreateBuffer(&indexBufferDesc, &indexData, &s_IndexBuffer))) {
        throw std::runtime_error("Failed to create Index Buffer");
    }

    D3D11_BUFFER_DESC constantBufferDesc = {
        sizeof(ConstantBufferData),
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_CONSTANT_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, 0
    };

    if (FAILED(s_Device->CreateBuffer(&constantBufferDesc, nullptr, &s_ConstantBuffer))) {
        throw std::runtime_error("Failed to create Constant Buffer");
    }

    constexpr unsigned stride = sizeof(Vertex);
    constexpr unsigned offset = 0;
    ID3D11Buffer* vBuffers[] { s_VertexBuffer.Get() };
    s_DeviceContext->IASetVertexBuffers(0, 1, vBuffers, &stride, &offset);
    s_DeviceContext->IASetIndexBuffer(s_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    s_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Application::InitDrawingState() {
    ComPtr<ID3D11InputLayout> InputLayout;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    ComPtr<ID3DBlob> inputShaderBlob;

    if (FAILED(D3DCompileFromFile(L"board.hlsl", nullptr, nullptr, "vert", "vs_5_0", 0, 0, &inputShaderBlob, nullptr))) {
        throw std::runtime_error("Failed to compile vertex shader for input layout");
    }

    if (FAILED(s_Device->CreateInputLayout(polygonLayout, 2, inputShaderBlob->GetBufferPointer(), inputShaderBlob->GetBufferSize(), &InputLayout))) {
        throw std::runtime_error("Failed to create input layout");
    }

    s_DeviceContext->IASetInputLayout(InputLayout.Get());

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    if (FAILED(s_Device->CreateDepthStencilState(&depthStencilDesc, &s_DepthStencilState))) {
        throw std::runtime_error("Failed to create depth stencil state");
    }

    s_DeviceContext->OMSetDepthStencilState(s_DepthStencilState.Get(), 1);

    D3D11_RASTERIZER_DESC2 rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    rasterDesc.ForcedSampleCount = 0;

    if (FAILED(s_Device->CreateRasterizerState2(&rasterDesc, &s_RasterizerState))) {
        throw std::runtime_error("Failed to create raster state");
    }

    s_DeviceContext->RSSetState(s_RasterizerState.Get());
}

void Application::CompileShaders() {
    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;

    // Board
    if (FAILED(D3DCompileFromFile(L"board.hlsl", nullptr, nullptr, "vert", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr))) {
        throw std::runtime_error("Failed to compile vertex shader");
    }

    if (FAILED(D3DCompileFromFile(L"board.hlsl", nullptr, nullptr, "frag", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr))) {
        throw std::runtime_error("Failed to compile pixel shader");
    }

    if (FAILED(s_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &s_BoardShaderVertex))) {
        throw std::runtime_error("Failed to create vertex shader");
    }

    if (FAILED(s_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &s_BoardShaderPixel))) {
        throw std::runtime_error("Failed to create pixel shader");
    }

    // Piece
    if (FAILED(D3DCompileFromFile(L"piece.hlsl", nullptr, nullptr, "vert", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr))) {
        throw std::runtime_error("Failed to compile vertex shader");
    }

    if (FAILED(D3DCompileFromFile(L"piece.hlsl", nullptr, nullptr, "frag", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr))) {
        throw std::runtime_error("Failed to compile pixel shader");
    }

    if (FAILED(s_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &s_PieceShaderVertex))) {
        throw std::runtime_error("Failed to create vertex shader");
    }

    if (FAILED(s_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &s_PieceShaderPixel))) {
        throw std::runtime_error("Failed to create pixel shader");
    }
}

void Application::Render() {
    ConstantBufferData cbuffer {};
    DirectX::XMStoreFloat4x4(&cbuffer.ModelMatrix, DirectX::XMMatrixScaling(8, 8, 1));
    DirectX::XMStoreFloat4x4(&cbuffer.ViewMatrix, DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranslation(4, 4, 0)));
    constexpr float aspect = 1920.0f / 1080.0f;
    DirectX::XMStoreFloat4x4(&cbuffer.ProjectionMatrix, DirectX::XMMatrixOrthographicLH(9.0f * aspect, 9.0f, -1, 1));

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(s_DeviceContext->Map(s_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        throw std::runtime_error("Failed to map constant buffer");
    }

    std::memcpy(mappedResource.pData, &cbuffer, sizeof(ConstantBufferData));

    s_DeviceContext->Unmap(s_ConstantBuffer.Get(), 0);

    ID3D11Buffer* constBuffers[] { s_ConstantBuffer.Get() };

    s_DeviceContext->VSSetConstantBuffers(0, 1, constBuffers);

    constexpr float color[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    s_DeviceContext->ClearRenderTargetView(s_RTV.Get(), color);

    s_DeviceContext->ClearDepthStencilView(s_DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    s_DeviceContext->VSSetShader(s_BoardShaderVertex.Get(), nullptr, 0);
    s_DeviceContext->PSSetShader(s_BoardShaderPixel.Get(), nullptr, 0);

    s_DeviceContext->DrawIndexed(6, 0, 0);

    DirectX::XMStoreFloat4x4(&cbuffer.ModelMatrix, DirectX::XMMatrixTranslation(0, 0, -0.01f));

    if (FAILED(s_DeviceContext->Map(s_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
        throw std::runtime_error("Failed to map constant buffer");
    }

    std::memcpy(mappedResource.pData, &cbuffer, sizeof(ConstantBufferData));

    s_DeviceContext->Unmap(s_ConstantBuffer.Get(), 0);

    s_DeviceContext->VSSetConstantBuffers(0, 1, constBuffers);

    s_DeviceContext->VSSetShader(s_PieceShaderVertex.Get(), nullptr, 0);
    s_DeviceContext->PSSetShader(s_PieceShaderPixel.Get(), nullptr, 0);

    ID3D11ShaderResourceView* SRVs[] { s_QueenSRV.Get() };

    s_DeviceContext->PSSetShaderResources(0, 1, SRVs);

    s_DeviceContext->DrawIndexed(6, 0, 0);

    s_SwapChain->Present(0, 0);
}

HWND Application::InitWindow(const HINSTANCE hInstance, const LPCWSTR className, const LPCWSTR windowName) {
    RECT wndRect { 0, 0, 1280, 720 };
    AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);

    const LONG w = wndRect.right - wndRect.left;
    const LONG h = wndRect.bottom - wndRect.top;

    MONITORINFOEX mInfo;
    mInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(MonitorFromPoint({ 0,  0}, MONITOR_DEFAULTTOPRIMARY), &mInfo);

    const LONG hmw = (mInfo.rcMonitor.right - mInfo.rcMonitor.left) / 2;
    const LONG hmh = (mInfo.rcMonitor.bottom - mInfo.rcMonitor.top) / 2;

    return CreateWindowEx(
        0, className, windowName,
        WS_OVERLAPPEDWINDOW,
        hmw - w / 2, hmh - h / 2, w, h,
        nullptr, nullptr, hInstance, nullptr
    );
}

void Application::CreateWindowClass(const HINSTANCE hInstance, const LPCWSTR name) {
    WNDCLASSEX wndClass = {
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW,
        WindowProcedure,
        0, 0, hInstance, LoadIcon(nullptr, IDI_WINLOGO),
        LoadCursor(nullptr, IDC_ARROW), nullptr,
        nullptr, name, nullptr
    };

    RegisterClassEx(&wndClass);
}

LRESULT Application::WindowProcedure(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return TRUE;
        }
        case WM_PAINT: {
            Render();
            return TRUE;
        }
        default: {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
}

// Static members
ComPtr<IDXGIFactory7> Application::s_Factory;
ComPtr<IDXGIAdapter4> Application::s_Adapter;
ComPtr<ID3D11Device5> Application::s_Device;
ComPtr<ID3D11DeviceContext4> Application::s_DeviceContext;
ComPtr<IDXGISwapChain4> Application::s_SwapChain;
ComPtr<ID3D11RenderTargetView> Application::s_RTV;
ComPtr<ID3D11Texture2D1> Application::s_DepthTexture;
ComPtr<ID3D11DepthStencilView> Application::s_DSV;
ComPtr<ID3D11DepthStencilState> Application::s_DepthStencilState;
ComPtr<ID3D11Buffer> Application::s_VertexBuffer;
ComPtr<ID3D11Buffer> Application::s_IndexBuffer;
ComPtr<ID3D11Buffer> Application::s_ConstantBuffer;
ComPtr<ID3D11RasterizerState2> Application::s_RasterizerState;

ComPtr<ID3D11SamplerState> Application::s_SamplerState;

ComPtr<ID3D11Texture2D1> Application::s_QueenTex;
ComPtr<ID3D11ShaderResourceView> Application::s_QueenSRV;

ComPtr<ID3D11VertexShader> Application::s_BoardShaderVertex;
ComPtr<ID3D11PixelShader> Application::s_BoardShaderPixel;

ComPtr<ID3D11VertexShader> Application::s_PieceShaderVertex;
ComPtr<ID3D11PixelShader> Application::s_PieceShaderPixel;