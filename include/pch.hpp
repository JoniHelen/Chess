#pragma once
#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows Kit
#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

// std
#include <string>
#include <memory>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <ranges>

// DXTK
#include <PlatformHelpers.h> // Not really public, but has ThrowIfFailed
#include <WICTextureLoader.h>
#include <SimpleMath.h>
#include <Mouse.h>

using std::vector;
using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::Mouse;
using ButtonState = Mouse::ButtonStateTracker::ButtonState;