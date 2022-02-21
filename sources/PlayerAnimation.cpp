#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //Ž©•ª‚ÌƒNƒ‰ƒX‚ÌŠÖ”ƒ|ƒCƒ“ƒ^‚ðŒÄ‚Ô
    (this->*player_activity)(elapsed_time, sky_dome);

}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //ˆÚ“®‚É‘JˆÚ
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //‰ñ”ð‚É‘JˆÚ
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        TransitionAvoidance();
    }
    //“ËiŠJŽn‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //ˆÚ“®“ü—Í‚ª‚È‚­‚È‚Á‚½‚ç‘Ò‹@‚É‘JˆÚ
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //‰ñ”ð‚É‘JˆÚ
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        TransitionAvoidance();
    }
    //“ËiŠJŽn‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }

}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    AvoidanceAcceleration(elapsed_time);
    //‰ñ”ð‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ªI‚í‚Á‚½‚ç
    if (model->end_of_animation())
    {
        //ˆÚ“®“ü—Í‚ª‚ ‚Á‚½‚çˆÚ“®‚É‘JˆÚ
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        //ˆÚ“®“ü—Í‚ª‚È‚©‚Á‚½‚ç‘Ò‹@‚É‘JˆÚ
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
    //ƒƒbƒNƒIƒ“’†‚Íƒ^[ƒQƒbƒg‚ÉŒü‚©‚Á‚Ä“Ëi‚·‚é
    if (is_lock_on)
    {
        ChargeAcceleration(elapsed_time);
    }
    else
    {

    }
    //“ËiŽžŠÔ‚ð’´‚¦‚½‚ç‚»‚ê‚¼‚ê‚Ì‘JˆÚ‚É‚Æ‚Ô
    if (charge_time > CHARGE_MAX_TIME)
    {
        //ˆÚ“®“ü—Í‚ª‚ ‚Á‚½‚çˆÚ“®‚É‘JˆÚ
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            TransitionMove();
        }
        //ˆÚ“®“ü—Í‚ª‚È‚©‚Á‚½‚ç‘Ò‹@‚É‘JˆÚ
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
            //“G‚É“–‚½‚Á‚ÄUŒ‚ƒ{ƒ^ƒ“(“Ëiƒ{ƒ^ƒ“)‚ð‰Ÿ‚µ‚½‚çˆêŒ‚–Ú
                TransitionAttackType1(0);
        }
    }
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //—P—\ŽžŠÔ‚ð’´‚¦‚½‚ç‘Ò‹@‚É‘JˆÚ
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //—P—\ŽžŠÔ‚æ‚è‚à‘‚­‰Ÿ‚µ‚½‚çUŒ‚2Œ‚–Ú‚É‘JˆÚ
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
        //—P—\ŽžŠÔ‚ð’´‚¦‚½‚ç‘Ò‹@‚É‘JˆÚ
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //—P—\ŽžŠÔ‚æ‚è‚à‘‚­‰Ÿ‚µ‚½‚çUŒ‚3Œ‚–Ú‚É‘JˆÚ
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
