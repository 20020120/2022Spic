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
        //回り込み回避よりも進んでいたら切り替えれる
        if (tutorial_state > TutorialState::BehindAvoidanceTutorial)
        {
            if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
            {
                transition_chain_behavior();
            }
        }
        //チュートリアルがロックオンの時よりも大きければ出来る
        if (tutorial_state >= TutorialState::LockOnTutorial)     TutorialLockOn();
        //カメラリセット
        CameraReset();
        break;
    case Player::Behavior::Chain:

        break;
    default:
        break;
    }

    //アニメーション更新処理
    GetPlayerDirections();
    //プレイヤーのパラメータの変更
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
            float length_radius = Math::calc_vector_AtoB_length(p, target);//距離(半径)
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
        //自分のクラスの関数ポインタを呼ぶ
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
    //もし今のチュートリアルステートよりも小さい値が入ってきたら
    //ひとつ前にもどるからはいれないようにする
    if (tutorial_state > static_cast<TutorialState>(state) || tutorial_state == static_cast<TutorialState>(state)) return;
    tutorial_state = static_cast<TutorialState>(state);
    is_next_tutorial = false;
    execution_timer = 0;
}

void Player::TutorialIdleUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    //移動に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionTutorialMove();
    }

    switch (tutorial_state)
    {
    case Player::TutorialState::MoveTutorial:
        //MoveTutorialの時にこの時間動いていたら終わったことにする
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
        //突進開始に遷移
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
            //回避に遷移
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //そうじゃなかったら普通の回避
            else TransitionTutorialAvoidance();

        }
        //突進開始に遷移
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
            //回避に遷移
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //そうじゃなかったら普通の回避
            else TransitionTutorialAvoidance();

        }
        //突進開始に遷移
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
    //移動入力がなくなったら待機に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionTutoriaIdle();
    }
    switch (tutorial_state)
    {
    case Player::TutorialState::MoveTutorial:
        execution_timer += 1.0f * elapsed_time;
        //MoveTutorialの時にこの時間動いていたら終わったことにする
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
        //突進開始に遷移
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
            //回避に遷移
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //そうじゃなかったら普通の回避
            else TransitionTutorialAvoidance();

        }
        //突進開始に遷移
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
            //回避に遷移
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //そうじゃなかったら普通の回避
            else TransitionTutorialAvoidance();

        }
        //突進開始に遷移
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
            //回避に遷移
            float length{ Math::calc_vector_AtoB_length(position, target) };
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionTutorialBehindAvoidance();
            }
            //そうじゃなかったら普通の回避
            else TransitionTutorialAvoidance();

        }
        //突進開始に遷移
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
        //もしチュートリアルが回避なら
        if (tutorial_state == TutorialState::AvoidanceTutorial) is_next_tutorial = true;
            //回避中かどうかの設定
            is_avoidance = false;
            is_behind_avoidance = false;
            //移動入力があったら移動に遷移
            if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
            {
                TransitionTutorialMove();
            }
            //移動入力がなかったら待機に遷移
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
        //回避中かどうかの設定
        is_avoidance = false;
        is_behind_avoidance = false;
        is_just_avoidance = false;
        //もしチュートリアルが回り込み回避なら
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
    //突進時間を超えたらそれぞれの遷移にとぶ
    if (charge_time > CHARGE_MAX_TIME)
    {
        velocity = {};
        end_dash_effect = true;

        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            is_charge = false;
            TransitionTutorialMove();
        }
        //移動入力がなかったら待機に遷移
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
            //敵に当たって攻撃ボタン(突進ボタン)を押したら一撃目
            is_charge = false;
            velocity = {};
            //チュートリアルが攻撃なら
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
        //猶予時間を超えたら待機に遷移
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionTutoriaIdle();
        }
        //猶予時間よりも早く押したら攻撃2撃目に遷移
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
    //敵に当たったか時間が2秒たったら加速を終わる
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
        //猶予時間を超えたら待機に遷移
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            attack_time = 0;
            TransitionTutoriaIdle();
        }
        //猶予時間よりも早く押したら攻撃3撃目に遷移
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
    //敵に当たったか時間が2秒たったら加速を終わる

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
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            TransitionTutorialMove();
        }
        //移動入力がなかったら待機に遷移
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
        //もしチュートリアルが覚醒なら
        if (tutorial_state == TutorialState::AwaikingTutorial) is_next_tutorial = true;
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionTutorialMove();
        }
        //移動入力がなかったら待機に遷移
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
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionTutorialMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            TransitionTutoriaIdle();
        }
    }
}

void Player::TransitionTutoriaIdle(float blend_second)
{
    //覚醒状態の時の待機アニメーションにセット
    if (is_awakening)model->play_animation(AnimationClips::AwakingIdle, true, true, blend_second);
    //通常状態の待機アニメーションにセット
    else model->play_animation(AnimationClips::Idle, true, true, blend_second);
    //アニメーション速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //攻撃中かどうかの設定
    is_attack = false;
    player_tutorial_activity = &Player::TutorialIdleUpdate;
}

void Player::TransitionTutorialMove(float blend_second)
{
    //覚醒状態の時の移動アニメーションの設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingMove, true, true, blend_second);
    //通常状態の時に移動アニメーションの設定
    else model->play_animation(AnimationClips::Move, true, true, blend_second);
    //アニメーション速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //攻撃中かどうかの設定
    is_attack = false;

    player_tutorial_activity = &Player::TutorialMoveUpdate;
}

