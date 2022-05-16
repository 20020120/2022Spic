#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time)
{
    (this->*player_title_activity)(elapsed_time);
}
void Player::UpdateTitleAnimationReadyIdle(float elaosed_time)
{
    if (start_title_animation)
    {
        model->play_animation(AnimationClips::TitleAnimationStart);
        player_title_activity = &Player::UpdateTitleAnimationStart;
    }
}
void Player::UpdateTitleAnimationStart(float elaosed_time)
{
    if (model->end_of_animation())
    {
        model->play_animation(AnimationClips::TitleAnimationStartIdle);
        player_title_activity = &Player::UpdateTitleAnimationStartIdle;
    }
}
void Player::UpdateTitleAnimationStartIdle(float elaosed_time)
{
    title_timer += 1.0f * elaosed_time;
    //���f���̃A�j���[�V�������I�����������1�b��������
    if (model->end_of_animation() && title_timer > 0.5f)
    {
        title_timer = 0;
        model->play_animation(AnimationClips::TitleAnimationEnd);
        player_title_activity = &Player::UpdateTitleAnimationEnd;
    }
}
void Player::UpdateTitleAnimationEnd(float elaosed_time)
{
    if (model->end_of_animation())
    {
        model->play_animation(AnimationClips::TitleAnimationEndIdle);
        player_title_activity = &Player::UpdateTitleAnimationEndIdle;
    }
}
void Player::UpdateTitleAnimationEndIdle(float elaosed_time)
{
    title_timer += 1.0f * elaosed_time;
    if(title_timer > 0.5) end_title_animation = true;

    if (model->end_of_animation())
    {
    }
}
void Player::TransitionTitleAnimationReadyIdle()
{
    model->play_animation(AnimationClips::TitleAnimationReadyIdle,true);
    //������update���Ă����Ȃ���1�t���[�����������̃A�j���[�V�������f���Ă��܂�����
    model->update_animation(1.0f);
    //�^�C�g���̓��b�V�����B���Ȃ�����0�ɂ��Ă���
    threshold_mesh = 0.0f;
}
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies, GraphicsPipeline& Graphics_)
{
    switch (behavior_state)
    {
    case Player::Behavior::Normal:
        //�����̃N���X�̊֐��|�C���^���Ă�
        (this->*player_activity)(elapsed_time, sky_dome);
        break;
    case Player::Behavior::Chain:
        (this->*player_chain_activity)(elapsed_time, enemies,Graphics_);
        break;
    default:
        break;
    }
}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
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
        if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
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

    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
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
        if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
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

    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //�G�t�F�N�g�̈ʒu�C��]�ݒ�
    player_air_registance_effec->set_position(effect_manager->get_effekseer_manager(),position);
    player_air_registance_effec->set_quaternion(effect_manager->get_effekseer_manager(), orientation);

    AvoidanceAcceleration(elapsed_time);
    //����̃A�j���[�V�������I�������
    if (avoidance_boost_time > avoidance_easing_time && model->end_of_animation())
    {
        player_air_registance_effec->stop(effect_manager->get_effekseer_manager());
        //��𒆂��ǂ����̐ݒ�
        is_avoidance = false;
        is_behind_avoidance = false;
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
    //behind_timer += 2.0f * elapsed_time;
    player_behind_effec->set_position(effect_manager->get_effekseer_manager(), { position.x,position.y + air_registance_offset_y ,position.z });
    //BehindAvoidanceMove(elapsed_time);
    if (BehindAvoidanceMove(elapsed_time, behind_transit_index,position,100.0f, behind_interpolated_way_points,1.0f))
    {
        player_behind_effec->stop(effect_manager->get_effekseer_manager());
        //��𒆂��ǂ����̐ݒ�
        is_avoidance = false;
        is_behind_avoidance = false;
        //�W���X�g����̃t���O��������
        is_just_avoidance = false;
        TransitionIdle();
    }
    else
    {
        is_lock_on = true;
    }
    UpdateBehindAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionCharge(attack_animation_blends_speeds.x);
    }
    //ChargeAcceleration(elapsed_time);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    start_dash_effect = false;
    charge_time += charge_add_time * elapsed_time;
    //ChargeAcceleration(elapsed_time);
    //�U���̉����̐ݒ�
    SetAccelerationVelocity();
    //�ːi���Ԃ𒴂����炻�ꂼ��̑J�ڂɂƂ�
    if (charge_time > CHARGE_MAX_TIME)
    {
        end_dash_effect = true;
        velocity.x *= 0.2f;
        velocity.y *= 0.2f;
        velocity.z *= 0.2f;

        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            is_charge = false;
            charge_change_direction_count = CHARGE_DIRECTION_COUNT;
            TransitionMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
        else
        {
            charge_time = 0;
            is_charge = false;
            charge_change_direction_count = CHARGE_DIRECTION_COUNT;
            TransitionIdle();
        }
        Awaiking();
    }
    else
    {
        if (is_enemy_hit)
        {
            end_dash_effect = true;
            //�G�ɓ������čU���{�^��(�ːi�{�^��)����������ꌂ��
            is_charge = false;
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            charge_change_direction_count = CHARGE_DIRECTION_COUNT;
            TransitionAttackType1(attack_animation_blends_speeds.y);
        }
        if (is_lock_on == false && charge_change_direction_count > 0)
        {
            if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
            {
                charge_change_direction_count--;
                velocity = {};
                DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
                if ((movevec.x * movevec.x) + (movevec.z * movevec.z) > 0)
                {
                    ChargeTurn(elapsed_time, movevec, turn_speed, position, orientation);
                    charge_point = Math::calc_designated_point(position, movevec, 200.0f);
                }
                else
                {
                    ChargeTurn(elapsed_time, forward, turn_speed, position, orientation);
                    charge_point = Math::calc_designated_point(position, forward, 200.0f);
                }
               //SetAccelerationVelocity();
                charge_time = 0;
                //TransitionCharge();
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
#if 0
            if (enemy_length > 20.0f)
            {
                TransitionChargeInit();
            }
            else
            {
                TransitionAttackType2(attack_animation_blends_speeds.z);
            }
#else
            if (target_enemy != nullptr && target_enemy->fGetPercentHitPoint() != 0)
            {
                if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
                {
                    attack_time = 0;
                    velocity.x *= 0.2f;
                    velocity.y *= 0.2f;
                    velocity.z *= 0.2f;
                    TransitionAttackType2(attack_animation_blends_speeds.z);
                }
            }
            else   TransitionIdle();

#endif // 0
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
    attack_time += attack_add_time * elapsed_time;
    //�G�ɓ������������Ԃ�2�b��������������I���
#if 0
    if (is_update_animation == false && (is_enemy_hit || attack_time >= 1.0f))
    {
        is_charge = false;
        attack_time = 0;
        is_update_animation = true;
    }
    else
    {
        float length{ Math::calc_vector_AtoB_length(position,target) };

        //if (length > 5.0f)ChargeAcceleration(elapsed_time);
        SetAccelerationVelocity();
    }
#else
    if (is_update_animation == false)
    {
        SetAccelerationVelocity();
        if (is_enemy_hit)
        {
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            is_charge = false;
            attack_time = 0;
            is_update_animation = true;
        }
        else if (attack_time >= 2.0f)
        {
            is_charge = false;
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            attack_time = 0;
            TransitionIdle();
        }

    }

#endif // 0
    if (model->end_of_animation())
    {
        //�P�\���Ԃ𒴂�����ҋ@�ɑJ��
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            attack_time = 0;
            TransitionIdle();
        }
        //�P�\���Ԃ���������������U��3���ڂɑJ��
#if 0
            if (enemy_length > 0.0f)
            {
                TransitionChargeInit();
            }
            else
            {
                TransitionAttackType3(attack_animation_blends_speeds.w);
            }
#else
            if (target_enemy != nullptr && target_enemy->fGetPercentHitPoint() != 0)
            {
                if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
                {
                    velocity.x *= 0.2f;
                    velocity.y *= 0.2f;
                    velocity.z *= 0.2f;
                    attack_time = 0;
                    TransitionAttackType3(attack_animation_blends_speeds.w);
                }
            }
            else
            {
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                TransitionIdle();
            }
#endif // 0
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

    attack_time += attack_add_time * elapsed_time;
    //�G�ɓ������������Ԃ�2�b��������������I���

#if 0
    if (is_update_animation == false && (is_enemy_hit || attack_time >= 1.0f))
    {
        is_charge = false;
        attack_time = 0;
        is_update_animation = true;
    }
    else
    {
        float length{ Math::calc_vector_AtoB_length(position,target) };
        //if (length > 5.0f) ChargeAcceleration(elapsed_time);
        SetAccelerationVelocity();
    }
#else
    if (is_update_animation == false)
    {
        SetAccelerationVelocity();
        if (is_enemy_hit)
        {
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            is_charge = false;
            attack_time = 0;
            is_update_animation = true;
        }
        else if (attack_time >= 2.0f)
        {
            is_charge = false;
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            attack_time = 0;
            TransitionIdle();
        }
    }

#endif // 0

    if (model->end_of_animation())
    {
        if (target_enemy != nullptr && target_enemy->fGetPercentHitPoint() != 0)
        {
            if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
            {
                attack_time = 0;
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                TransitionAttackType1(attack_animation_blends_speeds.x);
            }
        }
        else
        {
            //�ړ����͂���������ړ��ɑJ��
            if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
            {
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                charge_time = 0;
                TransitionMove();
            }
            //�ړ����͂��Ȃ�������ҋ@�ɑJ��
            else
            {
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                charge_time = 0;
                TransitionIdle();
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

void Player::SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    special_surge_timer += 1.0f * elapsed_time;

    if (special_surge_timer > 1.0f)
    {
        velocity = {};
        TransitionOpportunity();
    }
    UpdateSpecialSurgeVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
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
            TransitionIdle();
        }
        //����ɑJ��
        float length{ Math::calc_vector_AtoB_length(position, target) };
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
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
    //���b�N�I�����Ă���G�̕�������
    UpdateRotateToTarget(elapsed_time, position, orientation, camera_forward, camera_position);
   //UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
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

void Player::InvAwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
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

void Player::StageMoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    model->update_animation(elapsed_time);
}

void Player::WingDashStartUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::WingDashIdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::WingDashEndUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::DieUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionDying();
    }
}

void Player::DyingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    is_dying_update = true;
    threshold += 1.0f * elapsed_time;
    threshold_mesh += 1.0f * elapsed_time;
    if (threshold > 1.0f && threshold_mesh > 1.0f)
    {
        is_alive = false;
    }
}

void Player::StartMothinUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::NamelessMotionUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionNamelessMotionIdle();
    }
}

