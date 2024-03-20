#include <Texture2D.hpp>

ComPtr<ID3D11Resource>& Texture2D::Resource() noexcept {
    return m_TextureResource;
}

ComPtr<ID3D11ShaderResourceView> & Texture2D::ResourceView() noexcept {
    return m_SRV;
}