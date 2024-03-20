#pragma once

/// <summary>
/// Holds a texture resource and a shader resource view to the resource
/// </summary>
class Texture2D final {
public:

    /// <summary>
    /// Exposes the texture resource
    /// </summary>
    [[nodiscard]] ComPtr<ID3D11Resource>& Resource() noexcept;

    /// <summary>
    /// Exposes the shader resource view
    /// </summary>
    [[nodiscard]] ComPtr<ID3D11ShaderResourceView>& ResourceView() noexcept;

private:
    ComPtr<ID3D11Resource> m_TextureResource;
    ComPtr<ID3D11ShaderResourceView> m_SRV;
};