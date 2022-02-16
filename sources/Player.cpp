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
            float t = model->get_anim_para().animation_tick;
            ImGui::InputFloat("current_keyframe", &t);
            ImGui::InputFloat3("camera_f", &camera_forward.x);
            ImGui::InputFloat3("camera_r", &camera_right.x);
            ImGui::DragFloat("step_offset_z", &step_offset_z);
            if (ImGui::TreeNode("PlayerFlags"))
            {
                ImGui::Checkbox("camera_reset", &camera_reset);
                ImGui::Checkbox("is_lock_on", &is_lock_on);
                ImGui::Checkbox("is_enemy_hit", &is_enemy_hit);
                ImGui::TreePop();
            }
            ImGui::DragFloat("avoidance_boost_time", &avoidance_boost_time);
            ImGui::DragFloat3("target", &target.x);
            ImGui::End();
        }
    }
#endif // USE_IMGUI

    model->update_animation(elapsed_time);
    float mx{ velocity.x * step_offset_z * elapsed_time };
    float mz{ velocity.z * step_offset_z * elapsed_time };

    Collision::sphere_vs_sphere(position, 1.0f, target, 1.0f);
}

void Player::Render(GraphicsPipeline& graphics, float elapsed_time)
{
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f });
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

void Player::AvoidanceAcceleration(float elapse_time)
{
       avoidance_boost_time += 1.0f * elapse_time;
    //入力がなければプレイヤーの前方向に加速
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        //velocity.x = forward.x * 30.0f;
        //velocity.z = forward.z * 30.0f;
        //DirectX::XMFLOAT3 end{ forward.x * 15.0f ,forward.y * 15.0f,forward.z * 15.0f };
        //velocity.x = easing::Quart::easeOut(avoidance_boost_time, velocity.x,end.x, 2.0f);
        //velocity.z = easing::Quart::easeOut(avoidance_boost_time, velocity.z,end.z, 2.0f);


    }
    //入力があれば入力方向に加速
    else
    {
        //DirectX::XMVECTOR ve{ DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&velocity))};
        //DirectX::XMFLOAT3 dire{};
        //DirectX::XMStoreFloat3(&dire, ve);
        //velocity.x = dire.x * 30.0f;
        //velocity.z = dire.z * 30.0f;

    }
}

void Player::ChargeAcceleration(float elapse_time)
{
    //位置を補間
    position = Math::lerp(position, target, 1.0f * elapse_time);
}

