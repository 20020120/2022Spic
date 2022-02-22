#include"SwordTrail.h"
#include"texture.h"
#include"shader.h"

#include <stdexcept>
#include<memory>
#include "misc.h"

void SwordTrail::fInitialize(ID3D11Device* pDevice_, const wchar_t* FileName_)
{
    HRESULT hr{ S_OK };

    std::unique_ptr<TrailVertex[]> vertices{ std::make_unique<TrailVertex[]>(mMaxTrailCount) };

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(TrailVertex) * mMaxTrailCount);   //用意するバッファの大きさ
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //書き込み形式の設定
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pSysMem = vertices.get();
    subResourceData.SysMemPitch = 0;
    subResourceData.SysMemSlicePitch = 0;

    hr = pDevice_->CreateBuffer(&bufferDesc, &subResourceData, &mVertexBuffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    hr = load_texture_from_file(pDevice_, FileName_, &mShaderResourceView, &mTexture2DDesc);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    //入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //頂点シェーダーオブジェクトの生成
    const char* cso_name{ "./shaders/SwordTrailPs.cso" };
    hr = create_vs_from_cso(pDevice_, cso_name, mVertexShader.GetAddressOf(), mInputLayout.GetAddressOf(),
        inputElementDesc, _countof(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    //ピクセルシェーダーオブジェクトの生成
    const char* cso_ps_name{ "./shaders/SwordTrailVs.cso" };
    hr = create_ps_from_cso(pDevice_, cso_ps_name, mPixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void SwordTrail::fUpdate(float elapsedTime_)
{
    mTrailVertexVec.clear();
    // 剣の位置データから頂点を生成する
    for (int i = mDataVec.size() - 1; i > 0; i--)
    {
        TrailVertex vertex{};
        // 左上
        vertex.mPosition = mDataVec[i].mTopPoint;
        vertex.mTexCoord = { 0.0f,0.0f };
        mTrailVertexVec.emplace_back(vertex);
        // 右下
        vertex.mPosition = mDataVec[i - 1].mBottomPoint;
        vertex.mTexCoord = { 1.0f,1.0f };
        mTrailVertexVec.emplace_back(vertex);
        // 左下
        vertex.mPosition = mDataVec[i].mBottomPoint;
        vertex.mTexCoord = { 0.0f,1.0f };
        mTrailVertexVec.emplace_back(vertex);
        // 左上
        vertex.mPosition = mDataVec[i].mTopPoint;
        vertex.mTexCoord = { 0.0f,0.0f };
        mTrailVertexVec.emplace_back(vertex);
        //右上
        vertex.mPosition = mDataVec[i - 1].mTopPoint;
        vertex.mTexCoord = { 1.0f,0.0f };
        mTrailVertexVec.emplace_back(vertex);
        // 右下
        vertex.mPosition = mDataVec[i - 1].mBottomPoint;
        vertex.mTexCoord = { 1.0f,1.0f };
        mTrailVertexVec.emplace_back(vertex);
    }


}

void SwordTrail::fRender(ID3D11DeviceContext* pDeviceContext_)
{
    if (mTrailVertexVec.size() < 6)
    {
        return;
    }
    pDeviceContext_->VSSetShader(mVertexShader.Get(), nullptr, 0);
    pDeviceContext_->PSSetShader(mPixelShader.Get(), nullptr, 0);
    pDeviceContext_->PSSetShaderResources(0, 1, mShaderResourceView.GetAddressOf());

    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
    hr = pDeviceContext_->Map(mVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    size_t vertex_count = mTrailVertexVec.size();
    _ASSERT_EXPR(mMaxTrailCount >= vertex_count, "Buffer Overflow");

    TrailVertex* data{ reinterpret_cast<TrailVertex*>(mappedSubresource.pData) };
    if (data != nullptr)
    {
        const TrailVertex* p = mTrailVertexVec.data();
        memcpy_s(data, mMaxTrailCount * sizeof(TrailVertex), p, vertex_count * sizeof(TrailVertex));
    }

    pDeviceContext_->Unmap(mVertexBuffer.Get(), 0);

    //頂点バッファのバインド
    UINT stride{ sizeof(TrailVertex) };
    UINT offset{ 0 };
    pDeviceContext_->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

    //プリミティブタイプおよびデータの順序に関する情報のバインド
    pDeviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //入力レイアウトオブジェクトのバインド
    pDeviceContext_->IASetInputLayout(mInputLayout.Get());

    //プリミティブの描画
    pDeviceContext_->Draw(static_cast<UINT>(vertex_count), 0);

}

void SwordTrail::fAddTrailPoint(DirectX::XMFLOAT3 Top_, DirectX::XMFLOAT3 Bottom_)
{
    TrailData data;
    data.mTopPoint = Top_;
    data.mBottomPoint = Bottom_;
    if (mDataVec.size() < 10)
    {
        mDataVec.emplace_back(data);
    }
}

void SwordTrail::fEraseTrailPoint()
{
    mDataVec.erase(mDataVec.begin());
}
