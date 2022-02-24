#include"SwordTrail.h"
#include"texture.h"
#include"shader.h"

#include <stdexcept>
#include<memory>
#include "misc.h"
#include "imgui_include.h"

CatmullRomSpline::CatmullRomSpline(const std::vector<DirectX::XMFLOAT3>& data) : segment_count(data.size() - 1)
{
    using namespace DirectX;
    assert(data.size() >= 2);
    control_points.resize(data.size() + 2);
    std::copy(data.begin(), data.end(), control_points.begin() + 1);
    XMStoreFloat3(&control_points.at(0), 2 * XMLoadFloat3(&data.at(0)) - XMLoadFloat3(&data.at(1)));
    XMStoreFloat3(&control_points.at(control_points.size() - 1), 2 * XMLoadFloat3(&data.at(data.size() - 1)) - XMLoadFloat3(&data.at(data.size() - 2)));
}
void CatmullRomSpline::interpolate(std::vector<DirectX::XMFLOAT3>& interpolated_data, size_t steps)
{
    using namespace DirectX;
    for (size_t segment_index = 0; segment_index < segment_count; ++segment_index)
    {
        interpolate(segment_index, interpolated_data, steps);
    }
    interpolated_data.emplace_back(control_points.at(segment_count + 1));
}
void CatmullRomSpline::interpolate(size_t segment, std::vector<DirectX::XMFLOAT3>& interpolated_data, size_t steps)
{
    using namespace DirectX;
    assert(segment < segment_count);
#if 1
    const float power = 0.5f; // Usually power is 0.5f
    XMVECTOR P0 = XMLoadFloat3(&control_points.at(segment + 0));
    XMVECTOR P1 = XMLoadFloat3(&control_points.at(segment + 1));
    XMVECTOR P2 = XMLoadFloat3(&control_points.at(segment + 2));
    XMVECTOR P3 = XMLoadFloat3(&control_points.at(segment + 3));
    XMVECTOR V0 = (P2 - P0) * power;
    XMVECTOR V1 = (P3 - P1) * power;
    for (size_t step = 0; step < steps; ++step)
    {
        const float t = static_cast<float>(step) / steps;

        XMVECTOR P;
        P = t * t * t * (2 * P1 - 2 * P2 + V0 + V1);
        P += t * t * (-3 * P1 + 3 * P2 - 2 * V0 - V1);
        P += t * V0 + P1;

        XMFLOAT3 interpolated_point{};
        XMStoreFloat3(&interpolated_point, P);
        interpolated_data.emplace_back(interpolated_point);
    }
#else
    // https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
    XMVECTOR P0 = XMLoadFloat3(&control_points.at(segment));
    XMVECTOR P1 = XMLoadFloat3(&control_points.at(segment + 1));
    XMVECTOR P2 = XMLoadFloat3(&control_points.at(segment + 2));
    XMVECTOR P3 = XMLoadFloat3(&control_points.at(segment + 3));

    for (size_t step = 0; step < steps; ++step)
    {
        float t = static_cast<float>(step) / steps;
        float alpha = .5f; /* between 0 and 1 */

        XMVECTOR T0 = XMVectorZero();
        XMVECTOR T1 = XMVectorPow(XMVector3LengthSq(P1 - P0), XMLoadFloat(&alpha) * .5f) + T0;
        XMVECTOR T2 = XMVectorPow(XMVector3LengthSq(P2 - P1), XMLoadFloat(&alpha) * .5f) + T1;
        XMVECTOR T3 = XMVectorPow(XMVector3LengthSq(P3 - P2), XMLoadFloat(&alpha) * .5f) + T2;
        XMVECTOR T = XMVectorLerp(T1, T2, t);
        XMVECTOR A1 = (T1 - T) / (T1 - T0) * P0 + (T - T0) / (T1 - T0) * P1;
        XMVECTOR A2 = (T2 - T) / (T2 - T1) * P1 + (T - T1) / (T2 - T1) * P2;
        XMVECTOR A3 = (T3 - T) / (T3 - T2) * P2 + (T - T2) / (T3 - T2) * P3;
        XMVECTOR B1 = (T2 - T) / (T2 - T0) * A1 + (T - T0) / (T2 - T0) * A2;
        XMVECTOR B2 = (T3 - T) / (T3 - T1) * A2 + (T - T1) / (T3 - T1) * A3;
        XMVECTOR C = (T2 - T) / (T2 - T1) * B1 + (T - T1) / (T2 - T1) * B2;

        XMFLOAT3 interpolated_point{};
        XMStoreFloat3(&interpolated_point, C);
        interpolated_data.emplace_back(interpolated_point);
    }
#endif
}

