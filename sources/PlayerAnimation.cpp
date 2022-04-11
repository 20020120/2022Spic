#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //�����̃N���X�̊֐��|�C���^���Ă�
    (this->*player_activity)(elapsed_time, sky_dome);

}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    //�ړ��ɑJ��
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //����ɑJ��
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
        if (is_lock_on && length < BEHIND_LANGE)
        {
            TransitionBehindAvoidance();//�v���g�i�K�ł͉�荞�݉���͏����Ă���
        }
        //��������Ȃ������畁�ʂ̉��
        else TransitionAvoidance();
    }
    //�ːi�J�n�ɑJ��
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
    //�Q�[�W����̓ːi
    if (game_pad->get_button_down() & GamePad::BTN_Y)
    {
        if (combo_count >= 10.0f)
        {
            TransitionTransformWing();
        }
    }

    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time);
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    //�ړ����͂��Ȃ��Ȃ�����ҋ@�ɑJ��
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //����ɑJ��
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
        if (is_lock_on && length < BEHIND_LANGE)
        {
            TransitionBehindAvoidance();
        }
        //��������Ȃ������畁�ʂ̉��
        else TransitionAvoidance();
    }
    //�ːi�J�n�ɑJ��
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
    //�Q�[�W����̓ːi
    if (game_pad->get_button_down() & GamePad::BTN_Y)
    {
        if (combo_count >= 10.0f)
        {
            TransitionTransformWing();
        }
    }
    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time);
}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    AvoidanceAcceleration(elapsed_time);
    //����̃A�j���[�V�������I�������
    if (avoidance_boost_time > avoidance_easing_time && model->end_of_animation())
    {
        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
        else
        {
            TransitionIdle();
        }
        Awaiking();
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
    //�ːi���Ԃ𒴂����炻�ꂼ��̑J�ڂɂƂ�
    if (charge_time > CHARGE_MAX_TIME)
    {
        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            is_charge = false;
            TransitionMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
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
            //�G�ɓ������čU���{�^��(�ːi�{�^��)����������ꌂ��
            is_charge = false;
            TransitionAttackType1(0);
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
    end_dash_effect = false;
    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //�P�\���Ԃ𒴂�����ҋ@�ɑJ��
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //�P�\���Ԃ���������������U��2���ڂɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            if (enemy_length > 10.0f)
            {
                TransitionCharge();
            }
            else
            {
                TransitionAttackType2(0);
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
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::AttackType2Update(float elapsed_time, SkyDome* sky_dome)
{

    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //�P�\���Ԃ𒴂�����ҋ@�ɑJ��
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //�P�\���Ԃ���������������U��3���ڂɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            if (enemy_length > 10.0f)
            {
                TransitionCharge();
            }
            else
            {
                TransitionAttackType3(0);
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
        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            TransitionMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
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
    model->update_animation(elapsed_time * ATTACK_ANIMATION_SPEED);
}

void Player::SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    special_surge_timer += 1.0f * elapsed_time;
    SpecialSurgeAcceleration(elapsed_time);

    if (special_surge_timer > 2.0f)
    {
        TransitionOpportunity();
    }
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time);
}

void Player::OpportunityUpdate(float elapsed_time, SkyDome* sky_dome)
{
    ////���̃J�E���g��������Ό����Ȃ��Ȃ��Ă���
    ////special_surge_combo_count
    ////�����łǂꂾ���ق��̓����ɑJ�ڂ���̂ɕK�v�ȑ҂����Ԃ����߂�
    //switch (static_cast<int>(special_surge_combo_count / 5))
    //{
    //case 0:
    //    special_surge_opportunity = 1.5f;
    //    break;
    //case 1:
    //    special_surge_opportunity = 1.0f;
    //    break;
    //case 2:
    //    special_surge_opportunity = 0.5f;
    //    break;
    //default:
    //    special_surge_opportunity = 0;
    //    break;
    //}
    special_surge_opportunity = 1.0f;
    opportunity_timer += 1.0f * elapsed_time;
    //�ݒ肵�����������Ԃ��������炻�ꂼ��̍s���ɑJ�ڂ���
    if (opportunity_timer > special_surge_opportunity)
    {
        special_surge_combo_count = 0;
        TransitionTransformHum();
    }
        UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
        model->update_animation(elapsed_time);
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
        //�ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        else
        {
            TransitionIdle(0);
        }
        //����ɑJ��
        float length{ Math::calc_vector_AtoB_length(position, target) };
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE)
            {
                TransitionBehindAvoidance();//�v���g�i�K�ł͉�荞�݉���͏����Ă���
            }
            //��������Ȃ������畁�ʂ̉��
            else TransitionAvoidance();
        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionChargeInit();
        }

    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * 2.0f);

}

