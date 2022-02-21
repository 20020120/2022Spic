#pragma once
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"BaseEnemy.h"
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
    //ロックオンできる距離
    static constexpr float LOCK_ON_LANGE = 70.0f;
    //後ろに回り込める距離
    static constexpr float  BEHIND_LANGE = 20.0f;

private:
    DirectX::XMFLOAT3 camera_forward{};//カメラの前方向
    DirectX::XMFLOAT3 camera_right{};//カメラの右方向
    DirectX::XMFLOAT3 camera_position{};//カメラの右方向
private:
    //プレイヤーの各方向
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;
private:
    //カメラの向きをリセット
    bool camera_reset{ false };
    //敵をロックオンしたかどうか(カメラ用のロックオンしたかどうか)
    bool is_camera_lock_on{ false };
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
    //回避ブースト開始と到達
    DirectX::XMFLOAT3 avoidance_start{};
    DirectX::XMFLOAT3 avoidance_end{};
    float avoidance_easing_time{ 0.6f };
    //ブーストの倍率
    float leverage{ 15.0f };
private:
    //ターゲットの敵
    const  BaseEnemy* target_enemy;
private:
    //後ろに回り込むための計算する関数
    void BehindAvoidancePosition();
    //スプライン曲線を使うための途中の点
    DirectX::XMFLOAT3 behind_point_0{};//スタート
    DirectX::XMFLOAT3 behind_point_1{};//中継地点
    DirectX::XMFLOAT3 behind_point_2{};//中継地点
    DirectX::XMFLOAT3 behind_point_3{};//ゴール
private:
    //プレイヤーの攻撃力(コンボによって変化していく)
    int player_attack_power{ 1 };
    //コンボ数
    int combo_count{ 0 };
private:
    //カプセル敵との当たり判定
    struct CapsuleParam
    {
        DirectX::XMFLOAT3 start{};
        DirectX::XMFLOAT3 end{};
        float rasius{ 1.2f };
    }capsule_parm;
    DirectX::XMFLOAT3 capsule_body_start{0,2.6f,0};
    DirectX::XMFLOAT3 capsule_body_end{0,0.2f,0};

    void BodyCapsule();

public:
    DirectX::XMFLOAT3 GetForward() { return forward; }
    DirectX::XMFLOAT3 GetRight() { return right; }
    DirectX::XMFLOAT3 GetUp() { return up; }
    DirectX::XMFLOAT3 GetPosition() { return position; }
    DirectX::XMFLOAT3 GetVelocity() { return velocity; }
    HitResult& GetPlayerHitResult() { return hit; }
    bool GetCameraReset() { return camera_reset; }
    bool GetCameraLockOn() { return is_camera_lock_on; }
    bool GetEnemyLockOn() { return is_lock_on; }
    CapsuleParam GetCapsuleParam() { return capsule_parm; }
    void SetRaycast(bool r) { raycast = r; }
    int GetPlayerPower() { return player_attack_power; }
    const  BaseEnemy* GetPlayerTargetEnemy() const { return target_enemy; }
    //一番近い敵を持って来てその位置をセットする
    void SetTarget(const BaseEnemy* target_enemy);
    DirectX::XMFLOAT3 GetTarget() { return target; };
    void AddCombo(int count) { combo_count += count; }
public:
    void FalseCameraReset() { camera_reset = false; }
    void FalseCameraLockOn() { is_camera_lock_on = false; }
private:
    void GetPlayerDirections();
public:
    void SetCameraDirection(const DirectX::XMFLOAT3& c_forward,  const DirectX::XMFLOAT3& c_right)
    {
        camera_forward = c_forward;
        camera_right = c_right;
    }
    void SetCameraPosition(DirectX::XMFLOAT3 p) { camera_position = p; }
private:
    //回避の加速
    void AvoidanceAcceleration(float elapse_time);
    //突進の加速(線形補間)
    void ChargeAcceleration(float elapse_time);
private:
    //ロックオン
    void LockOn();
    //カメラリセット
    void CameraReset();
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