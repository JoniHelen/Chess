#include <pch.hpp>
#include <Texture2D.hpp>

auto Texture2D::Resource() noexcept -> ComPtr<ID3D11Resource>& {
    return m_TextureResource;
}

auto Texture2D::ResourceView() noexcept -> ComPtr<ID3D11ShaderResourceView>& {
    return m_SRV;
}