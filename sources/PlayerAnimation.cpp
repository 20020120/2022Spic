#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //自分のクラスの関数ポインタを呼ぶ
    (this->*player_activity)(elapsed_time, sky_dome);

}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //移動に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //回避に遷移
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        TransitionAvoidance();
    }
    //突進開始に遷移
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //移動入力がなくなったら待機に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //回避に遷移
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        TransitionAvoidance();
    }
    //突進開始に遷移
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }

}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    AvoidanceAcceleration(elapsed_time);
    //回避のアニメーションが終わったら
    if (model->end_of_animation())
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
}

void Player::ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionCharge();
    }
}

void Player::ChargeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    charge_time += charge_add_time * elapsed_time;
    //ロックオン中はターゲットに向かって突進する
    if (is_lock_on)
    {
        ChargeAcceleration(elapsed_time);
    }
    else
    {

    }
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
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
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
            TransitionAttackType2(0);
        }
    }
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
            TransitionAttackType3(0);
        }
    }
}

void Player::AttackType3Update(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionIdle();
    }
}

void Player::TransitionIdle()
{
    model->play_animation(AnimationClips::Idle, true);
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove()
{
    model->play_animation(AnimationClips::Move, true);
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    BehindAvoidancePosition();
    avoidance_boost_time = 0;
    avoidance_start = velocity;
    avoidance_end = { forward.x * 10.0f ,forward.y * 10.0f,forward.z * 10.0f };
    model->play_animation(AnimationClips::Avoidance, false);
    player_activity = &Player::AvoidanceUpdate;
}

void Player::TransitionChargeInit()
{
    model->play_animation(AnimationClips::ChargeInit, false);
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge()
{
    model->play_animation(AnimationClips::Charge, false,0);
    player_activity = &Player::ChargeUpdate;
}

void Player::TransitionAttackType1(float blend_seconds)
{
    model->play_animation(AnimationClips::AttackType1, false, 0);
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    model->play_animation(AnimationClips::AttackType2, false, 0);
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    model->play_animation(AnimationClips::AttackType3, false, 0);
    player_activity = &Player::AttackType3Update;
}
