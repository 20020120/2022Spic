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

    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time);
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
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time);
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
    }
    UpdateAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * AVOIDANCE_ANIMATION_SPEED);
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
    model->update_animation(elapsed_time);
}

void Player::ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionCharge();
    }

    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::ChargeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    start_dash_effect = false;
    charge_time += charge_add_time * elapsed_time;
    ChargeAcceleration(elapsed_time);
    //突進時間を超えたらそれぞれの遷移にとぶ
    if (charge_time > CHARGE_MAX_TIME)
    {
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
    else
    {
        if (is_enemy_hit)
        {
            //敵に当たって攻撃ボタン(突進ボタン)を押したら一撃目
                TransitionAttackType1(0);
        }
    }
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
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
            if (target_count != old_target_count)
            {
                TransitionCharge();
            }
            else
            {
                TransitionAttackType2(0);
            }
        }
    }
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
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
            if (target_count != old_target_count)
            {
                TransitionCharge();
            }
            else
            {
                TransitionAttackType3(0);
            }
        }
    }
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::AttackType3Update(float elapsed_time, SkyDome* sky_dome)
{

    if (model->end_of_animation())
    {
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionCharge();
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
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    SpecialSurgeAcceleration(elapsed_time);

    if (model->end_of_animation())
    {
        TransitionOpportunity();
    }
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::OpportunityUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //このカウントが多ければ隙がなくなっていく
    //special_surge_combo_count

    //ここでどれだけほかの動きに遷移するのに必要な待ち時間を決める
    switch (static_cast<int>(special_surge_combo_count / 5))
    {
    case 0:
        special_surge_opportunity = 1.5f;
        break;
    case 1:
        special_surge_opportunity = 1.0f;
        break;
    case 2:
        special_surge_opportunity = 0.5f;
        break;
    default:
        special_surge_opportunity = 0;
        break;
    }
    special_surge_timer += 1.0f * elapsed_time;
    //設定した隙よりも時間がたったらそれぞれの行動に遷移する
    if (special_surge_timer > special_surge_opportunity)
    {
        special_surge_combo_count = 0;
        //移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        else
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
        model->update_animation(elapsed_time);
}

void Player::TransitionIdle()
{
    end_dash_effect = true;
    model->play_animation(AnimationClips::Idle, true);
    is_attack = false;
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove()
{
    end_dash_effect = true;
    model->play_animation(AnimationClips::Move, true);
    is_attack = false;
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    //--------------------------イージング加速の変数初期化---------------------------------//
    avoidance_boost_time = 0;
    avoidance_start = velocity;
    if (is_lock_on)
    {
        leverage = 5.0f;
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
        leverage = 15.0f;
        avoidance_end = { forward.x * leverage ,forward.y * leverage,forward.z * leverage };
    }
    //-----------------------------------------------------------------------------------------//
    model->play_animation(AnimationClips::Avoidance, false);
    is_avoidance = true;
    is_attack = false;
    player_activity = &Player::AvoidanceUpdate;
}

void Player::TransitionBehindAvoidance()
{
    BehindAvoidancePosition();
    is_avoidance = true;
    behind_timer = 0;
    behind_late = 0;
    velocity = {};
    is_attack = false;
    player_activity = &Player::BehindAvoidanceUpdate;;
}

void Player::TransitionChargeInit()
{
    model->play_animation(AnimationClips::ChargeInit, false);
    is_attack = true;
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge()
{
    end_dash_effect = false;
    start_dash_effect = true;//ポストエフェクトをかける
    model->play_animation(AnimationClips::Charge, false,0);
    is_attack = true;
    charge_point = Math::calc_designated_point(position, forward, 60.0f);
    player_activity = &Player::ChargeUpdate;

}

void Player::TransitionAttackType1(float blend_seconds)
{
    end_dash_effect = true;
    model->play_animation(AnimationClips::AttackType1, false, 0);
    is_attack = true;
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    model->play_animation(AnimationClips::AttackType2, false, 0);
    is_attack = true;
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    model->play_animation(AnimationClips::AttackType3, false, 0);
    is_attack = true;
    player_activity = &Player::AttackType3Update;
}

void Player::TransitionSpecialSurge()
{
    model->play_animation(AnimationClips::Charge, false, 0);
    special_surge_combo_count = 0;//ゲージ消費の突進中に当たった敵の数を初期化しておく
    is_special_surge = true;
    is_attack = true;
    player_activity = &Player::SpecialSurgeUpdate;
    combo_count -= 10.0f;
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);

}

void Player::TransitionOpportunity()
{
    is_special_surge = false;//突進攻撃終了
    is_attack = false;
    model->play_animation(AnimationClips::Idle, true, 0);
    special_surge_timer = 0;//隙が生じた時の経過時間をリセット
    player_activity = &Player::OpportunityUpdate;
}
