#pragma once
#include<memory>
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"BaseEnemy.h"
#include"player_config.h"
#include"SwordTrail.h"
#include"post_effect.h"
#include"graphics_pipeline.h"
#include "Common.h"
class Player :public BasePlayer, private PlayerMove
{
public:
    Player(GraphicsPipeline& graphics);
    ~Player();
private:
    enum AnimationClips
    {
        Idle,//待機
        TransformWing,//飛行機モードになる
        IdleWing,//変形待機
        TransformHum,//人型モードになる
        Move,//移動
        Avoidance,//回避
        ChargeInit,//突進開始
        Charge,//突進中
        AttackType1,//攻撃1撃目
        AttackType2,//攻撃2撃目
        AttackType3,//攻撃3撃目
        Damage,//ダメージ
        Awaking,//覚醒状態になる
        InvAwaking,//通常状態になる
        AwakingIdle,//覚醒状態の待機
        AwakingMove,//覚醒状態の移動
        AwakingAvoidance,//覚醒状態の回避
        AwakingChargeInit,//覚醒状態の突進開始
        AwakingCharge,//覚醒状態の突進
        AwakingAttackType1,//覚醒状態の攻撃1撃目
        AwakingAttackType2,//覚醒状態の攻撃2撃目
        AwakingAttackType3,//覚醒状態の攻撃3撃目
        AwakingDamage,//覚醒状態のダメージ
    };
public:
    void Initialize()override;
    void Update(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome)override;
    void Render(GraphicsPipeline& graphics, float     elapsed_time)override;
private:
    //突進時間
    static constexpr float CHARGE_MAX_TIME = 1.0f;
    //攻撃1撃目の猶予時間
    static constexpr float ATTACK_TYPE1_MAX_TIME = 0.7f;
    //攻撃2撃目の猶予時間
    static constexpr float ATTACK_TYPE2_MAX_TIME = 0.7f;
    //ロックオンできる距離
    static constexpr float LOCK_ON_LANGE = 70.0f;
    //後ろに回り込める距離
    static constexpr float  BEHIND_LANGE = 20.0f;
    //攻撃力の最低値
    static const int MIN_PLAYER_ATTACK_POWER = 1;
    //攻撃力の最大値
    static const int MAX_PLAYER_ATTACK_POWER = 20;
    //コンボの最大数
    static constexpr float MAX_COMBO_COUNT = 200.0f;
    //回避の時のアニメーションスピード
    static constexpr float AVOIDANCE_ANIMATION_SPEED = 1.0f;
    //突進開始の時のアニメーションスピード
    static constexpr float CHARGEINIT_ANIMATION_SPEED = 1.5f;
    //突進の時のアニメーションスピード
    static constexpr float CHARGE_ANIMATION_SPEED = 1.0f;
    //攻撃1の時のアニメーションスピード
    static constexpr float ATTACK1_ANIMATION_SPEED = 1.0f;
    //攻撃2の時のアニメーションスピード
    static constexpr float ATTACK2_ANIMATION_SPEED = 1.0f;
    //攻撃3の時のアニメーションスピード
    static constexpr float ATTACK3_ANIMATION_SPEED = 1.0f;
    //ゲージ消費量(突進)
    static constexpr float GAUGE_CONSUMPTION = 5.0f;
    //人型に戻るときのアニメーションスピード
    static constexpr float TRANSFORM_HUM_ANIMATION_SPEED = 2.0f;
    //飛行機モードになるときのアニメーションスピード
    static constexpr float TRANSFORM_WING_ANIMATION_SPEED = 2.0f;


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
    //カメラ用の回避した瞬間
    bool is_avoidance{ false };
private:
    //ターゲットの敵
    const  BaseEnemy* target_enemy;
    float enemy_length{ 0 };//敵とプレイヤーの距離
    int target_count{ 0 };
    int old_target_count{ 0 };
private:
    //後ろに回り込むための計算する関数
    void BehindAvoidancePosition();
    //スプライン曲線を使うための途中の点
    DirectX::XMFLOAT3 behind_point_0{};//スタート
    DirectX::XMFLOAT3 behind_point_1{};//中継地点
    DirectX::XMFLOAT3 behind_point_2{};//中継地点
    DirectX::XMFLOAT3 behind_point_3{};//ゴール
    //背後に回り込む点
    std::vector<DirectX::XMFLOAT3> behind_point{};
    void InterpolateCatmullRomSpline(float elapsed_time);
    //背後に回り込むときに進むタイマー
    float behind_timer{};
    //背後に回り込むときのレート
    float behind_late{};
private:
    //プレイヤーの攻撃力(コンボによって変化していく)
    int player_attack_power{ MIN_PLAYER_ATTACK_POWER  };
    //コンボ数
    float combo_count{ 0 };
    //ゲージ消費の突進中に当たった数
    float special_surge_combo_count{ 0 };
    //プレイヤーが今攻撃中かそうでないか
    bool is_attack{ false };
    //プレイヤーげゲージ消費の突進をしているか
    bool is_special_surge{ false };
    //ゲージ消費の突進の隙
    float special_surge_opportunity{ 1.5f };
    float special_surge_timer{ 0 };
    float opportunity_timer{ 0 };
    //プレイヤーの体力
    int player_health = 100;
    //無敵時間
    float invincible_timer{};
    //コンボの持続時間
    float duration_combo_timer{};
    //ロックオンしていないときの突進のターゲット
    DirectX::XMFLOAT3 charge_point{};
    //ダッシュエフェクト開始
    bool start_dash_effect{ false };
    //ダッシュエフェクト終了
    bool end_dash_effect{ false };
    //覚醒状態かどうか
    bool is_awakening{ false };
    //突進中かどうか
    bool is_charge{ false };
    //プレイヤーのアニメーションスピード
    float animation_speed{ 1.0f };
    //突進の加速用のvelocity
    DirectX::XMFLOAT3 acceleration_velocity;
    //突進のvelocityの補間のレート
    float lerp_rate{ 1.0f };
    //突進のターゲットまでの距離の倍率(どれだけ伸ばすか)
    float charge_length_magnification{ 65.0f };
    //攻撃のアニメーションスピードのデバッグ用
    DirectX::XMFLOAT4 attack_animation_speeds{ 1.0f,1.0f,1.0f,1.0f };
    //攻撃のアニメーション補間の時間
    DirectX::XMFLOAT4 attack_animation_blends_speeds{ 0.3f,0.0f,0.0f,0.0f };
    //アニメーションをしていいかどうか
    bool is_update_animation{ true };
    //プレイヤーのパラメータ
    std::unique_ptr<PlayerConfig> player_config{ nullptr };
    //--------------------<SwordTrail～剣の軌跡～>--------------------//
    SwordTrail mSwordTrail[2]{};
    float mTrailEraseTimer{};


