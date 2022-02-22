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
    GetPlayerDirections();
    //ロックオン
    LockOn();
    //カメラリセット
    CameraReset();
    //体の大きさのカプセルパラメータ設定
    BodyCapsule();

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
                ImGui::Checkbox("is_camera_lock_on", &is_camera_lock_on);
                ImGui::Checkbox("is_enemy_hit", &is_enemy_hit);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("CapsuleParam"))
            {
                ImGui::DragFloat3("capsule_parm.start", &capsule_body_start.x, 0.1f);
                ImGui::DragFloat3("capsule_parm.end", &capsule_body_end.x, 0.1f);
                ImGui::DragFloat("capsule_parm.rasius", &capsule_parm.rasius,0.1f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("easing"))
            {
                ImGui::DragFloat("avoidance_easing_time", &avoidance_easing_time,0.1f);
                ImGui::DragFloat("avoidance_boost_time", &avoidance_boost_time,0.1f);
                ImGui::DragFloat("leverage", &leverage,0.1f);
                ImGui::TreePop();
            }
            ImGui::DragFloat("behind_timer", &behind_timer);




            ImGui::InputFloat3("camera_f", &camera_forward.x);
            ImGui::InputFloat3("camera_r", &camera_right.x);

            ImGui::DragFloat3("target", &target.x);
            ImGui::End();
        }
    }
#endif // USE_IMGUI
    model->update_animation(elapsed_time);

    //if (is_lock_on)
    //{
    //    //BehindAvoidancePosition();

    //    if (Collision::sphere_vs_sphere(position, 1.0f, behind_point_3, 1.0f))
    //    {
    //        is_enemy_hit = true;
    //    }
    //    else
    //    {
    //        is_enemy_hit = false;
    //    }
    //    Collision::sphere_vs_sphere(behind_point_1, 1.0f, behind_point_2, 1.0f);
    //}

}

void Player::Render(GraphicsPipeline& graphics, float elapsed_time)
{
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
    DirectX::XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f });
}

