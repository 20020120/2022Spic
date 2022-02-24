#pragma once
#include"MoveBehavior.h"
class PlayerMove : public MoveBehavior
{
public:
    PlayerMove();
    ~PlayerMove();
public:
    void UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right,const DirectX::XMFLOAT3& camera_pos ,SkyDome* sky_dome);
    //回避中のUpdateVelocity
    void UpdateAvoidanceVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right,const DirectX::XMFLOAT3& camera_pos ,SkyDome* sky_dome);
    //回り込み中の更新処理
    void UpdateBehindAvoidanceVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right,const DirectX::XMFLOAT3& camera_pos ,SkyDome* sky_dome);
    //攻撃中の更新処理
    void UpdateAttackVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right,const DirectX::XMFLOAT3& camera_pos ,SkyDome* sky_dome);

private:
    //垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedFrame);
    //垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position, SkyDome* sky_dome);
    //水平速力更新処理
    void UpdateHrizontalVelocity(float elasedFrame);
    //水平移動更新処理
    void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position, SkyDome* sky_dome);
    //ターゲットに向かって回転
    void RotateToTarget(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);
    //ピッチ回転
    void PitchTurn(DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& camera_pos, const DirectX::XMFLOAT3& camera_forward, DirectX::XMFLOAT4& orientation, float elapsed_time);
    //ロール回転
    void RollTurn(DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, float elapsed_time);
protected:
    //敵をロックオンしたかどうか
    bool is_lock_on{ false };
    //敵がいるかどうか
    bool is_enemy{ false };
    //ターゲット
    DirectX::XMFLOAT3 target{};
private:
    //プレイヤーの前方向
    DirectX::XMFLOAT3 player_forward{};
    //方向取得
    void SetDirections(DirectX::XMFLOAT4 o);
};