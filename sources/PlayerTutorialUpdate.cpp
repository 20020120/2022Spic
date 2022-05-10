#include"Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"

void Player::UpdateTutorial(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    ExecFuncTutorialUpdate(elapsed_time, sky_dome, enemies);
    switch (behavior_state)
    {
    case Player::Behavior::Normal:
        player_attack_power = 3;
        //��荞�݉�������i��ł�����؂�ւ����
        if (tutorial_state > TutorialState::BehindAvoidanceTutorial)
        {
            if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
            {
                transition_chain_behavior();
            }
        }
        //�`���[�g���A�������b�N�I���̎������傫����Ώo����
        if (tutorial_state >= TutorialState::LockOnTutorial)     TutorialLockOn();
        //�J�������Z�b�g
        CameraReset();
        break;
    case Player::Behavior::Chain:

        break;
    default:
        break;
    }

    //�A�j���[�V�����X�V����
    GetPlayerDirections();
    //�v���C���[�̃p�����[�^�̕ύX
    InflectionParameters(elapsed_time);
    if (is_awakening)
    {
        for (int i = 0; i < 2; ++i)
        {
            mSwordTrail[i].fUpdate(elapsed_time, 10);
            mSwordTrail[i].fEraseTrailPoint(elapsed_time);
        }
    }
    else
    {
        mSwordTrail[0].fUpdate(elapsed_time, 10);
        mSwordTrail[0].fEraseTrailPoint(elapsed_time);
    }
    LerpCameraTarget(elapsed_time);
    player_config->update(graphics, elapsed_time);

    if(is_update_animation)model->update_animation(elapsed_time * animation_speed);
#ifdef USE_IMGUI
    static bool display_scape_imgui;
    imgui_menu_bar("Player", "Player", display_scape_imgui);
    if (display_scape_imgui)
    {
        if (ImGui::Begin("Player"))
        {
            if (ImGui::TreeNode("transform"))
            {
                ImGui::DragFloat3("position", &position.x);
                ImGui::DragFloat3("scale", &scale.x, 0.001f);
                ImGui::DragFloat4("orientation", &orientation.x);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("PlayerDirection"))
            {
                ImGui::DragFloat3("forward", &forward.x);
                ImGui::DragFloat3("right", &right.x);
                ImGui::DragFloat3("up", &up.x);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("speed"))
            {
                ImGui::DragFloat3("velocity", &velocity.x);
                ImGui::DragFloat3("acceleration_velocity", &acceleration_velocity.x);
                ImGui::DragFloat("max_speed", &move_speed);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("PlayerFlags"))
            {
                ImGui::Checkbox("is_avoidance", &is_avoidance);
                ImGui::Checkbox("is_behind_avoidance", &is_behind_avoidance);
                ImGui::Checkbox("camera_reset", &camera_reset);
                ImGui::Checkbox("is_lock_on", &is_lock_on);
                ImGui::Checkbox("is_camera_lock_on", &is_camera_lock_on);
                ImGui::Checkbox("is_enemy_hit", &is_enemy_hit);
                ImGui::Checkbox("is_awakening", &is_awakening);
                ImGui::Checkbox("start_dash_effect", &start_dash_effect);
                ImGui::Checkbox("end_dash_effect", &end_dash_effect);
                ImGui::Checkbox("is_attack", &is_attack);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("CapsuleParam"))
            {
                if (ImGui::TreeNode("BodyCapsuleParam"))
                {
                    ImGui::DragFloat3("capsule_parm.start", &body_capsule_param.start.x, 0.1f);
                    ImGui::DragFloat3("capsule_parm.end", &body_capsule_param.end.x, 0.1f);
                    ImGui::DragFloat("body_capsule_param.rasius", &body_capsule_param.rasius, 0.1f);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("easing"))
            {
                ImGui::DragFloat("avoidance_easing_time", &avoidance_easing_time, 0.1f);
                ImGui::DragFloat("avoidance_boost_time", &avoidance_boost_time, 0.1f);
                ImGui::DragFloat("leverage", &leverage, 0.1f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("PlayerGameParm"))
            {
                ImGui::DragInt("player_health", &player_health);
                ImGui::DragFloat("combo", &combo_count);
                ImGui::DragFloat("attack_time", &attack_time);

                ImGui::DragFloat("duration_combo_timer", &duration_combo_timer);
                ImGui::DragInt("player_attack_power", &player_attack_power);
                ImGui::DragFloat("invincible_timer", &invincible_timer);
                ImGui::TreePop();
            }

            if (ImGui::Button("TransitionStageMove")) TransitionStageMove();
            if (ImGui::Button("TransitionIdle")) TransitionIdle();

            ImGui::DragFloat("threshold", &threshold, 0.01f, 0, 1.0f);
            ImGui::DragFloat("threshold_mesh", &threshold_mesh, 0.01f, 0, 1.0f);
            ImGui::DragFloat("glow_time", &glow_time);
            ImGui::DragFloat4("emissive_color", &emissive_color.x, 0.1f);
            DirectX::XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
            float length_radius = Math::calc_vector_AtoB_length(p, target);//����(���a)
            ImGui::DragFloat("l", &length_radius);

            static int state = 1;
            ImGui::SliderInt("tutorial_state", &state, 1, 7);
            tutorial_state = static_cast<TutorialState>(state);

            ImGui::DragFloat("execution_timer", &execution_timer);
            ImGui::Checkbox("is_next", &is_next_tutorial);

            ImGui::End();
        }
    }
#endif // USE_IMGUI

}


void Player::ExecFuncTutorialUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    switch (behavior_state)
    {
    case Player::Behavior::Normal:
        //�����̃N���X�̊֐��|�C���^���Ă�
        (this->*player_tutorial_activity)(elapsed_time, sky_dome, enemies);
        break;
    case Player::Behavior::Chain:
        (this->*player_chain_activity)(elapsed_time, enemies);
        break;
    default:
        break;
    }

}

void Player::ChangeTutorialState(int state)
{
    //�������̃`���[�g���A���X�e�[�g�����������l�������Ă�����
    //�ЂƂO�ɂ��ǂ邩��͂���Ȃ��悤�ɂ���
    if (tutorial_state > static_cast<TutorialState>(state) || tutorial_state == static_cast<TutorialState>(state)) return;
    tutorial_state = static_cast<TutorialState>(state);
    is_next_tutorial = false;
    execution_timer = 0;
}

void Player::TutorialIdleUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    //�ړ��ɑJ��
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionTutorialMove();
    }

    switch (tutorial_state)
    {
    case Player::TutorialState::MoveTutorial:
        //MoveTutorial�̎��ɂ��̎��ԓ����Ă�����I��������Ƃɂ���
        if (execution_timer > 1.5f) is_next_tutorial = true;
        break;
    case Player::TutorialState::AvoidanceTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        break;
    }
    case Player::TutorialState::LockOnTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        break;
    }
    case Player::TutorialState::AttackTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }
        break;
    }
    case Player::TutorialState::BehindAvoidanceTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //����ɑJ��
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //��������Ȃ������畁�ʂ̉��
            else TransitionTutorialAvoidance();

        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }
        break;
    }
    case Player::TutorialState::ChainAttackTutorial:
    {
        break;
    }
    case Player::TutorialState::AwaikingTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //����ɑJ��
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //��������Ȃ������畁�ʂ̉��
            else TransitionTutorialAvoidance();

        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }
        TutorialAwaiking();
        break;
    }
    default:
        break;
    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::TutorialMoveUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    //�ړ����͂��Ȃ��Ȃ�����ҋ@�ɑJ��
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionTutoriaIdle();
    }
    switch (tutorial_state)
    {
    case Player::TutorialState::MoveTutorial:
        execution_timer += 1.0f * elapsed_time;
        //MoveTutorial�̎��ɂ��̎��ԓ����Ă�����I��������Ƃɂ���
        if (execution_timer > 1.5f) is_next_tutorial = true;
        break;
    case Player::TutorialState::AvoidanceTutorial:
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        break;
    case Player::TutorialState::LockOnTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
    }
        break;
    case Player::TutorialState::AttackTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }
        break;
    }
    case Player::TutorialState::BehindAvoidanceTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //����ɑJ��
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //��������Ȃ������畁�ʂ̉��
            else TransitionTutorialAvoidance();

        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }
        break;
    }
    case Player::TutorialState::ChainAttackTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //����ɑJ��
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //��������Ȃ������畁�ʂ̉��
            else TransitionTutorialAvoidance();

        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }

        break;
    }
    case Player::TutorialState::AwaikingTutorial:
    {
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //����ɑJ��
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //���ɉ�荞�߂鋗���Ȃ��荞�݂悤��Update
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //��������Ȃ������畁�ʂ̉��
            else TransitionTutorialAvoidance();

        }
        //�ːi�J�n�ɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionTutorialChargeInit();
        }
        TutorialAwaiking();
        break;
    }
    default:
        break;
    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::TutorialAvoidanvceUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    AvoidanceAcceleration(elapsed_time);
    if (avoidance_boost_time > avoidance_easing_time&& model->end_of_animation())
    {
        //�����`���[�g���A��������Ȃ�
        if (tutorial_state == TutorialState::AvoidanceTutorial) is_next_tutorial = true;
            //��𒆂��ǂ����̐ݒ�
            is_avoidance = false;
            is_behind_avoidance = false;
            //�ړ����͂���������ړ��ɑJ��
            if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
            {
                TransitionTutorialMove();
            }
            //�ړ����͂��Ȃ�������ҋ@�ɑJ��
            else
            {
                TransitionTutoriaIdle();
            }
            if(tutorial_state >= TutorialState::AwaikingTutorial)   TutorialAwaiking();
         UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
    else
    {
        UpdateAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
}

void Player::TutorialBehindAvoidanceUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    if (BehindAvoidanceMove(elapsed_time, behind_transit_index, position, 100.0f, behind_interpolated_way_points, 0.2f))
    {
        //��𒆂��ǂ����̐ݒ�
        is_avoidance = false;
        is_behind_avoidance = false;
        is_just_avoidance = false;
        //�����`���[�g���A������荞�݉���Ȃ�
        if (tutorial_state == TutorialState::BehindAvoidanceTutorial) is_next_tutorial = true;
        TransitionTutoriaIdle();
    }
    UpdateBehindAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::TutorialChargeinitUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    if (model->end_of_animation())
    {
        TransitionTutorialCharge(attack_animation_blends_speeds.x);
    }
    ChargeAcceleration(elapsed_time);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::TutorialChargeUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    start_dash_effect = false;
    charge_time += charge_add_time * elapsed_time;
    ChargeAcceleration(elapsed_time);
    //�ːi���Ԃ𒴂����炻�ꂼ��̑J�ڂɂƂ�
    if (charge_time > CHARGE_MAX_TIME)
    {
        velocity = {};
        end_dash_effect = true;

        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            is_charge = false;
            TransitionTutorialMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
        else
        {
            charge_time = 0;
            is_charge = false;
            TransitionTutoriaIdle();
        }
        if (tutorial_state >= TutorialState::AwaikingTutorial)   TutorialAwaiking();

    }
    else
    {
        if (is_enemy_hit)
        {
            end_dash_effect = true;
            //�G�ɓ������čU���{�^��(�ːi�{�^��)����������ꌂ��
            is_charge = false;
            velocity = {};
            //�`���[�g���A�����U���Ȃ�
            if (tutorial_state == TutorialState::AttackTutorial) is_next_tutorial = true;
            TransitionTutorialAttack1(attack_animation_blends_speeds.y);
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

void Player::TutorialAttack1Update(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{

    if (model->end_of_animation())
    {
        attack_time += attack_add_time * elapsed_time;
        //�P�\���Ԃ𒴂�����ҋ@�ɑJ��
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionTutoriaIdle();
        }
        //�P�\���Ԃ���������������U��2���ڂɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionTutorialAttack2(attack_animation_blends_speeds.z);
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

void Player::TutorialAttack2Update(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{

    attack_time += attack_add_time * elapsed_time;
    //�G�ɓ������������Ԃ�2�b��������������I���
    if (is_update_animation == false && (is_enemy_hit || attack_time >= 1.0f))
    {
        is_charge = false;
        attack_time = 0;
        is_update_animation = true;
    }
    else
    {
        float length{ Math::calc_vector_AtoB_length(position,target) };

        if (length > 5.0f)ChargeAcceleration(elapsed_time);
    }
    if (model->end_of_animation())
    {
        velocity = {};
        //�P�\���Ԃ𒴂�����ҋ@�ɑJ��
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            attack_time = 0;
            TransitionTutoriaIdle();
        }
        //�P�\���Ԃ���������������U��3���ڂɑJ��
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionTutorialAttack3(attack_animation_blends_speeds.w);
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

void Player::TutorialAttack3Update(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{

    attack_time += attack_add_time * elapsed_time;
    //�G�ɓ������������Ԃ�2�b��������������I���

    if (is_update_animation == false && (is_enemy_hit || attack_time >= 1.0f))
    {
        is_charge = false;
        attack_time = 0;
        is_update_animation = true;
    }
    else
    {
        float length{ Math::calc_vector_AtoB_length(position,target) };
        if (length > 5.0f) ChargeAcceleration(elapsed_time);
    }

    if (model->end_of_animation())
    {
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            attack_time = 0;
            TransitionTutorialCharge(attack_animation_blends_speeds.x);
        }
        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            TransitionTutorialMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
        else
        {
            charge_time = 0;
            TransitionTutoriaIdle();
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

void Player::TutorialAwaikingUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    if (model->end_of_animation())
    {
        //�����`���[�g���A�����o���Ȃ�
        if (tutorial_state == TutorialState::AwaikingTutorial) is_next_tutorial = true;
        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionTutorialMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
        else
        {
            TransitionTutoriaIdle();
        }
    }
}

void Player::TutorialInvAwaikingUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    if (model->end_of_animation())
    {
        //�ړ����͂���������ړ��ɑJ��
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionTutorialMove();
        }
        //�ړ����͂��Ȃ�������ҋ@�ɑJ��
        else
        {
            TransitionTutoriaIdle();
        }
    }
}

void Player::TransitionTutoriaIdle(float blend_second)
{
    //�o����Ԃ̎��̑ҋ@�A�j���[�V�����ɃZ�b�g
    if (is_awakening)model->play_animation(AnimationClips::AwakingIdle, true, true, blend_second);
    //�ʏ��Ԃ̑ҋ@�A�j���[�V�����ɃZ�b�g
    else model->play_animation(AnimationClips::Idle, true, true, blend_second);
    //�A�j���[�V�������x
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    player_tutorial_activity = &Player::TutorialIdleUpdate;
}

void Player::TransitionTutorialMove(float blend_second)
{
    //�o����Ԃ̎��̈ړ��A�j���[�V�����̐ݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingMove, true, true, blend_second);
    //�ʏ��Ԃ̎��Ɉړ��A�j���[�V�����̐ݒ�
    else model->play_animation(AnimationClips::Move, true, true, blend_second);
    //�A�j���[�V�������x
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�U�������ǂ����̐ݒ�
    is_attack = false;

    player_tutorial_activity = &Player::TutorialMoveUpdate;
}

void Player::TransitionTutorialAvoidance(float blend_second)
{
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
    if (is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false, true);
    //�ʏ��Ԃ̎��̃A�j���[�V�����̐ݒ�
    else model->play_animation(AnimationClips::Avoidance, false, true);
    //�U�������ǂ����̐ݒ�
    is_attack = false;
    //�A�j���[�V�����̑��x
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�����Ԃ̎��̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialAvoidanvceUpdate;

}

void Player::TransitionTutorialBehindAvoidance()
{
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
    //���b�N�I�����Ă���G���X�^��������
    target_enemy->fSetStun(true);
    //�w��ɉ�荞�ނƂ��̊֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialBehindAvoidanceUpdate;;
}

void Player::TransitionTutorialChargeInit()
{
    //�o����Ԃ̎��̓ːi�̎n�܂�̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false, true);
    //�ʏ��Ԃ̎��̓ːi�̎n�܂�̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::ChargeInit, false, true);
    //�U�������ǂ����̐ݒ�
    is_attack = true;
    //�ːi�����ǂ����̐ݒ�
    is_charge = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = CHARGEINIT_ANIMATION_SPEED;
    //���b�N�I�����ĂȂ��ꍇ�̃^�[�Q�b�g�̐ݒ�
    charge_point = Math::calc_designated_point(position, forward, 60.0f);
    //�U���̉����̐ݒ�
    SetAccelerationVelocity();
    //�����̃��[�g
    lerp_rate = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�ːi�̎n�܂�̎��̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialChargeinitUpdate;

}

void Player::TransitionTutorialCharge(float blend_second)
{
    //�_�b�V���|�X�g�G�t�F�N�g��������
    start_dash_effect = true;
    //�o����Ԃ̎��̓ːi�A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingCharge, false, true, blend_second);
    //�ʏ��Ԃ̎��̓ːi�A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::Charge, false, true, blend_second);
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
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�����̃��[�g
    lerp_rate = 4.0f;
    //�ːi���̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialChargeUpdate;
}

void Player::TransitionTutorialAttack1(float blend_second)
{
    //�o����Ԃ̎��̂P���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType1, false, true, blend_second);
    //�ʏ��Ԃ̎��̂P���ڂ̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::AttackType1, false, true, blend_second);
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
    player_tutorial_activity = &Player::TutorialAttack1Update;
}

void Player::TransitionTutorialAttack2(float blend_second)
{
    //�o����Ԃ̎��̂Q���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType2, false, true, blend_second);
    //�ʏ��Ԃ̎��̂Q���ڂ̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::AttackType2, false, true, blend_second);
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
    SetAccelerationVelocity();
    //�����̃��[�g
    lerp_rate = 2.0f;
    //�U���̎���
    attack_time = 0;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = false;
    //�ːi�����ǂ����̐ݒ�
    is_charge = true;
    //�Q���ڂ̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialAttack2Update;
}

void Player::TransitionTutorialAttack3(float blend_second)
{
    //�o����Ԃ̎��̂R���ڂ̃A�j���[�V�����ɐݒ�
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType3, false, true, blend_second);
    //�ʏ��Ԃ̎��̂R���ڂ̂̃A�j���[�V�����ɐݒ�
    else model->play_animation(AnimationClips::AttackType3, false, true, blend_second);
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
    SetAccelerationVelocity();
    //�����̃��[�g
    lerp_rate = 2.0f;
    //�U���̎���
    attack_time = 0;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = false;
    //�ːi�����ǂ����̐ݒ�
    is_charge = true;
    //�R���ڂ̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialAttack3Update;

}

void Player::TransitionTutorialAwaiking()
{
    //�o����ԂɂȂ�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::Awaking, false, true);
    //�o����Ԃ��ǂ����̐ݒ�
    is_awakening = true;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�o����ԂɂȂ�r���̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialAwaikingUpdate;
}

void Player::TransitionTutorialInvAwaiking()
{
    //�ʏ��Ԃɖ߂�A�j���[�V�����ɐݒ�
    model->play_animation(AnimationClips::InvAwaking, false, true);
    //�o����Ԃ��ǂ����̐ݒ�
    is_awakening = false;
    //�A�j���[�V�������x�̐ݒ�
    animation_speed = 1.0f;
    //�A�j���[�V���������Ă������ǂ���
    is_update_animation = true;
    //�ʏ��Ԃɖ߂��Ă�Ƃ��̍X�V�֐��ɐ؂�ւ���
    player_tutorial_activity = &Player::TutorialInvAwaikingUpdate;
}

void Player::TutorialAwaiking()
{
    //�{�^������
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT - 5.0f)TransitionTutorialAwaiking();//�R���{�J�E���g���ő�̂Ƃ��͊o����ԂɂȂ�
    }
    if (is_awakening && combo_count <= 0) TransitionTutorialInvAwaiking();//�o����Ԃ̂Ƃ��ɃJ�E���g��0�ɂȂ�����ʏ��ԂɂȂ�
}
