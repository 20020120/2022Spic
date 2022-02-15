#pragma once
#include"skinned_mesh.h"
#include<d3d11.h>
#include<memory>
//****************************************************************
// 
// 敵の基底クラス 
// 
//****************************************************************

struct EnemySource
{
    float mSpawnTimer{};  // 出現タイミングを記録
    int mEmitterNumber{}; // 出現位置の番号
    int mType{};          // 敵の種類
};


class BaseEnemy
{
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
public:
    BaseEnemy(ID3D11Device *pDevice_,const char* ModelName_);
    virtual ~BaseEnemy() = default;

    virtual void fInitialize() = 0;
    virtual void fUpdate(float elapsedTime_) = 0;
    void fRender(ID3D11DeviceContext* pDeviceContext_) const;

    //****************************************************************
    // 
    // 変数 
    // 
    //****************************************************************
private:
    // モデル
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
    DirectX::XMFLOAT3 mPosition{};    // 位置
    DirectX::XMFLOAT4 mOrientation{}; //回転
    DirectX::XMFLOAT3 mScale{};       // 大きさ
};