#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //自分のクラスの関数ポインタを呼ぶ
    (this->*player_activity)(elapsed_time, sky_dome);
}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    //移動に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //回避に遷移
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //後ろに回り込める距離なら回り込みようのUpdate
        if (is_lock_on && length < BEHIND_LANGE)
        {
            TransitionBehindAvoidance();//プロト段階では回り込み回避は消しておく
        }
        //そうじゃなかったら普通の回避
        else TransitionAvoidance();
    }
    //突進開始に遷移
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
    //ゲージ消費の突進
    if (is_lock_on && game_pad->get_button_down() & GamePad::BTN_Y)
    {
        if (combo_count >= 10.0f)
        {
            TransitionTransformWing();
        }
    }

    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    //移動入力がなくなったら待機に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //回避に遷移
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //後ろに回り込める距離なら回り込みようのUpdate
        if (is_lock_on && length < BEHIND_LANGE)
        {
            TransitionBehindAvoidance();
        }
        //そうじゃなかったら普通の回避
        else TransitionAvoidance();
    }
    //突進開始に遷移
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
    //ゲージ消費の突進
    if (is_lock_on && game_pad->get_button_down() & GamePad::BTN_Y)
    {
        if (combo_count >= 10.0f)
        {
            TransitionTransformWing();
        }
    }
    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    AvoidanceAcceleration(elapsed_time);
    //回避のアニメーションが終わったら
    if (avoidance_boost_time > avoidance_easing_time && model->end_of_animation())
    {
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            TransitionIdle();
        }
        Awaiking();
        UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
    else
    {
        UpdateAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
}

void Player::BehindAvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    behind_timer += 2.0f * elapsed_time;

    InterpolateCatmullRomSpline(elapsed_time);

    if (behind_timer > 1.0f)
    {
        TransitionIdle();
    }
    UpdateBehindAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionCharge(attack_animation_blends_speeds.x);
    }
    ChargeInitAcceleration(elapsed_time);
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    start_dash_effect = false;
    charge_time += charge_add_time * elapsed_time;
    ChargeAcceleration(elapsed_time);
    //突進時間を超えたらそれぞれの遷移にとぶ
    if (charge_time > CHARGE_MAX_TIME)
    {
        velocity = {};

        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            is_charge = false;
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            charge_time = 0;
            is_charge = false;
            TransitionIdle();
        }
        Awaiking();
    }
    else
    {
        if (is_enemy_hit)
        {
            //敵に当たって攻撃ボタン(突進ボタン)を押したら一撃目
            is_charge = false;
            velocity = {};
            TransitionAttackType1(attack_animation_blends_speeds.y);
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //猶予時間を超えたら待機に遷移
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //猶予時間よりも早く押したら攻撃2撃目に遷移
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            if (enemy_length > 10.0f)
            {
                TransitionCharge(attack_animation_blends_speeds.x);
            }
            else
            {
                TransitionAttackType2(attack_animation_blends_speeds.z);
            }
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType2Update(float elapsed_time, SkyDome* sky_dome)
{

    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //猶予時間を超えたら待機に遷移
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //猶予時間よりも早く押したら攻撃3撃目に遷移
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            if (enemy_length > 10.0f)
            {
                TransitionCharge(attack_animation_blends_speeds.x);
            }
            else
            {
                TransitionAttackType3(attack_animation_blends_speeds.w);
            }
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType3Update(float elapsed_time, SkyDome* sky_dome)
{

    if (model->end_of_animation())
    {
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionCharge(attack_animation_blends_speeds.x);
        }
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            charge_time = 0;
            TransitionIdle();
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    special_surge_timer += 1.0f * elapsed_time;

    if (special_surge_timer > 1.0f)
    {
        velocity = {};
        TransitionOpportunity();
    }
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::OpportunityUpdate(float elapsed_time, SkyDome* sky_dome)
{
    opportunity_timer += 1.0f * elapsed_time;
    if (special_surge_combo_count > 0)special_surge_opportunity = 2.0f / special_surge_combo_count;
    else special_surge_opportunity = 2.0f;

    //設定した隙よりも時間がたったらそれぞれの行動に遷移する
    if (opportunity_timer > special_surge_opportunity)
    {
        special_surge_combo_count = 0;
        TransitionTransformHum();
    }
        //UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::DamageUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionIdle();
    }
}

void Player::TransformHumUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        //移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        else
        {
            TransitionIdle(0);
        }
        //回避に遷移
        float length{ Math::calc_vector_AtoB_length(position, target) };
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE)
            {
                TransitionBehindAvoidance();//プロト段階では回り込み回避は消しておく
            }
            //そうじゃなかったら普通の回避
            else TransitionAvoidance();
        }
        //突進開始に遷移
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionChargeInit();
        }

    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::TransformWingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionSpecialSurge();
    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionIdle();
    }
}

