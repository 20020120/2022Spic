#pragma once
#include"BaseBullet.h"
class CannonballBullet :public BaseBullet
{
public:
    CannonballBullet(GraphicsPipeline& Graphics_,
        DirectX::XMFLOAT3 FirstVec_, float Power_,DirectX::XMFLOAT3 Position_);
    void fUpdate(float elapsedTime_) override;

private:
    const float mkGravity{ 0.98f };
};