#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //©•ª‚ÌƒNƒ‰ƒX‚ÌŠÖ”ƒ|ƒCƒ“ƒ^‚ğŒÄ‚Ô
    (this->*player_activity)(elapsed_time, sky_dome);

}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    //ˆÚ“®‚É‘JˆÚ
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //‰ñ”ğ‚É‘JˆÚ
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //Œã‚ë‚É‰ñ‚è‚ß‚é‹——£‚È‚ç‰ñ‚è‚İ‚æ‚¤‚ÌUpdate
        if (is_lock_on && length < BEHIND_LANGE)
        {
            //TransitionBehindAvoidance();//ƒvƒƒg’iŠK‚Å‚Í‰ñ‚è‚İ‰ñ”ğ‚ÍÁ‚µ‚Ä‚¨‚­
            TransitionAvoidance();
        }
        //‚»‚¤‚¶‚á‚È‚©‚Á‚½‚ç•’Ê‚Ì‰ñ”ğ
        else TransitionAvoidance();
    }
    //“ËiŠJn‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }

    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);

}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    //ˆÚ“®“ü—Í‚ª‚È‚­‚È‚Á‚½‚ç‘Ò‹@‚É‘JˆÚ
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //‰ñ”ğ‚É‘JˆÚ
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //Œã‚ë‚É‰ñ‚è‚ß‚é‹——£‚È‚ç‰ñ‚è‚İ‚æ‚¤‚ÌUpdate
        if (is_lock_on && length < BEHIND_LANGE)
        {
            //TransitionBehindAvoidance();
            TransitionAvoidance();
        }
        //‚»‚¤‚¶‚á‚È‚©‚Á‚½‚ç•’Ê‚Ì‰ñ”ğ
        else TransitionAvoidance();
    }
    //“ËiŠJn‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);

}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    AvoidanceAcceleration(elapsed_time);
    //‰ñ”ğ‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ªI‚í‚Á‚½‚ç
    if (avoidance_boost_time > avoidance_easing_time && model->end_of_animation())
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
    UpdateAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);

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
        TransitionCharge();
    }

    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    start_dash_effect = false;
    charge_time += charge_add_time * elapsed_time;
    ChargeAcceleration(elapsed_time);
    //“ËiŠÔ‚ğ’´‚¦‚½‚ç‚»‚ê‚¼‚ê‚Ì‘JˆÚ‚É‚Æ‚Ô
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
            //“G‚É“–‚½‚Á‚ÄUŒ‚ƒ{ƒ^ƒ“(“Ëiƒ{ƒ^ƒ“)‚ğ‰Ÿ‚µ‚½‚çˆêŒ‚–Ú
                TransitionAttackType1(0);
        }
    }
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    ChargeAcceleration(elapsed_time);
    //¡UŒ‚‚µ‚Ä‚¢‚é“G‚ª€‚ñ‚Å‚¢‚½‚çŸ‚ÌUŒ‚‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B && target_enemy != nullptr && target_enemy->fGetIsAlive() == false)
    {
        attack_time = 0;
        TransitionAttackType2(0);
    }
    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //—P—\ŠÔ‚ğ’´‚¦‚½‚ç‘Ò‹@‚É‘JˆÚ
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //—P—\ŠÔ‚æ‚è‚à‘‚­‰Ÿ‚µ‚½‚çUŒ‚2Œ‚–Ú‚É‘JˆÚ
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionAttackType2(0);
        }
    }
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);

}

void Player::AttackType2Update(float elapsed_time, SkyDome* sky_dome)
{
    ChargeAcceleration(elapsed_time);

    //¡UŒ‚‚µ‚Ä‚¢‚é“G‚ª€‚ñ‚Å‚¢‚½‚çŸ‚ÌUŒ‚‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B && target_enemy != nullptr && target_enemy->fGetIsAlive() == false)
    {
        attack_time = 0;
        TransitionAttackType3(0);

    }
    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //—P—\ŠÔ‚ğ’´‚¦‚½‚ç‘Ò‹@‚É‘JˆÚ
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //—P—\ŠÔ‚æ‚è‚à‘‚­‰Ÿ‚µ‚½‚çUŒ‚3Œ‚–Ú‚É‘JˆÚ
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionAttackType3(0);
        }
    }
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);

}

void Player::AttackType3Update(float elapsed_time, SkyDome* sky_dome)
{
    ChargeAcceleration(elapsed_time);
    //¡UŒ‚‚µ‚Ä‚¢‚é“G‚ª€‚ñ‚Å‚¢‚½‚çŸ‚ÌUŒ‚‚É‘JˆÚ
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B && target_enemy != nullptr && target_enemy->fGetIsAlive() == false)
    {
        attack_time = 0;
        TransitionCharge();

    }
    if (model->end_of_animation())
    {
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionCharge();
        }
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
    mSwordTrail.fAddTrailPoint(sword_capsule_param.start, sword_capsule_param.end);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
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
    //--------------------------ƒC[ƒWƒ“ƒO‰Á‘¬‚Ì•Ï”‰Šú‰»---------------------------------//
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
        leverage = 25.0f;
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
    start_dash_effect = true;//ƒ|ƒXƒgƒGƒtƒFƒNƒg‚ğ‚©‚¯‚é
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
