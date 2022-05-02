#include"Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"

void Player::UpdateTutorial(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    ExecFuncTutorialUpdate(elapsed_time, sky_dome, enemies);
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

    if (is_update_animation)model->update_animation(elapsed_time);
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

            ImGui::End();
        }
    }
#endif // USE_IMGUI

}


void Player::ExecFuncTutorialUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    (this->*player_tutorial_activity)(elapsed_time, sky_dome,enemies);
}

void Player::ChangeTutorialState(int state)
{
    //もし今のチュートリアルステートよりも小さい値が入ってきたら
    //ひとつ前にもどるからはいれないようにする
    if (tutorial_state > static_cast<TutorialState>(state)) return;
    tutorial_state = static_cast<TutorialState>(state);
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
        break;
    case Player::TutorialState::AvoidanceTutorial:
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        break;
    case Player::TutorialState::LockOnTutorial:
        break;
    case Player::TutorialState::AttackTutorial:
        break;
    case Player::TutorialState::BehindAvoidanceTutorial:
        break;
    case Player::TutorialState::ChainAttackTutorial:
        break;
    case Player::TutorialState::AwaikingTutorial:
        break;
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
        break;
    case Player::TutorialState::AvoidanceTutorial:
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            TransitionTutorialAvoidance();
        }
        break;
    case Player::TutorialState::LockOnTutorial:
        break;
    case Player::TutorialState::AttackTutorial:
        break;
    case Player::TutorialState::BehindAvoidanceTutorial:
        break;
    case Player::TutorialState::ChainAttackTutorial:
        break;
    case Player::TutorialState::AwaikingTutorial:
        break;
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
        switch (tutorial_state)
        {
        case Player::TutorialState::MoveTutorial:
            break;
        case Player::TutorialState::AvoidanceTutorial:
        {
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
            break;
        }
        case Player::TutorialState::LockOnTutorial:
        {
            break;
        }
        case Player::TutorialState::AttackTutorial:
        {
            break;
        }
        case Player::TutorialState::BehindAvoidanceTutorial:
        {
            break;
        }
        case Player::TutorialState::ChainAttackTutorial:
        {
            break;
        }
        case Player::TutorialState::AwaikingTutorial:
        {
            break;
        }
        default:
            break;
        }
        UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
    else
    {
        UpdateAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
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
