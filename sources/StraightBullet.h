#pragma once
#include"BaseBullet.h"
//****************************************************************
// 
// 直進するだけの弾 
// 
//****************************************************************

class StraightBullet final :public BaseBullet
{
public:
    StraightBullet(GraphicsPipeline& Graphics_,DirectX::XMFLOAT3 InitPoint_,DirectX::XMFLOAT3 mVelocity_);
    ~StraightBullet()override = default;
    void fUpdate(float elapsedTime_) override;
private:
    float mLifeTime{}; // 生存時間

};


