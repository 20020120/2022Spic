#pragma once
#include"MoveBehavior.h"
class PlayerMove : public MoveBehavior
{
public:
    PlayerMove();
    ~PlayerMove();
public:
    void UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right, SkyDome* sky_dome);
private:
    //垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedFrame);
    //垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position, SkyDome* sky_dome);
    //水平速力更新処理
    void UpdateHrizontalVelocity(float elasedFrame);
    //水平移動更新処理
    void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position, SkyDome* sky_dome);

};