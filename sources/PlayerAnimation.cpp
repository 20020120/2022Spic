#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //�����̃N���X�̊֐��|�C���^���Ă�
    (this->*player_activity)(elapsed_time, sky_dome);

}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //�ړ��ɑJ��
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //����ɑJ��
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        TransitionAvoidance();
    }
    //�ːi�J�n�ɑJ��
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //�ړ����͂��Ȃ��Ȃ�����ҋ@�ɑJ��
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //����ɑJ��
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        TransitionAvoidance();
    }
    //�ːi�J�n�ɑJ��
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }

}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    AvoidanceAcceleration(elapsed_time);
    //����̃A�j���[�V�������I�������
    if (model->end_of_animation())
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
    //���b�N�I�����̓^�[�Q�b�g�Ɍ������ēːi����
    if (is_lock_on)
    {
        ChargeAcceleration(elapsed_time);
    }
    else
    {

    }
    //�ːi���Ԃ𒴂����炻�ꂼ��̑J�ڂɂƂ�
    if (charge_time > CHARGE_MAX_TIME)
    {
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
    else
    {
        if (is_enemy_hit)
        {
            //�G�ɓ������čU���{�^��(�ːi�{�^��)����������ꌂ��
                TransitionAttackType1(0);
        }
    }
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
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
            TransitionAttackType2(0);
        }
    }
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
