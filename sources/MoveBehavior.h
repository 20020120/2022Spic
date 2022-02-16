#pragma once
#include<DirectXMath.h>
#include"practical_entities.h"
class MoveBehavior : public PracticalEntities
{
public:
    MoveBehavior() {}
    virtual ~MoveBehavior() {}
protected:
    DirectX::XMFLOAT3 velocity{};
    float acceleration{1.0f};
    float max_move_speed{5.0f};
    float move_vec_x{};
    float move_vec_z{};
    float friction{0.5f};
    float move_speed{5.0f};
    float turn_speed{ DirectX::XMConvertToRadians(720) };
protected:
    //�������͍X�V����
    virtual  void UpdateVerticalVelocity(float elapsedFrame) {};
    //�����ړ��X�V����
    virtual void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position) {};
    //�������͍X�V����
    virtual void UpdateHrizontalVelocity(float elasedFrame) {};
    //�����ړ��X�V����
    virtual  void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position) {};
public:
    //---------�R���g���[���[�ňړ����鏈��----------//
    DirectX::XMFLOAT3 SetMoveVec(const DirectX::XMFLOAT3& camera_forward,const DirectX::XMFLOAT3& camera_right);
    void MovingProcess(float vx, float vz, float speed);
    virtual void Turn(float elapsed_time, float vx, float vz, float speed, DirectX::XMFLOAT4& orientation);
    //--------------------------------------------//

};

inline DirectX::XMFLOAT4X4 QuaternionConvertXMFLOAT4X4(DirectX::XMFLOAT4 orientation)
{
    using namespace DirectX;

    XMVECTOR orientation_vec{ XMLoadFloat4(&orientation) };
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);

    return m4x4;
}
