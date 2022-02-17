#pragma once
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"graphics_pipeline.h"

class Player :public BasePlayer, private PlayerMove
{
public:
    Player(GraphicsPipeline& graphics);
    ~Player();
private:
    enum AnimationClips
    {
        Idle,//待機
        Move,//移動
        Avoidance,//回避
        ChargeInit,//突進開始
        Charge,//突進中
        AttackType1,//攻撃1撃目
        AttackType2,//攻撃2撃目
        AttackType3,//攻撃3撃目
    };
public:
    void Initialize()override;
    void Update(float elapsed_time, SkyDome* sky_dome)override;
    void Render(GraphicsPipeline& graphics, float elapsed_time)override;
private:
    //突進時間
    static constexpr float CHARGE_MAX_TIME = 3.0f;
    //攻撃1撃目の猶予時間
    static constexpr float ATTACK_TYPE1_MAX_TIME = 0.2f;
    //攻撃2撃目の猶予時間
    static constexpr float ATTACK_TYPE2_MAX_TIME = 0.3f;
private:
    DirectX::XMFLOAT3 camera_forward{};//カメラの前方向
    DirectX::XMFLOAT3 camera_right{};//カメラの右方向
private:
    //プレイヤーの各方向
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;
private:
    //カメラの向きをリセット
    bool camera_reset{ false };
private:
    //敵に当たったかどうか
    bool is_enemy_hit{ false };
private:
    //突進中の時間(突進が始まったらタイマーが動く)
    float charge_time{ 0 };
    //突進中の時間をどれだけ増やすか
    float charge_add_time{ 1.0f };
    //攻撃の時間(アニメーションが終わったら増える)
    float attack_time{ 0 };
    //攻撃中の時間をどれだけ増やすか
    float attack_add_time{ 1.0f };
private:
    //回避が始まった時間
    float avoidance_boost_time{ 0 };
    //回避力
    float  avoidance_velocity{ 15.0f };
    //イージングの効果時間
    float easing_time{ 1.0f };
private:
    //プレイヤーの攻撃力(コンボによって変化していく)
    int player_attack_power{ 1 };
    //コンボ数
    int combo_count{ 0 };
public:
    DirectX::XMFLOAT3 GetForward() { return forward; }
    DirectX::XMFLOAT3 GetRight() { return right; }
    DirectX::XMFLOAT3 GetUp() { return up; }
    DirectX::XMFLOAT3 GetPosition() { return position; }
    DirectX::XMFLOAT3 GetVelocity() { return velocity; }
    HitResult& GetPlayerHitResult() { return hit; }
    bool GetCameraReset() { return camera_reset; }
    void SetRaycast(bool r) { raycast = r; }
    bool GetEnemyLockOn() { return is_lock_on; }
public:
    void FalseCameraReset() { camera_reset = false; }
private:
    void GetPlayerDirections();
public:
    void SetCameraDirection(const DirectX::XMFLOAT3& c_forward,  const DirectX::XMFLOAT3& c_right)
    {
        camera_forward = c_forward;
        camera_right = c_right;
    }
private:
    //回避の加速
    void AvoidanceAcceleration(float elapse_time);
    //突進の加速(線形補間)
    void ChargeAcceleration(float elapse_time);
private:
    //ロックオン
    void LockOn();
private:
    //-----------アニメーションに関係する関数,変数------------//

    //アニメーション遷移の関数ポインタ//
    //関数ポインタ
    typedef void(Player::* PlayerActivity)(float elapsed_time, SkyDome* sky_dome);
    //関数ポインタの変数
    PlayerActivity player_activity = &Player::IdleUpdate;
    //自分のメンバ関数の関数ポインタを呼ぶ
    void ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome);

    void IdleUpdate(float elapsed_time, SkyDome* sky_dome);//待機アニメーション中の更新処理
    void MoveUpdate(float elapsed_time, SkyDome* sky_dome);//移動アニメーション中の更新処理
    void AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);//回避アニメーション中の更新処理
    void ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome);//突進開始アニメーション中の更新処理
    void ChargeUpdate(float elapsed_time, SkyDome* sky_dome);//突進中の更新処理
    void AttackType1Update(float elapsed_time, SkyDome* sky_dome);//攻撃1撃目の更新処理
    void AttackType2Update(float elapsed_time, SkyDome* sky_dome);//攻撃2撃目の更新処理
    void AttackType3Update(float elapsed_time, SkyDome* sky_dome);//攻撃3撃目の更新処理

    //アニメーション遷移(1frameだけしか呼ばないもの)
    void TransitionIdle();
    void TransitionMove();
    void TransitionAvoidance();
    void TransitionChargeInit();
    void TransitionCharge();
    void TransitionAttackType1(float blend_seconds);
    void TransitionAttackType2(float blend_seconds);
    void TransitionAttackType3(float blend_seconds);
};