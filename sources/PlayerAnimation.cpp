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
    //�ːi���Ԃ𒴂����炻�ꂼ��̑J�ڂɂƂ�
    if (charge_time > CHARGE_MAX_TIME)
    {
        velocity = {};

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

    //�ݒ肵�����������Ԃ��������炻�ꂼ��̍s���ɑJ�ڂ���
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
    //�{�^������
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT)TransitionAwaking();//�R���{�J�E���g���ő�̂Ƃ��͊o����ԂɂȂ�
    }
    if (is_awakening &&combo_count <= 0) TransitionInvAwaking();//�o����Ԃ̂Ƃ��ɃJ�E���g��0�ɂȂ�����ʏ��ԂɂȂ�
}

void Player::TransitionIdle(float blend_second)
{
    //�_�b�V���G�t�F�N�g�̏I��
    end_dash_effect = true;
    //�o����Ԃ̎��̑ҋ@�A�j���[�V�����ɃZ�b�g
    if(is_awakening)model->play_animation(AnimationClips::AwakingIdle, true,true,blend_second);
    //�ʏ��Ԃ̑ҋ@�A�j���[�V�����ɃZ�b�g
    else model->play_animation(AnimationClips::Idle, true,true,blend_second);
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�������x
    animation_speed = 1.0f;
    //�ҋ@��Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove(float blend_second)
{
    //�_�b�V���G�t�F�N�g�̏I��
    end_dash_effect = true;
    //�o����Ԃ̎��̈ړ��A�j���[�V�����̐ݒ�
    if(is_awakening)model->play_animation(AnimationClips::AwakingMove, true,true, blend_second);
    //�ʏ��Ԃ̎��Ɉړ��A�j���[�V�����̐ݒ�
    else model->play_animation(AnimationClips::Move, true, true,blend_second);
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�������x
    animation_speed = 1.0f;
    //�ړ���Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    //--------------------------�C�[�W���O�����̕ϐ�������---------------------------------//
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
    //�o����Ԃ̎��̉���A�j���[�V�����̐ݒ�
    if(is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false,true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����̐ݒ�
    else model->play_animation(AnimationClips::Avoidance, false,true);
    //��𒆂��ǂ����̐ݒ�
    is_avoidance = true;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�����̑��x
    animation_speed = 1.0f;
    //�����Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AvoidanceUpdate;
}

void Player::TransitionBehindAvoidance()
{
    //���ɉ�荞�ލ��W�̎擾
    BehindAvoidancePosition();
    //��𒆂��ǂ����̐ݒ�
    is_avoidance = true;
    //��荞�ނƂ��̃^�C�}�[
    behind_timer = 0;
    //��荞�݂̕⊮���[�g
    behind_late = 0;
    //�ړ����x�̏�����
    velocity = {};
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�����̑��x
    animation_speed = 1.0f;
    //�w��ɉ�荞�ނƂ��̊֐��ɐ؂�ւ���
    player_activity = &Player::BehindAvoidanceUpdate;;
}

void Player::TransitionChargeInit()
{
    //�o����Ԃ̎��̓ːi�̎n�܂�̃A�j���[�V�����ɐݒ�
   if(is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false,true);
   //�ʏ��Ԃ̎��̓ːi�̎n�܂�̃A�j���[�V�����ɐݒ�
   else model->play_animation(AnimationClips::ChargeInit, false,true);
   //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = CHARGEINIT_ANIMATION_SPEED;
    //���b�N�I�����ĂȂ��ꍇ�̃^�[�Q�b�g�̐ݒ�
    charge_point = Math::calc_designated_point(position, forward, 60.0f);
    //�ːi�̎n�܂�̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge(float blend_seconds)
{
    //�_�b�V���G�t�F�N�g�̏I��
    end_dash_effect = false;
    //�_�b�V���|�X�g�G�t�F�N�g��������
    start_dash_effect = true;
    //�o����Ԃ̎��̓ːi�A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingCharge, false, true, blend_seconds);
    //�ʏ��Ԃ̎��̓ːi�A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::Charge, false, true, blend_seconds);
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�ːi�����ǂ����̐ݒ�
    is_charge = true;
    //�A�j���[�V�����X�s�[�h�̐ݒ�
#if 1
    animation_speed = CHARGE_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.x;
#endif // 0
    //�ːi���̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::ChargeUpdate;

}

void Player::TransitionAttackType1(float blend_seconds)
{
    //�_�b�V���G�t�F�N�g�̏I��
    end_dash_effect = true;
    //�o����Ԃ̎��̂P���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType1, false, true, blend_seconds);
    //�ʏ��Ԃ̎��̂P���ڂ̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::AttackType1, false, true, blend_seconds);
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�����X�s�[�h�̐ݒ�
#if 1
    animation_speed = ATTACK1_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.y;
#endif // 0
    //�P���ڂ̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    //�o����Ԃ̎��̂Q���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType2, false, true, blend_seconds);
    //�ʏ��Ԃ̎��̂Q���ڂ̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::AttackType2, false, true, blend_seconds);
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�������x�̐ݒ�
#if 1
    animation_speed = ATTACK2_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.z;
#endif // 0
    //�Q���ڂ̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    //�o����Ԃ̎��̂R���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType3, false, true, blend_seconds);
    //�ʏ��Ԃ̎��̂R���ڂ̂̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::AttackType3, false, true, blend_seconds);
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�������x�̐ݒ�
#if 1
    animation_speed = ATTACK3_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.w;
#endif // 0
    //�R���ڂ̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AttackType3Update;
}

void Player::TransitionSpecialSurge()
{
    //��s�@���[�h�ɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::IdleWing, true,true);
    //�Q�[�W����̓ːi���ɓ��������G�̐���������
    special_surge_combo_count = 0;
    //�Q�[�W����̓ːi���ǂ����̐ݒ�
    is_special_surge = true;
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //���b�N�I�����ĂȂ��ꍇ�̃^�[�Q�b�g�̐ݒ�
    charge_point = Math::calc_designated_point(position, forward, 10.0f);
    //�Q�[�W����̓ːi�̈ړ����x��ݒ�
    SpecialSurgeAcceleration();
    //�R���{�J�E���g�̏���
    combo_count -= 10.0f;
    //�R���{�J�E���g�̐���
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);
    //�Q�[�W����̓ːi�̃^�C�}�[
    special_surge_timer = 0.0f;
    //�A�j���[�V�����X�s�[�h�̐ݒ�
    animation_speed = 1.0f;
    //�Q�[�W����̓ːi�̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::SpecialSurgeUpdate;
}

void Player::TransitionOpportunity()
{
    //�Q�[�W����̓ːi�U���I��
    is_special_surge = false;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�������������̌o�ߎ��Ԃ����Z�b�g
    special_surge_timer = 0;
    //�A�j���[�V�����X�s�[�h�̐ݒ�
    animation_speed = 1.0f;
    //�Q�[�W����̓ːi�̌��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::OpportunityUpdate;
}

void Player::TransitionDamage()
{
    //�_�b�V���G�t�F�N�g�̏I��
    start_dash_effect = false;
    end_dash_effect = false;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�o����Ԃ̎��̃_���[�W�A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingDamage, false, true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::Damage, false, true);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�_���[�W�󂯂��Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::DamageUpdate;
}

void Player::TransitionTransformHum()
{
    //�l�^�ɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::TransformHum, false,true);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = TRANSFORM_HUM_ANIMATION_SPEED;
    //�l�^�ɂȂ��Ă�Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::TransformHumUpdate;
}

void Player::TransitionTransformWing()
{
    //��s�@���[�h�ɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::TransformWing, false,true);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = TRANSFORM_WING_ANIMATION_SPEED;
    //��s�@���[�h���̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::TransformWingUpdate;
}

void Player::TransitionAwaking()
{
    //�o����ԂɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::Awaking, false,true);
    //�o����Ԃ��ǂ����̐ݒ�
    is_awakening = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�o����ԂɂȂ�r���̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AwakingUpdate;
}

void Player::TransitionInvAwaking()
{
    //�ʏ��Ԃɖ߂�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::InvAwaking, false,true);
    //�o����Ԃ��ǂ����̐ݒ�
    is_awakening = false;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�ʏ��Ԃɖ߂��Ă�Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::InvAwakingUpdate;

}
