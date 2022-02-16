#pragma once
#include"MoveBehavior.h"
class PlayerMove : public MoveBehavior
{
public:
    PlayerMove();
    ~PlayerMove();
public:
    void UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right);
private:
    //�������͍X�V����
    void UpdateVerticalVelocity(float elapsedFrame);
    //�����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position);
    //�������͍X�V����
    void UpdateHrizontalVelocity(float elasedFrame);
    //�����ړ��X�V����
    void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position);

};