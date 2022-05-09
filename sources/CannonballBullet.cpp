#include"CannonballBullet.h"
#include"Operators.h"
CannonballBullet::CannonballBullet(GraphicsPipeline& Graphics_,
    DirectX::XMFLOAT3 FirstVec_, float Power_, DirectX::XMFLOAT3 Position_)
        :BaseBullet(Graphics_, "./resources/Effect/fire_ball.efk",Position_)
{
    mVelocity = FirstVec_ * Power_;
    mPosition = Position_;
}

void CannonballBullet::fUpdate(float elapsed_time)
{

    // ï˙ï®ê¸
    const DirectX::XMFLOAT3 speed =
    {
        mVelocity.x,
        mVelocity.y * mkGravity,
        mVelocity.z
    };
    mVelocity = speed;
    mPosition += (speed * elapsed_time);

    if(mPosition.y<=0.0f)
    {
        mIsAlive = false;
    }
    BaseBullet::fUpdate(elapsed_time);
}