void Player::TransformWingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionSpecialSurge();
    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    model->update_animation(elapsed_time * 2.0f);
}

void Player::AwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation()) TransitionIdle();
    model->update_animation(elapsed_time);
}

void Player::InvAwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation()) TransitionIdle(0);
    model->update_animation(elapsed_time);
}

void Player::Awaiking()
{
    //�{�^������
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT)TransitionAwaking();//�R���{�J�E���g���ő�̂Ƃ��͊o����ԂɂȂ�
    }
    if (is_awakening &&combo_count <= 0) TransitionInvAwaking();//�o����Ԃ̂Ƃ��ɃJ�E���g��0�ɂȂ�����ʏ��ԂɂȂ�
}

void Player::TransitionIdle(float blend_second)
{
    end_dash_effect = true;
    if(is_awakening)model->play_animation(AnimationClips::AwakingIdle, true,blend_second);
    else model->play_animation(AnimationClips::Idle, true,blend_second);
    is_attack = false;
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove()
{
    end_dash_effect = true;
    if(is_awakening)model->play_animation(AnimationClips::AwakingMove, true);
    else model->play_animation(AnimationClips::Move, true);
    is_attack = false;
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    //--------------------------�C�[�W���O�����̕ϐ�������---------------------------------//
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
    if(is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false);
    else model->play_animation(AnimationClips::Avoidance, false);
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
   if(is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false);
   else model->play_animation(AnimationClips::ChargeInit, false);
    is_attack = true;
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge()
{
    end_dash_effect = false;
    start_dash_effect = true;//�|�X�g�G�t�F�N�g��������
    if (is_awakening)model->play_animation(AnimationClips::AwakingCharge, false, 0);
    else model->play_animation(AnimationClips::Charge, false, 0);
    is_attack = true;
    charge_point = Math::calc_designated_point(position, forward, 60.0f);
    is_charge = true;
    player_activity = &Player::ChargeUpdate;

}

void Player::TransitionAttackType1(float blend_seconds)
{
    end_dash_effect = true;
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType1, false, 0);
    else model->play_animation(AnimationClips::AttackType1, false, 0);
    is_attack = true;
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType2, false, 0);
    else model->play_animation(AnimationClips::AttackType1, false, 0);
    is_attack = true;
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType3, false, 0);
    else model->play_animation(AnimationClips::AttackType3, false, 0);
    is_attack = true;
    player_activity = &Player::AttackType3Update;
}

void Player::TransitionSpecialSurge()
{
    model->play_animation(AnimationClips::IdleWing, true);
    special_surge_combo_count = 0;//�Q�[�W����̓ːi���ɓ��������G�̐������������Ă���
    is_special_surge = true;
    is_attack = true;
    charge_point = Math::calc_designated_point(position, forward, 200.0f);
    player_activity = &Player::SpecialSurgeUpdate;
    combo_count -= 10.0f;
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);
    special_surge_timer = 0.0f;
}

void Player::TransitionOpportunity()
{
    is_special_surge = false;//�ːi�U���I��
    is_attack = false;
    special_surge_timer = 0;//�������������̌o�ߎ��Ԃ����Z�b�g
    player_activity = &Player::OpportunityUpdate;
}

void Player::TransitionDamage()
{
    start_dash_effect = false;
    end_dash_effect = false;
    is_attack = false;
    player_activity = &Player::DamageUpdate;
}

void Player::TransitionTransformHum()
{
    model->play_animation(AnimationClips::TransformHum, false);
    player_activity = &Player::TransformHumUpdate;
}

void Player::TransitionTransformWing()
{
    model->play_animation(AnimationClips::TransformWing, false);
    player_activity = &Player::TransformWingUpdate;
}

void Player::TransitionAwaking()
{
    model->play_animation(AnimationClips::Awaking, false);
    is_awakening = true;
    player_activity = &Player::AwakingUpdate;
}

void Player::TransitionInvAwaking()
{
    model->play_animation(AnimationClips::InvAwaking, false);
    is_awakening = false;
    player_activity = &Player::InvAwakingUpdate;

}
