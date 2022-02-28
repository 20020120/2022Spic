#include"StraightBullet.h"
#include"Operators.h"
StraightBullet::StraightBullet(GraphicsPipeline& Graphics_, DirectX::XMFLOAT3 InitPoint_, DirectX::XMFLOAT3 mVelocity_)
    :BaseBullet(Graphics_, "./resources/Effect/RedPlayerBullet.efk")
{
    mPosition = InitPoint_;
    mVelocity = mVelocity_;
    mScale = { 0.2f,0.2f,0.2f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mLifeTime = 10.0f;
}

void StraightBullet::fUpdate(float elapsed_time)
{
    mPosition += (mVelocity * elapsed_time);
    mLifeTime -= elapsed_time;
    mIsAlive = mLifeTime <= 0.0f;
    BaseBullet::fUpdate(elapsed_time);
}