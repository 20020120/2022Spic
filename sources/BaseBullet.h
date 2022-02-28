#pragma once

#include"effect.h"
#include"graphics_pipeline.h"
#include"practical_entities.h"
#include<memory>
//****************************************************************
// 
// 弾の基底クラス 
// 
//****************************************************************
class BaseBullet : public PracticalEntities
{
public:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    BaseBullet(GraphicsPipeline& Graphics_, const char* EffectFileName_);
    ~BaseBullet() override = default;

    virtual void fUpdate(float elapsedTime_);
protected:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    std::unique_ptr<Effect> mpEffect{ nullptr };
    DirectX::XMFLOAT3 mPosition{};
    DirectX::XMFLOAT3 mScale{};
    DirectX::XMFLOAT4 mOrientation{};
    bool mIsAlive{}; // 弾の生存判定
    DirectX::XMFLOAT3 mVelocity{}; // 速度

};
