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

class CatmullRomSpline
{
private:
    std::vector<DirectX::XMFLOAT3> control_points;
public:
    CatmullRomSpline(const std::vector<DirectX::XMFLOAT3>& data);
    virtual ~CatmullRomSpline() = default;
    CatmullRomSpline(const CatmullRomSpline&) = delete;
    CatmullRomSpline& operator=(const CatmullRomSpline&) = delete;
    CatmullRomSpline(CatmullRomSpline&&) noexcept = delete;
    CatmullRomSpline& operator=(CatmullRomSpline&&) noexcept = delete;

    const size_t segment_count;
    void interpolate(std::vector<DirectX::XMFLOAT3>& interpolated_data, size_t steps);
    void interpolate(size_t segment, std::vector<DirectX::XMFLOAT3>& interpolated_data, size_t steps);
};


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
    void fUpdate(float elapsedTime_, size_t steps);
    void fRender(ID3D11DeviceContext* pDeviceContext_);

    void fAddTrailPoint(DirectX::XMFLOAT3 Top_, DirectX::XMFLOAT3 Bottom_);
    void fEraseTrailPoint();
private:
    void fInterpolate(size_t steps);

    //****************************************************************
    //
    // 変数
    //
    //****************************************************************
    std::vector<TrailData> mDataVec{}; // 軌跡データのコンテナ
    std::vector<TrailVertex> mTrailVertexVec{}; // 頂点データのコンテナ

    std::vector<DirectX::XMFLOAT3> mTopPoints;
    std::vector<DirectX::XMFLOAT3> mIinterpolatedTopPoints;
    std::vector<DirectX::XMFLOAT3> mBottomPoints;
    std::vector<DirectX::XMFLOAT3> mIinterpolatedBottomPoints;

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
    const int mMaxTrailCount = 1000;
};