void Player::InvAwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation()) TransitionIdle();
}

void Player::Awaiking()
{
    //ボタン入力
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT)TransitionAwaking();//コンボカウントが最大のときは覚醒状態になる
    }
    if (is_awakening &&combo_count <= 0) TransitionInvAwaking();//覚醒状態のときにカウントが0になったら通常状態になる
}

void Player::TransitionIdle(float blend_second)
{
    //ダッシュエフェクトの終了
    end_dash_effect = true;
    //覚醒状態の時の待機アニメーションにセット
    if(is_awakening)model->play_animation(AnimationClips::AwakingIdle, true,true,blend_second);
    //通常状態の待機アニメーションにセット
    else model->play_animation(AnimationClips::Idle, true,true,blend_second);
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーション速度
    animation_speed = 1.0f;
    //待機状態の時の更新関数に切り替える
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove(float blend_second)
{
    //ダッシュエフェクトの終了
    end_dash_effect = true;
    //覚醒状態の時の移動アニメーションの設定
    if(is_awakening)model->play_animation(AnimationClips::AwakingMove, true,true, blend_second);
    //通常状態の時に移動アニメーションの設定
    else model->play_animation(AnimationClips::Move, true, true,blend_second);
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーション速度
    animation_speed = 1.0f;
    //移動状態の時の更新関数に切り替える
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    //--------------------------イージング加速の変数初期化---------------------------------//
    avoidance_boost_time = 0;
    avoidance_start = velocity;
    if (is_lock_on)
    {
        leverage = 15.0f;
        DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
        if (sqrtf((movevec.x * movevec.x) + (movevec.y * movevec.y) + (movevec.z * movevec.z)) <= 0.0f)
        {
            avoidance_end = { forward.x * leverage ,forward.y * leverage,forward.z * leverage };
        }
        else
        {
            avoidance_end = { movevec.x * leverage ,movevec.y * leverage,movevec.z * leverage };
        }
    }
    else
    {
        leverage = 30.0f;
        avoidance_end = { forward.x * leverage ,forward.y * leverage,forward.z * leverage };
    }
    //-----------------------------------------------------------------------------------------//
    //覚醒状態の時の回避アニメーションの設定
    if(is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false,true);
    //通常状態の時のアニメーションの設定
    else model->play_animation(AnimationClips::Avoidance, false,true);
    //回避中かどうかの設定
    is_avoidance = true;
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーションの速度
    animation_speed = 1.0f;
    //回避状態の時の更新関数に切り替える
    player_activity = &Player::AvoidanceUpdate;
}

void Player::TransitionBehindAvoidance()
{
    //後ろに回り込む座標の取得
    BehindAvoidancePosition();
    //回避中かどうかの設定
    is_avoidance = true;
    //回り込むときのタイマー
    behind_timer = 0;
    //回り込みの補完レート
    behind_late = 0;
    //移動速度の初期化
    velocity = {};
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーションの速度
    animation_speed = 1.0f;
    //背後に回り込むときの関数に切り替える
    player_activity = &Player::BehindAvoidanceUpdate;;
}

void Player::TransitionChargeInit()
{
    //覚醒状態の時の突進の始まりのアニメーションに設定
   if(is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false,true);
   //通常状態の時の突進の始まりのアニメーションに設定
   else model->play_animation(AnimationClips::ChargeInit, false,true);
   //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
    animation_speed = CHARGEINIT_ANIMATION_SPEED;
    //ロックオンしてない場合のターゲットの設定
    charge_point = Math::calc_designated_point(position, forward, 60.0f);
    //突進の始まりの時の更新関数に切り替える
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge(float blend_seconds)
{
    //ダッシュエフェクトの終了
    end_dash_effect = false;
    //ダッシュポストエフェクトをかける
    start_dash_effect = true;
    //覚醒状態の時の突進アニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingCharge, false, true, blend_seconds);
    //通常状態の時の突進アニメーションに設定
    else model->play_animation(AnimationClips::Charge, false, true, blend_seconds);
    //攻撃中かどうかの設定
    is_attack = true;
    //突進中かどうかの設定
    is_charge = true;
    //アニメーションスピードの設定
#if 1
    animation_speed = CHARGE_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.x;
#endif // 0
    //突進中の更新関数に切り替える
    player_activity = &Player::ChargeUpdate;

}

void Player::TransitionAttackType1(float blend_seconds)
{
    //ダッシュエフェクトの終了
    end_dash_effect = true;
    //覚醒状態の時の１撃目のアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType1, false, true, blend_seconds);
    //通常状態の時の１撃目のアニメーションに設定
    else model->play_animation(AnimationClips::AttackType1, false, true, blend_seconds);
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーションスピードの設定
#if 1
    animation_speed = ATTACK1_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.y;
#endif // 0
    //１撃目の更新関数に切り替える
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    //覚醒状態の時の２撃目のアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType2, false, true, blend_seconds);
    //通常状態の時の２撃目のアニメーションに設定
    else model->play_animation(AnimationClips::AttackType2, false, true, blend_seconds);
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
#if 1
    animation_speed = ATTACK2_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.z;
#endif // 0
    //２撃目の更新関数に切り替える
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    //覚醒状態の時の３撃目のアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType3, false, true, blend_seconds);
    //通常状態の時の３撃目ののアニメーションに設定
    else model->play_animation(AnimationClips::AttackType3, false, true, blend_seconds);
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
#if 1
    animation_speed = ATTACK3_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.w;
#endif // 0
    //３撃目の更新関数に切り替える
    player_activity = &Player::AttackType3Update;
}

void Player::TransitionSpecialSurge()
{
    //飛行機モードになるアニメーションに設定
    model->play_animation(AnimationClips::IdleWing, true,true);
    //ゲージ消費の突進中に当たった敵の数を初期化
    special_surge_combo_count = 0;
    //ゲージ消費の突進かどうかの設定
    is_special_surge = true;
    //攻撃中かどうかの設定
    is_attack = true;
    //ロックオンしてない場合のターゲットの設定
    charge_point = Math::calc_designated_point(position, forward, 10.0f);
    //ゲージ消費の突進の移動速度を設定
    SpecialSurgeAcceleration();
    //コンボカウントの消費
    combo_count -= 10.0f;
    //コンボカウントの制限
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);
    //ゲージ消費の突進のタイマー
    special_surge_timer = 0.0f;
    //アニメーションスピードの設定
    animation_speed = 1.0f;
    //ゲージ消費の突進の更新関数に切り替える
    player_activity = &Player::SpecialSurgeUpdate;
}

void Player::TransitionOpportunity()
{
    //ゲージ消費の突進攻撃終了
    is_special_surge = false;
    //攻撃中かどうかの設定
    is_attack = false;
    //隙が生じた時の経過時間をリセット
    special_surge_timer = 0;
    //アニメーションスピードの設定
    animation_speed = 1.0f;
    //ゲージ消費の突進の隙の更新関数に切り替える
    player_activity = &Player::OpportunityUpdate;
}

void Player::TransitionDamage()
{
    //ダッシュエフェクトの終了
    start_dash_effect = false;
    end_dash_effect = false;
    //攻撃中かどうかの設定
    is_attack = false;
    //覚醒状態の時のダメージアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingDamage, false, true);
    //通常状態の時のアニメーションに設定
    else model->play_animation(AnimationClips::Damage, false, true);
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //ダメージ受けたときの更新関数に切り替える
    player_activity = &Player::DamageUpdate;
}

void Player::TransitionTransformHum()
{
    //人型になるアニメーションに設定
    model->play_animation(AnimationClips::TransformHum, false,true);
    //アニメーション速度の設定
    animation_speed = TRANSFORM_HUM_ANIMATION_SPEED;
    //人型になってるときの更新関数に切り替える
    player_activity = &Player::TransformHumUpdate;
}

void Player::TransitionTransformWing()
{
    //飛行機モードになるアニメーションに設定
    model->play_animation(AnimationClips::TransformWing, false,true);
    //アニメーション速度の設定
    animation_speed = TRANSFORM_WING_ANIMATION_SPEED;
    //飛行機モード中の更新関数に切り替える
    player_activity = &Player::TransformWingUpdate;
}

void Player::TransitionAwaking()
{
    //覚醒状態になるアニメーションに設定
    model->play_animation(AnimationClips::Awaking, false,true);
    //覚醒状態かどうかの設定
    is_awakening = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //覚醒状態になる途中の更新関数に切り替える
    player_activity = &Player::AwakingUpdate;
}

void Player::TransitionInvAwaking()
{
    //通常状態に戻るアニメーションに設定
    model->play_animation(AnimationClips::InvAwaking, false,true);
    //覚醒状態かどうかの設定
    is_awakening = false;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //通常状態に戻ってるときの更新関数に切り替える
    player_activity = &Player::InvAwakingUpdate;

}