void Player::TransitionTutorialAvoidance(float blend_second)
{
    //回避中かどうかの設定
    is_avoidance = true;
    //回り込み回避かどうか
    is_behind_avoidance = false;
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
    if (is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false, true);
    //通常状態の時のアニメーションの設定
    else model->play_animation(AnimationClips::Avoidance, false, true);
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーションの速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //回避状態の時の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialAvoidanvceUpdate;

}

void Player::TransitionTutorialBehindAvoidance()
{
    velocity = {};
    //回避中かどうかの設定
    is_avoidance = true;
    //回り込み回避かどうか
    is_behind_avoidance = true;
    //覚醒状態の時の回避アニメーションの設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false, true);
    //通常状態の時のアニメーションの設定
    else model->play_animation(AnimationClips::Avoidance, false, true);
    //後ろに回り込む座標の取得
    BehindAvoidancePosition();
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
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //ロックオンしている敵をスタンさせる
    target_enemy->fSetStun(true);
    //背後に回り込むときの関数に切り替える
    player_tutorial_activity = &Player::TutorialBehindAvoidanceUpdate;;
}

void Player::TransitionTutorialChargeInit()
{
    //覚醒状態の時の突進の始まりのアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false, true);
    //通常状態の時の突進の始まりのアニメーションに設定
    else model->play_animation(AnimationClips::ChargeInit, false, true);
    //攻撃中かどうかの設定
    is_attack = true;
    //突進中かどうかの設定
    is_charge = true;
    //アニメーション速度の設定
    animation_speed = CHARGEINIT_ANIMATION_SPEED;
    //ロックオンしてない場合のターゲットの設定
    charge_point = Math::calc_designated_point(position, forward, 60.0f);
    //攻撃の加速の設定
    SetAccelerationVelocity();
    //加速のレート
    lerp_rate = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //突進の始まりの時の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialChargeinitUpdate;

}

void Player::TransitionTutorialCharge(float blend_second)
{
    //ダッシュポストエフェクトをかける
    start_dash_effect = true;
    //覚醒状態の時の突進アニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingCharge, false, true, blend_second);
    //通常状態の時の突進アニメーションに設定
    else model->play_animation(AnimationClips::Charge, false, true, blend_second);
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
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //加速のレート
    lerp_rate = 4.0f;
    //突進中の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialChargeUpdate;
}

void Player::TransitionTutorialAttack1(float blend_second)
{
    //覚醒状態の時の１撃目のアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType1, false, true, blend_second);
    //通常状態の時の１撃目のアニメーションに設定
    else model->play_animation(AnimationClips::AttackType1, false, true, blend_second);
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーションスピードの設定
#if 1
    animation_speed = ATTACK1_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.y;
#endif // 0
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //加速のレート
    lerp_rate = 4.0f;
    //１撃目の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialAttack1Update;
}

void Player::TransitionTutorialAttack2(float blend_second)
{
    //覚醒状態の時の２撃目のアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType2, false, true, blend_second);
    //通常状態の時の２撃目のアニメーションに設定
    else model->play_animation(AnimationClips::AttackType2, false, true, blend_second);
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
#if 1
    animation_speed = ATTACK2_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.z;
#endif // 0
    //攻撃の加速の設定
    SetAccelerationVelocity();
    //加速のレート
    lerp_rate = 2.0f;
    //攻撃の時間
    attack_time = 0;
    //アニメーションをしていいかどうか
    is_update_animation = false;
    //突進中かどうかの設定
    is_charge = true;
    //２撃目の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialAttack2Update;
}

void Player::TransitionTutorialAttack3(float blend_second)
{
    //覚醒状態の時の３撃目のアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAttackType3, false, true, blend_second);
    //通常状態の時の３撃目ののアニメーションに設定
    else model->play_animation(AnimationClips::AttackType3, false, true, blend_second);
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
#if 1
    animation_speed = ATTACK3_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.w;
#endif // 0
    //攻撃の加速の設定
    SetAccelerationVelocity();
    //加速のレート
    lerp_rate = 2.0f;
    //攻撃の時間
    attack_time = 0;
    //アニメーションをしていいかどうか
    is_update_animation = false;
    //突進中かどうかの設定
    is_charge = true;
    //３撃目の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialAttack3Update;

}

void Player::TransitionTutorialAwaiking()
{
    //覚醒状態になるアニメーションに設定
    model->play_animation(AnimationClips::Awaking, false, true);
    //覚醒状態かどうかの設定
    is_awakening = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //覚醒状態になる途中の更新関数に切り替える
    player_tutorial_activity = &Player::TutorialAwaikingUpdate;
}

void Player::TransitionTutorialInvAwaiking()
{
    //通常状態に戻るアニメーションに設定
    model->play_animation(AnimationClips::InvAwaking, false, true);
    //覚醒状態かどうかの設定
    is_awakening = false;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //通常状態に戻ってるときの更新関数に切り替える
    player_tutorial_activity = &Player::TutorialInvAwaikingUpdate;
}

void Player::TutorialAwaiking()
{
    //ボタン入力
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT - 5.0f)TransitionTutorialAwaiking();//コンボカウントが最大のときは覚醒状態になる
    }
    if (is_awakening && combo_count <= 0) TransitionTutorialInvAwaiking();//覚醒状態のときにカウントが0になったら通常状態になる
}
