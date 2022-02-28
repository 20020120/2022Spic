#pragma once
#include"graphics_pipeline.h"
#include"skinned_mesh.h"
#include"constants.h"
//**********************************************************
// 
//  レーザービーム
// 
//****************************************************************
class LaserBeam final
{
    //****************************************************************
    // 
    // 構造体
    // 
    //****************************************************************
    struct Data
    {
        DirectX::XMFLOAT4 mColor;
    };


    //****************************************************************
    // 
    //  関数
    // 
    //****************************************************************
public:
    LaserBeam() = default;
    ~LaserBeam() = default;

    void fInitialize(ID3D11Device* pDevice_, const wchar_t* TextureName_);
    void fUpdate();
    void fRender(GraphicsPipeline& Graphics_);
    void fSetPosition(DirectX::XMFLOAT3 Start_, DirectX::XMFLOAT3 End_);
    void fSetRadius(float Radius_);
    void fSetColor(DirectX::XMFLOAT4 Color_);
    void fSetAlpha(float Alpha_);
private:
    void fCalcTransform(); // 姿勢の情報を算出
    void fGuiMenu();
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    DirectX::XMFLOAT3 mStartPoint{}; // ビームの開始地点
    DirectX::XMFLOAT3 mEndPoint{}; // ビームの開始地点
    float mRadius{};  // 半径
    float mLerpSpeed{}; // 補完の速さ

    DirectX::XMFLOAT4 mOrientation{}; // 回転
    DirectX::XMFLOAT3 mScale{}; // 大きさ
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
    std::unique_ptr<Constants<Data>> mConstantBuffer{ nullptr };
};

