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
//    //垂直速力更新処理
//    virtual  void UpdateVerticalVelocity(float elapsedFrame) {};
//    //垂直移動更新処理
//    virtual void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position) {};
//    //水平速力更新処理
//    virtual void UpdateHrizontalVelocity(float elasedFrame) {};
//    //水平移動更新処理
//    virtual  void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position) {};
//public:
//    //---------コントローラーで移動する処理----------//
//    DirectX::XMFLOAT3 SetMoveVec();
//    void MovingProcess(float vx, float vz, float speed);
//    virtual void Turn(float elapsed_time, float vx, float vz, float speed, DirectX::XMFLOAT4& orientation);
//    //--------------------------------------------//
//
//};
