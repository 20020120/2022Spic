#pragma once
//#include<DirectXMath.h>
//class MoveBehavior
//{
//public:
//    MoveBehavior() {}
//    virtual ~MoveBehavior() {}
//protected:
//    DirectX::XMFLOAT3 velocity{};
//    float acceleration;
//    float max_move_speed;
//    float move_vec_x;
//    float move_vec_z;
//    float friction;
//    float move_speed;
//    float turn_speed;
//protected:
//    //�������͍X�V����
//    virtual  void UpdateVerticalVelocity(float elapsedFrame) {};
//    //�����ړ��X�V����
//    virtual void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position) {};
//    //�������͍X�V����
//    virtual void UpdateHrizontalVelocity(float elasedFrame) {};
//    //�����ړ��X�V����
//    virtual  void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position) {};
//public:
//    //---------�R���g���[���[�ňړ����鏈��----------//
//    DirectX::XMFLOAT3 SetMoveVec();
//    void MovingProcess(float vx, float vz, float speed);
//    virtual void Turn(float elapsed_time, float vx, float vz, float speed, DirectX::XMFLOAT4& orientation);
//    //--------------------------------------------//
//
//};