void SwordTrail::fInitialize(ID3D11Device* pDevice_, const wchar_t* FileName_)
{
    HRESULT hr{ S_OK };

    std::unique_ptr<TrailVertex[]> vertices{ std::make_unique<TrailVertex[]>(mMaxTrailCount) };

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(TrailVertex) * mMaxTrailCount);   //�p�ӂ���o�b�t�@�̑傫��
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //�������݌`���̐ݒ�
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


    //���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    const char* cso_name{ "./shaders/SwordTrailVs.cso" };
    hr = create_vs_from_cso(pDevice_, cso_name, mVertexShader.GetAddressOf(), mInputLayout.GetAddressOf(),
        inputElementDesc, _countof(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    const char* cso_ps_name{ "./shaders/SwordTrailPs.cso" };
    hr = create_ps_from_cso(pDevice_, cso_ps_name, mPixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}

void SwordTrail::fUpdate(float elapsedTime_, size_t steps)
{
    mEraseTimer += elapsedTime_;

    fInterpolate(steps);

    mTrailVertexVec.clear();
    // ���̈ʒu�f�[�^���璸�_�𐶐�����
    for (int i = mDataVec.size() - 1; i > 0; i--)
    {
        TrailVertex vertex{};
        // ����
        vertex.mPosition = mDataVec[i].mTopPoint;
        vertex.mTexCoord = { 0.0f,0.0f };
        mTrailVertexVec.emplace_back(vertex);
        // �E��
        vertex.mPosition = mDataVec[i - 1].mBottomPoint;
        vertex.mTexCoord = { 1.0f,1.0f };
        mTrailVertexVec.emplace_back(vertex);
        // ����
        vertex.mPosition = mDataVec[i].mBottomPoint;
        vertex.mTexCoord = { 0.0f,1.0f };
        mTrailVertexVec.emplace_back(vertex);
        // ����
        vertex.mPosition = mDataVec[i].mTopPoint;
        vertex.mTexCoord = { 0.0f,0.0f };
        mTrailVertexVec.emplace_back(vertex);
        //�E��
        vertex.mPosition = mDataVec[i - 1].mTopPoint;
        vertex.mTexCoord = { 1.0f,0.0f };
        mTrailVertexVec.emplace_back(vertex);
        // �E��
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

    //���_�o�b�t�@�̃o�C���h
    UINT stride{ sizeof(TrailVertex) };
    UINT offset{ 0 };
    pDeviceContext_->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

    //�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
    pDeviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
    pDeviceContext_->IASetInputLayout(mInputLayout.Get());

    //�v���~�e�B�u�̕`��
    pDeviceContext_->Draw(static_cast<UINT>(vertex_count), 0);

}

void SwordTrail::fAddTrailPoint(DirectX::XMFLOAT3 Top_, DirectX::XMFLOAT3 Bottom_)
{
    if (mDataVec.size() < mMaxTrailCount / 6)
    {
        mTopPoints.emplace_back(Top_);
        mBottomPoints.emplace_back(Bottom_);
    }
}

void SwordTrail::fEraseTrailPoint()
{
    bool isErase = false;

    // �ő吔�ɂȂ�����O������
    if(mEraseTimer>=mEraseSeparateTime)
    {
        if (mTopPoints.size() > 0)
        {
            mTopPoints.erase(mTopPoints.begin());
        }
        if (mBottomPoints.size() > 0)
        {
            mBottomPoints.erase(mBottomPoints.begin());
        }
        mEraseTimer = 0.0f;
        isErase = true;
    }

    if (!isErase)
    {
        if (mTopPoints.size() >= 40)
        {
            mTopPoints.erase(mTopPoints.begin());
        }
        if (mBottomPoints.size() >= 40)
        {
            mBottomPoints.erase(mBottomPoints.begin());
        }
    }
}

void SwordTrail::fInterpolate(size_t steps)
{
    if (mTopPoints.size() > 1 && mBottomPoints.size() > 1)
    {
        // top�̕⊮
        CatmullRomSpline topCurve(mTopPoints);
        mIinterpolatedTopPoints.clear();
        topCurve.interpolate(mIinterpolatedTopPoints, steps);
        // bottom�̕⊮
        CatmullRomSpline BottomCurve(mBottomPoints);
        mIinterpolatedBottomPoints.clear();
        BottomCurve.interpolate(mIinterpolatedBottomPoints, steps);
        // mDataVec����
        mDataVec.clear();
        for (size_t count = 0; count < mIinterpolatedTopPoints.size(); ++count)
        {
            TrailData data;
            data.mTopPoint = mIinterpolatedTopPoints.at(count);
            data.mBottomPoint = mIinterpolatedBottomPoints.at(count);
            mDataVec.emplace_back(data);
        }
    }
    else
    {
        mDataVec.clear();
    }
}