void Player::NamelessMotionIdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        //�N���A���[�V�������I��������Ƃ�`����
        is_end_clear_motion = true;
    }
}

void Player::Awaiking()
{
    //�{�^������
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT - 5.0f)TransitionAwaking();//�R���{�J�E���g���ő�̂Ƃ��͊o����ԂɂȂ�
    }
    if (is_awakening &&combo_count <= 0) TransitionInvAwaking();//�o����Ԃ̂Ƃ��ɃJ�E���g��0�ɂȂ�����ʏ��ԂɂȂ�
}

void Player::TransitionIdle(float blend_second)
{
    //�N���A���o���Ȃ��������
    if (during_clear) during_clear = false;
    condition_state = ConditionState::Alive;
    //�_�b�V���G�t�F�N�g�̏I��
    //end_dash_effect = true;
    //�o����Ԃ̎��̑ҋ@�A�j���[�V�����ɃZ�b�g
    if(is_awakening)model->play_animation(AnimationClips::AwakingIdle, true,true,blend_second);
    //�ʏ��Ԃ̑ҋ@�A�j���[�V�����ɃZ�b�g
    else model->play_animation(AnimationClips::Idle, true,true,blend_second);
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�������x
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�ҋ@��Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove(float blend_second)
{
    //�_�b�V���G�t�F�N�g�̏I��
    //end_dash_effect = true;
    //�o����Ԃ̎��̈ړ��A�j���[�V�����̐ݒ�
    if(is_awakening)model->play_animation(AnimationClips::AwakingMove, true,true, blend_second);
    //�ʏ��Ԃ̎��Ɉړ��A�j���[�V�����̐ݒ�
    else model->play_animation(AnimationClips::Move, true, true,blend_second);
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�������x
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�ړ���Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    //�G�t�F�N�g�Đ�
    player_air_registance_effec->play(effect_manager->get_effekseer_manager(), position,0.3f);
    //��𒆂��ǂ����̐ݒ�
    is_avoidance = true;
    //��荞�݉�����ǂ���
    is_behind_avoidance = false;
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
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�����̑��x
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�����Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AvoidanceUpdate;
}

void Player::TransitionBehindAvoidance()
{
    if (is_just_avoidance_capsul)
    {
        //���b�N�I�����Ă���G���X�^��������
        if (target_enemy != nullptr)
        {
            target_enemy->fSetStun(true, true);
        }
        is_just_avoidance = true;
    }
    else
    {
        if (target_enemy != nullptr)
        {
            //���b�N�I�����Ă���G���X�^��������
            target_enemy->fSetStun(true);
        }
    }
    player_behind_effec->play(effect_manager->get_effekseer_manager(), {position.x,position.y + air_registance_offset_y ,position.z});
    velocity = {};
    //��𒆂��ǂ����̐ݒ�
    is_avoidance = true;
    //��荞�݉�����ǂ���
    is_behind_avoidance = true;
    //�o����Ԃ̎��̉���A�j���[�V�����̐ݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false, true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����̐ݒ�
    else model->play_animation(AnimationClips::Avoidance, false, true);
    //���ɉ�荞�ލ��W�̎擾
    BehindAvoidancePosition();
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
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�w��ɉ�荞�ނƂ��̊֐��ɐ؂�ւ���
    player_activity = &Player::BehindAvoidanceUpdate;
}

void Player::TransitionChargeInit()
{
    //�o����Ԃ̎��̓ːi�̎n�܂�̃A�j���[�V�����ɐݒ�
   if(is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false,true);
   //�ʏ��Ԃ̎��̓ːi�̎n�܂�̃A�j���[�V�����ɐݒ�
   else model->play_animation(AnimationClips::ChargeInit, false,true);
   //�U�������ǂ����̐ݒ�
   is_attack = true;
   //�ːi�����ǂ����̐ݒ�
   is_charge = true;
   //�A�j���[�V�������x�̐ݒ�
    animation_speed = CHARGEINIT_ANIMATION_SPEED;
    //���b�N�I�����ĂȂ��ꍇ�̃^�[�Q�b�g�̐ݒ�
    charge_point = Math::calc_designated_point(position, forward, 200.0f);
    //�����̃��[�g
    lerp_rate = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�ːi�̎n�܂�̎��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge(float blend_seconds)
{
    //�_�b�V���|�X�g�G�t�F�N�g��������
    start_dash_effect = true;
    //�o����Ԃ̎��̓ːi�A�j���[�V�����ɐݒ�
    if (is_awakening)
    {
        //�v���C���[�̍U����
        player_attack_power = 1;
        model->play_animation(AnimationClips::AwakingCharge, false, true, blend_seconds);
    }
    //�ʏ��Ԃ̎��̓ːi�A�j���[�V�����ɐݒ�
    else
    {
        //�v���C���[�̍U����
        player_attack_power = 1;
        model->play_animation(AnimationClips::Charge, false, true, blend_seconds);
    }
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
    //charge_point = Math::calc_designated_point(position, forward, 60.0f);
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�����̃��[�g
    lerp_rate = 4.0f;
    //�U���̉����̐ݒ�
    //SetAccelerationVelocity();
    //�ːi���̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::ChargeUpdate;

}

void Player::TransitionAttackType1(float blend_seconds)
{
    //�o����Ԃ̎��̂P���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)
    {
        //�v���C���[�̍U����
        player_attack_power = 4;
        model->play_animation(AnimationClips::AwakingAttackType1, false, true, blend_seconds);
    }
    //�ʏ��Ԃ̎��̂P���ڂ̃A�j���[�V�����ɐݒ�
    else
    {
        //�v���C���[�̍U����
        player_attack_power = 2;
        model->play_animation(AnimationClips::AttackType1, false, true, blend_seconds);
    }
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�����X�s�[�h�̐ݒ�
#if 1
    animation_speed = ATTACK1_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.y;
#endif // 0
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�����̃��[�g
    lerp_rate = 4.0f;
    //�P���ڂ̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    //�o����Ԃ̎��̂Q���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)
    {
        //�v���C���[�̍U����
        player_attack_power = 6;
        model->play_animation(AnimationClips::AwakingAttackType2, false, true, blend_seconds);
    }
    //�ʏ��Ԃ̎��̂Q���ڂ̃A�j���[�V�����ɐݒ�
    else
    {
        //�v���C���[�̍U����
        player_attack_power = 4;
        model->play_animation(AnimationClips::AttackType2, false, true, blend_seconds);
    }
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�������x�̐ݒ�
#if 1
    animation_speed = ATTACK2_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.z;
#endif // 0
    //�U���̉����̐ݒ�
    //SetAccelerationVelocity();
        //���b�N�I�����ĂȂ��ꍇ�̃^�[�Q�b�g�̐ݒ�
    charge_point = Math::calc_designated_point(position, forward, 200.0f);
    //�����̃��[�g
    lerp_rate = 2.0f;
    //�U���̎���
    attack_time = 0;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = false;
    //�ːi�����ǂ����̐ݒ�
    is_charge = true;
    //�Q���ڂ̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    //�o����Ԃ̎��̂R���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)
    {
        //�v���C���[�̍U����
        player_attack_power = 9;
        model->play_animation(AnimationClips::AwakingAttackType3, false, true, blend_seconds);
    }
    //�ʏ��Ԃ̎��̂R���ڂ̂̃A�j���[�V�����ɐݒ�
    else
    {
        //�v���C���[�̍U����
        player_attack_power = 5;
        model->play_animation(AnimationClips::AttackType3, false, true, blend_seconds);
    }
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�A�j���[�V�������x�̐ݒ�
#if 1
    animation_speed = ATTACK3_ANIMATION_SPEED;
#else
    //�f�o�b�O�p
    animation_speed = attack_animation_speeds.w;
#endif // 0
    //�U���̉����̐ݒ�
    //SetAccelerationVelocity();
    //�����̃��[�g
    lerp_rate =2.0f;
    //�U���̎���
    attack_time = 0;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = false;
    //�ːi�����ǂ����̐ݒ�
    is_charge = true;
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
    charge_point = Math::calc_designated_point(position, forward, 100.0f);
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
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
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
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�Q�[�W����̓ːi�̌��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::OpportunityUpdate;
}

void Player::TransitionDamage()
{
    //�_�b�V���G�t�F�N�g�̏I��
    start_dash_effect = false;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�o����Ԃ̎��̃_���[�W�A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingDamage, false, true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::Damage, false, true);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�_���[�W�󂯂��Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::DamageUpdate;
}

void Player::TransitionTransformHum()
{
    //�l�^�ɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::TransformHum, false,true);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = TRANSFORM_HUM_ANIMATION_SPEED;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�l�^�ɂȂ��Ă�Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::TransformHumUpdate;
}

void Player::TransitionTransformWing()
{
    velocity = {};
    //��s�@���[�h�ɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::TransformWing, false,true);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = TRANSFORM_WING_ANIMATION_SPEED;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
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
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
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
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�ʏ��Ԃɖ߂��Ă�Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::InvAwakingUpdate;

}

void Player::TransitionWingDashStart()
{
}

void Player::TransitionWingDashIdle()
{
}

void Player::TransitionWingDashEnd()
{
}
void Player::TransitionDie()
{
    condition_state = ConditionState::Dye;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    velocity = {};
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�o����Ԃ̎��̃_���[�W�A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingDie, false, true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::Die, false, true);
    //�X�V�֐��ɐ؂�ւ�
    player_activity = &Player::DieUpdate;
    if (GameFile::get_instance().get_vibration())game_pad->set_vibration(1.0f, 1.0f, 1.0f);
}

void Player::TransitionDying()
{
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    velocity = {};
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�o����Ԃ̎��̃_���[�W�A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingDying, true, true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::Dying, true, true);
    //�X�V�֐��ɐ؂�ւ�
    player_activity = &Player::DyingUpdate;

}

void Player::TransitionNamelessMotionIdle()
{
    PostEffect::clear_post_effect();
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    velocity = {};
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::NamelessMotionIdle, false, true);
    //�X�V�֐��ɐ؂�ւ�
    player_activity = &Player::NamelessMotionIdleUpdate;

}

void Player::TransitionStartMothin()
{
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    velocity = {};
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V�����ɐݒ�
    //model->play_animation(AnimationClips::StartMothin, false, true);
    //�X�V�֐��ɐ؂�ւ�
    player_activity = &Player::StartMothinUpdate;

}

void Player::TransitionNamelessMotion()
{
    PostEffect::wipe_effect(0.15f);
    //�N���A�p���[�V�������n�܂����炩��true�ɂ���
    is_start_cleear_motion = true;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    velocity = {};
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::NamelessMotion, false, true);
    //�X�V�֐��ɐ؂�ւ�
    player_activity = &Player::NamelessMotionUpdate;
}

void Player::TransitionStageMove()
{

    //�ړ��̃A�j���[�V�����ɂ���(���)
    model->play_animation(AnimationClips::Charge, false);
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    velocity = {};
    //�ʏ��Ԃɖ߂��Ă�Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_activity = &Player::StageMoveUpdate;
    during_clear = true;
}