    skeleton::bone player_bones[8];
private:
    //プレイヤーのパラメータの変化
    void InflectionParameters(float elpased_time);
    //攻撃力の変化
    void InflectionPower(float elapsed_time);
    //コンボの変化
    void InflectionCombo(float elapsed_time);
    //ダメージ受けたときに後ろにはじかれる

private:
    //カプセル敵との当たり判定
    struct CapsuleParam
    {
        DirectX::XMFLOAT3 start{};
        DirectX::XMFLOAT3 end{};
        float rasius{ 1.2f };
    };

    CapsuleParam sword_capsule_param[2]{};
    CapsuleParam charge_capsule_param{};
    CapsuleParam body_capsule_param{};

    DirectX::XMFLOAT3 capsule_body_start{0,2.6f,0};
    DirectX::XMFLOAT3 capsule_body_end{0,0.2f,0};

    void BodyCapsule();
    //剣のカプセル判定
    void SwordCapsule();
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
    bool GetAvoidance() { return is_avoidance; }
    bool GetIsPlayerAttack() { return is_attack; }
    bool GetIsSpecialSurge() { return is_special_surge; }
    bool GetStartDashEffect() { return start_dash_effect; }
    bool GetEndDashEffect() { return end_dash_effect; }
    bool GetIsAwakening() { return is_awakening; }
    CapsuleParam GetBodyCapsuleParam() { return body_capsule_param; }
    CapsuleParam GetSwordCapsuleParam(int i)
    {
        if (is_charge)
        {
            return charge_capsule_param;
        }
        if (is_awakening)
        {
            return sword_capsule_param[i];
        }
        return sword_capsule_param[0];
    }
    std::vector<DirectX::XMFLOAT3> GetBehindPoint() {return behind_point;}
    void SetRaycast(bool r) { raycast = r; }
    int GetPlayerPower() { return player_attack_power; }
    [[nodiscard("Not used")]] const AddDamageFunc GetDamagedFunc() { return damage_func; }

