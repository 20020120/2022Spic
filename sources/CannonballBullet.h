#pragma once
#include"BaseBullet.h"
class CannonballBullet :public BaseBullet
{
public:
    CannonballBullet(DirectX::XMFLOAT3 FirstVec_, float Power_);
    void fUpdate(float elapsedTime_) override;
};