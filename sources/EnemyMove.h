#pragma once
#include"MoveBehavior.h"
class EnemyMove : public MoveBehavior
{
public:
    EnemyMove() :MoveBehavior(){}
    ~EnemyMove(){}
protected:
    void fUpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);
    //速力計算処理
    void fCalcVelocity(float elasedFrame);
    //移動更新処理
    void fUpdateMove(float elapsedTime_, DirectX::XMFLOAT3& position);

public:
    void MovingProcess(DirectX::XMFLOAT3 v, float speed);
};