void Player::BehindAvoidancePosition()
{
    using namespace DirectX;
    XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
    float length_radius = Math::calc_vector_AtoB_length(p, target);//距離(半径)
    float diameter = length_radius * 2.0f;//(直径)
    //-----------------ゴール地点---------------//
    behind_point_3.x = target.x + (((right.x * cosf(DirectX::XMConvertToRadians(90.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(90.0f)))) * length_radius);//敵の後ろ側
    behind_point_3.y = target.y;//敵の後ろ側
    behind_point_3.z = target.z + (((right.z * cosf(DirectX::XMConvertToRadians(90.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(90.0f))))* length_radius);//敵の後ろ側
    //--------------------------------------------//
    //----------------中継１---------------------//
    behind_point_1.x = target.x + (((right.x * cosf(DirectX::XMConvertToRadians(330.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(330.0f)))) * length_radius);//敵の後ろ側
    behind_point_1.y = target.y;//敵の後ろ側
    behind_point_1.z = target.z + (((right.z * cosf(DirectX::XMConvertToRadians(330.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(330.0f)))) * length_radius);//敵の後ろ側
    //behind_point_1.x = position.x + ((right.x * cosf(DirectX::XMConvertToRadians(30.0f)) )* diameter);//敵の後ろ側
    //behind_point_1.y = position.y;//敵の後ろ側
    //behind_point_1.z = position.z + ((forward.z * sinf(DirectX::XMConvertToRadians(30.0f)) ) * diameter);//敵の後ろ側
    //--------------------------------------------//
    //----------------中継2---------------------//
    behind_point_2.x = target.x + (((right.x * cosf(DirectX::XMConvertToRadians(30.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(30.0f)))) * length_radius);//敵の後ろ側
    behind_point_2.y = target.y;//敵の後ろ側
    behind_point_2.z = target.z + (((right.z * cosf(DirectX::XMConvertToRadians(30.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(30.0f)))) * length_radius);//敵の後ろ側

    behind_point_0 = position;
    //--------------------------------------------//
}

void Player::InterpolateCatmullRomSpline(float elapsed_time)
{
    using namespace DirectX;
    behind_timer += 1.0f * elapsed_time;
#if 1
    const float power = 1.0f; // Usually power is 0.5f
    XMVECTOR p0 = XMLoadFloat3(&behind_point_0);
    XMVECTOR p1 = XMLoadFloat3(&behind_point_1);
    XMVECTOR p2 = XMLoadFloat3(&behind_point_2);
    XMVECTOR p3 = XMLoadFloat3(&behind_point_3);

    XMVECTOR v0 = (p2 - p0) * power;
    XMVECTOR v1 = (p3 - p1) * power;
    if (behind_timer < 1.5f)
    {
        const float t = behind_timer;
        XMVECTOR p;
        p = t * t * t * (2 * p1 - 2 * p2 + v0 + v1);
        p += t * t * (-3 * p1 + 3 * p2 - 2 * v0 - v1);
        p += t * v0 + p1;
        XMFLOAT3 interpolated_point{};
        XMStoreFloat3(&interpolated_point, p);
        position = Math::lerp(position, interpolated_point, 1.0f);
    }
#else
    XMVECTOR P0 = XMLoadFloat3(&behind_point_0);
    XMVECTOR P1 = XMLoadFloat3(&behind_point_1);
    XMVECTOR P2 = XMLoadFloat3(&behind_point_2);
    XMVECTOR P3 = XMLoadFloat3(&behind_point_3);

    for (size_t step = 0; step < 100; ++step)
    {
        float t = static_cast<float>(step) / 100;
        float alpha = .5f; /* between 0 and 1 */

        XMVECTOR T0 = XMVectorZero();
        XMVECTOR T1 = XMVectorPow(XMVector3LengthSq(P1 - P0), XMLoadFloat(&alpha) * .5f) + T0;
        XMVECTOR T2 = XMVectorPow(XMVector3LengthSq(P2 - P1), XMLoadFloat(&alpha) * .5f) + T1;
        XMVECTOR T3 = XMVectorPow(XMVector3LengthSq(P3 - P2), XMLoadFloat(&alpha) * .5f) + T2;
        XMVECTOR T = XMVectorLerp(T1, T2, t);
        XMVECTOR A1 = (T1 - T) / (T1 - T0) * P0 + (T - T0) / (T1 - T0) * P1;
        XMVECTOR A2 = (T2 - T) / (T2 - T1) * P1 + (T - T1) / (T2 - T1) * P2;
        XMVECTOR A3 = (T3 - T) / (T3 - T2) * P2 + (T - T2) / (T3 - T2) * P3;
        XMVECTOR B1 = (T2 - T) / (T2 - T0) * A1 + (T - T0) / (T2 - T0) * A2;
        XMVECTOR B2 = (T3 - T) / (T3 - T1) * A2 + (T - T1) / (T3 - T1) * A3;
        XMVECTOR C = (T2 - T) / (T2 - T1) * B1 + (T - T1) / (T2 - T1) * B2;

        XMFLOAT3 interpolated_point{};
        XMStoreFloat3(&position, C);
    }


#endif

}

void Player::BodyCapsule()
{
    capsule_parm.start =
    {
        position.x + capsule_body_start.x,
        position.y + capsule_body_start.y,
        position.z + capsule_body_start.z
    };
    capsule_parm.end =
    {
        position.x + capsule_body_end.x,
        position.y + capsule_body_end.y,
        position.z + capsule_body_end.z
    };
}

void Player::SetTarget(const BaseEnemy* target_enemys)
{
    //ターゲットを設定するのはロックオンした瞬間だけ
    if (is_lock_on == false && target_enemys != nullptr)
    {
        target_enemy = target_enemys;
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
            //一切入力がない
            if (sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z)) <= 0.0f)
            {
                velocity.x = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.x, avoidance_end.x, avoidance_easing_time);
                velocity.z = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.z, avoidance_end.z, avoidance_easing_time);
            }
            else
            {
                velocity.x = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.x, avoidance_end.x, avoidance_easing_time);
                velocity.y = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.y, avoidance_end.y, avoidance_easing_time);
                velocity.z = easing::Elastic::easeInOut(avoidance_boost_time, avoidance_start.z, avoidance_end.z, avoidance_easing_time);
            }
        }
}

void Player::ChargeAcceleration(float elapse_time)
{
    //位置を補間
    position = Math::lerp(position, target, 1.0f * elapse_time);
}

void Player::LockOn()
{
    if (target_enemy != nullptr)
    {
        target = target_enemy->fGetPosition();
        is_enemy = true;
    }
    else is_enemy = false;

    //自分と敵の距離を見る
    float length{ Math::calc_vector_AtoB_length(position, target) };

    if (is_enemy && length < LOCK_ON_LANGE)
    {
        if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER || game_pad->get_trigger_L())
        {
            if (is_lock_on == false)is_camera_lock_on = true;
            is_lock_on = true;
        }
        else is_lock_on = false;
    }
    else
    {
        is_camera_lock_on = false;
        is_lock_on = false;
    }

}

void Player::CameraReset()
{
    if (game_pad->get_button_down() & GamePad::BTN_X)
    {
        camera_reset = true;
    }
}