#include "Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"
//プレイヤーの原点は腰

Player::Player(GraphicsPipeline& graphics)
    :BasePlayer()
{
    model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\Player\\player_proto.fbx");
    model->play_animation(AnimationClips::Idle, true);
    scale = { 0.01f,0.01f,0.01f };
    GetPlayerDirections();
}

Player::~Player()
{

}

void Player::Initialize()
{
}

//このアップデートの中に書いていたらExecFuncUpdate関数で
//どの関数が呼ばれていても確実に通る
//アニメーションごとに動きを変えたいならそのアニメーションの時にしか呼ばれない関数で書く
void Player::Update(float elapsed_time, SkyDome* sky_dome)
{
    ExecFuncUpdate(elapsed_time, sky_dome);
    UpdateVelocity(elapsed_time, position, orientation,camera_forward,camera_right, sky_dome);
    GetPlayerDirections();
    //ロックオン
    LockOn();
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
                ImGui::DragFloat("max_speed", &move_speed);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("PlayerFlags"))
            {
                ImGui::Checkbox("camera_reset", &camera_reset);
                ImGui::Checkbox("is_lock_on", &is_lock_on);
                ImGui::Checkbox("is_enemy_hit", &is_enemy_hit);
                ImGui::TreePop();
            }
            float t = model->get_anim_para().animation_tick;
            ImGui::InputFloat("current_keyframe", &t);
            ImGui::InputFloat3("camera_f", &camera_forward.x);
            ImGui::InputFloat3("camera_r", &camera_right.x);
            ImGui::InputFloat3("avoidance_end", &avoidance_end.x);
            ImGui::DragFloat("step_offset_z", &step_offset_z);

            ImGui::DragFloat("avoidance_boost_time", &avoidance_boost_time);
            ImGui::DragFloat("avoidance_easing_time", &avoidance_easing_time,0.01f);
            ImGui::DragFloat3("target", &target.x);
            ImGui::End();
        }
    }
#endif // USE_IMGUI

    model->update_animation(elapsed_time);
    float mx{ velocity.x * step_offset_z * elapsed_time };
    float mz{ velocity.z * step_offset_z * elapsed_time };

    //if (Collision::sphere_vs_sphere(position, 3.0f, target, 3.0f))
    //{
    //    is_enemy_hit = true;
    //}
    //else
    //{
    //    is_enemy_hit = false;
    //}
}

void Player::Render(GraphicsPipeline& graphics, float elapsed_time)
{
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f });
}

void Player::SetTarget(const BaseEnemy* target_enemy)
{
    if (target_enemy != nullptr)
    {
        target = target_enemy->fGetPosition();
    }
}

void Player::GetPlayerDirections()
{
    using namespace DirectX;
    XMVECTOR orientation_vec{ XMLoadFloat4(&orientation) };
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    DirectX::XMVECTOR right_vec,forward_vec,up_vec;

    right_vec = { m4x4._11, m4x4._12, m4x4._13 };
    up_vec = { m4x4._21, m4x4._22, m4x4._23 };
    forward_vec = { m4x4._31, m4x4._32, m4x4._33 };

    XMStoreFloat3(&right, right_vec);
    XMStoreFloat3(&up, up_vec);
    XMStoreFloat3(&forward, forward_vec);

}

void Player::AvoidanceAcceleration(float elapsed_time)
{
    avoidance_boost_time += elapsed_time;

    if (avoidance_boost_time < avoidance_easing_time)
    {
        velocity.x = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.x, avoidance_end.x, avoidance_easing_time);
        velocity.z = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.z, avoidance_end.z, avoidance_easing_time);
    }
}

void Player::ChargeAcceleration(float elapse_time)
{
    //位置を補間
    position = Math::lerp(position, target, 1.0f * elapse_time);
}

void Player::LockOn()
{
    //自分と敵の距離を見る
    float length{ Math::calc_vector_AtoB_length(position, target) };

    if (length < 20.0f)
    {
        if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER || game_pad->get_trigger_L()) is_lock_on = true;
        else is_lock_on = false;
    }
    else
    {
        is_lock_on = false;
    }

}