    const  BaseEnemy* GetPlayerTargetEnemy() const
    {
        if (target_enemy != nullptr && target_enemy->fGetIsFrustum())
        {
            return target_enemy;
        }
        return nullptr;
    }
    //一番近い敵を持って来てその位置をセットする
    void SetTarget(const BaseEnemy* target_enemy);
    DirectX::XMFLOAT3 GetTarget() { return target; };
    void AddCombo(int count);
    //--------------------<敵からダメージを受ける>--------------------//
    void Damaged(int damage, float InvincibleTime);
    void PlayerKnocKback(float elapsed_time);
public:
    void FalseCameraReset() { camera_reset = false; }
    void FalseCameraLockOn() { is_camera_lock_on = false; }
    void FalseAvoidance() { is_avoidance = false; }
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
    //突進の加速の設定
    void SetAccelerationVelocity();
    //ゲージ消費の突進
    void SpecialSurgeAcceleration();
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
    AddDamageFunc damage_func;

    //自分のメンバ関数の関数ポインタを呼ぶ
    void ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome);

    void IdleUpdate(float elapsed_time, SkyDome* sky_dome);                  //待機アニメーション中の更新処理
    void MoveUpdate(float elapsed_time, SkyDome* sky_dome);               //移動アニメーション中の更新処理
    void AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);         //回避アニメーション中の更新処理
    void BehindAvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);//後ろに回り込む回避の更新処理
    void ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome);         //突進開始アニメーション中の更新処理
    void ChargeUpdate(float elapsed_time, SkyDome* sky_dome);             //突進中の更新処理
    void AttackType1Update(float elapsed_time, SkyDome* sky_dome);      //攻撃1撃目の更新処理
    void AttackType2Update(float elapsed_time, SkyDome* sky_dome);      //攻撃2撃目の更新処理
    void AttackType3Update(float elapsed_time, SkyDome* sky_dome);      //攻撃3撃目の更新処理
    void SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome);     //ゲージ消費する突進
    void OpportunityUpdate(float elapsed_time, SkyDome* sky_dome);     //ゲージ消費突進が終わってからの隙
    void DamageUpdate(float elapsed_time, SkyDome* sky_dome);          //ダメージ受けたとき
    void TransformHumUpdate(float elapsed_time, SkyDome* sky_dome) ; //人型に戻る
    void TransformWingUpdate(float elapsed_time, SkyDome* sky_dome); //飛行機モード
    void AwakingUpdate(float elapsed_time, SkyDome* sky_dome);          //覚醒状態に変形するときの更新
    void InvAwakingUpdate(float elapsed_time, SkyDome* sky_dome);      //通常状態に変形するときの更新

    void Awaiking();//覚醒状態のON,OFF
    //アニメーション遷移(1frameだけしか呼ばないもの)
    void TransitionIdle(float blend_second = 0.3f);
    void TransitionMove(float blend_second = 0.3f);
    void TransitionAvoidance();
    void TransitionBehindAvoidance();//背後に回り込む回避
    void TransitionChargeInit();
    void TransitionCharge(float blend_seconds = 0.3f);
    void TransitionAttackType1(float blend_seconds = 0.3f);
    void TransitionAttackType2(float blend_seconds = 0.3f);
    void TransitionAttackType3(float blend_seconds = 0.3f);
    void TransitionSpecialSurge();
    void TransitionOpportunity();
    void TransitionDamage();
    void TransitionTransformHum();
    void TransitionTransformWing();
    void TransitionAwaking();
    void TransitionInvAwaking();
};