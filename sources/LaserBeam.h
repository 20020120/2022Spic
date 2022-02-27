#pragma once
#include"graphics_pipeline.h"
#include"skinned_mesh.h"

//**********************************************************
// 
//  レーザービーム
// 
//****************************************************************
class LaserBeam final
{
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

private:
    void fCalcTransform(); // 姿勢の情報を算出


private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    DirectX::XMFLOAT3 mStartPoint{}; // ビームの開始地点
    DirectX::XMFLOAT3 mEndPoint{}; // ビームの開始地点
    float mRadius{};  // 半径
    DirectX::XMFLOAT4 mOrientation{}; // 回転
    DirectX::XMFLOAT3 mScale{}; // 大きさ
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

};

