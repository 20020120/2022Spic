#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<vector>
#include<wrl.h>
//****************************************************************
// 
// 剣の軌跡クラス 
// 
//****************************************************************

class SwordTrail final
{
   //****************************************************************
   // 
   // 構造体
   // 
   //****************************************************************
    struct TrailData
    {
        DirectX::XMFLOAT3 mTopPoint{}; // 軌跡の頂点
        DirectX::XMFLOAT3 mBottomPoint{};
    };
    struct TrailVertex
    {
        DirectX::XMFLOAT3 mPosition{};  // 位置
        DirectX::XMFLOAT2 mTexCoord{};  // UV
    };

    //****************************************************************
    // 
    //  関数
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
    // 変数 
    // 
    //****************************************************************
private:
    std::vector<TrailData> mDataVec{}; // 軌跡データのコンテナ
    std::vector<TrailVertex> mTrailVertexVec{}; // 頂点データのコンテナ

    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer{ nullptr };
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> mShaderResourceView{ nullptr };
    Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> mTrailNormalSrv{ nullptr };
    D3D11_TEXTURE2D_DESC mTexture2DDesc{};

    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************
    const int mMaxTrailCount = 120;
};
