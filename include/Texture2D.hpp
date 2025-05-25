#pragma once

/// <summary>
/// Holds a texture resource and a shader resource view to the resource
/// </summary>
class Texture2D final {
public:

    /// <summary>
    /// Exposes the texture resource
    /// </summary>
    [[nodiscard]] auto Resource() noexcept -> ComPtr<ID3D11Resource>&;

    /// <summary>
    /// Exposes the shader resource view
    /// </summary>
    [[nodiscard]] auto ResourceView() noexcept -> ComPtr<ID3D11ShaderResourceView>&;

private:
    ComPtr<ID3D11Resource> m_TextureResource;
    ComPtr<ID3D11ShaderResourceView> m_SRV;
};