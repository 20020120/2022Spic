#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<vector>
#include<wrl.h>
//****************************************************************
// 
// ���̋O�ՃN���X 
// 
//****************************************************************

class SwordTrail final
{
   //****************************************************************
   // 
   // �\����
   // 
   //****************************************************************
    struct TrailData
    {
        DirectX::XMFLOAT3 mTopPoint{}; // �O�Ղ̒��_
        DirectX::XMFLOAT3 mBottomPoint{};
    };
    struct TrailVertex
    {
        DirectX::XMFLOAT3 mPosition{};  // �ʒu
        DirectX::XMFLOAT2 mTexCoord{};  // UV
    };

    //****************************************************************
    // 
    //  �֐�
    // 
    //****************************************************************
public:
    SwordTrail() = default;
    ~SwordTrail() = default;

    void fInitialize(ID3D11Device* pDevice_, const wchar_t* FileName_);
    void fUpdate(float elapsedTime_);
    void fRender(ID3D11DeviceContext* pDeviceContext_);

    void fAddTrailPoint(DirectX::XMFLOAT3 Top_, DirectX::XMFLOAT3 Bottom_);
    void fEraseTrailPoint();

    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
private:
    std::vector<TrailData> mDataVec{}; // �O�Ճf�[�^�̃R���e�i
    std::vector<TrailVertex> mTrailVertexVec{}; // ���_�f�[�^�̃R���e�i

    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer{ nullptr };
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> mShaderResourceView{ nullptr };
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> mTrailNormalSrv{ nullptr };
    D3D11_TEXTURE2D_DESC mTexture2DDesc{};

    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const int mMaxTrailCount = 120;
};
