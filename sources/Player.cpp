#include "Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"
//プレイヤーの原点は腰

Player::Player(GraphicsPipeline& graphics)
    :BasePlayer()
{
    model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\Player\\player_sixteenth.fbx",true,60.0f);
    model->play_animation(AnimationClips::Idle, true);
    scale = { 0.06f,0.06f,0.06f };
    GetPlayerDirections();
    mSwordTrail[0].fInitialize(graphics.get_device().Get(),
        L"./resources/TexMaps/SwordTrail/warp_cut.png",
        L"./resources/TexMaps/SwordTrail/SwordTrail.png");
    mSwordTrail[1].fInitialize(graphics.get_device().Get(),
        L"./resources/TexMaps/SwordTrail/warp_cut.png",
        L"./resources/TexMaps/SwordTrail/SwordTrail.png");
    player_config = std::make_unique<PlayerConfig>(graphics);
    //ダメージを受ける関数を関数ポインタに格納
    damage_func = [=](int damage, float invincible)->void {Damaged(damage, invincible); };


    player_bones[0] = model->get_bone_by_name("body_joint");
    player_bones[1] = model->get_bone_by_name("face_joint");
    player_bones[2] = model->get_bone_by_name("largeblade_L_joint");
    player_bones[3] = model->get_bone_by_name("largeblade_L_top_joint");
    player_bones[4] = model->get_bone_by_name("largeblade_R_joint");
    player_bones[5] = model->get_bone_by_name("largeblade_R_top_joint");
    player_bones[6] = model->get_bone_by_name("shortsword_joint");
    player_bones[7] = model->get_bone_by_name("shortsword_top_joint");
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
void Player::Update(float elapsed_time, GraphicsPipeline& graphics,SkyDome* sky_dome)
{

    ExecFuncUpdate(elapsed_time, sky_dome);
    GetPlayerDirections();
    //ロックオン
    LockOn();
    //カメラリセット
    CameraReset();
    //プレイヤーのパラメータの変更
    InflectionParameters(elapsed_time);
    //体の大きさのカプセルパラメータ設定
    BodyCapsule();
    //剣の大きさのカプセルのパラメータ
    SwordCapsule();
    if (is_awakening)
    {
        for (int i = 0; i < 2; ++i)
        {
            mSwordTrail[i].fUpdate(elapsed_time, 10);
            mSwordTrail[i].fEraseTrailPoint();
        }
    }
    else
    {
        mSwordTrail[0].fUpdate(elapsed_time, 10);
        mSwordTrail[0].fEraseTrailPoint();
    }
    player_config->update(graphics,elapsed_time);
    model->update_animation(elapsed_time * animation_speed);

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
                ImGui::Checkbox("is_awakening", &is_awakening);
                ImGui::Checkbox("start_dash_effect", &start_dash_effect);
                ImGui::Checkbox("end_dash_effect", &end_dash_effect);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("CapsuleParam"))
            {
                if (ImGui::TreeNode("BodyCapsuleParam"))
                {
                    ImGui::DragFloat3("capsule_parm.start", &capsule_body_start.x, 0.1f);
                    ImGui::DragFloat3("capsule_parm.end", &capsule_body_end.x, 0.1f);
                    ImGui::DragFloat("body_capsule_param.rasius", &body_capsule_param.rasius, 0.1f);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("easing"))
            {
                ImGui::DragFloat("avoidance_easing_time", &avoidance_easing_time,0.1f);
                ImGui::DragFloat("avoidance_boost_time", &avoidance_boost_time,0.1f);
                ImGui::DragFloat("leverage", &leverage,0.1f);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("PlayerGameParm"))
            {
                ImGui::DragInt("player_health", &player_health);
                ImGui::DragFloat("combo", &combo_count);

                ImGui::DragFloat("duration_combo_timer", &duration_combo_timer);
                ImGui::DragInt("player_attack_power", &player_attack_power);
                ImGui::DragFloat("invincible_timer", &invincible_timer);
                ImGui::TreePop();
            }

            ImGui::DragFloat("threshold", &threshold,0.01f,0,1.0f);
            ImGui::DragFloat("threshold_mesh", &threshold_mesh,0.01f,0,1.0f);
            ImGui::DragFloat("glow_time", &glow_time);

            ImGui::DragFloat4("attack_animation_speeds", &attack_animation_speeds.x,0.1f);
            ImGui::DragFloat4("attack_animation_blends_speeds", &attack_animation_blends_speeds.x,0.1f);

            ImGui::End();
        }
    }
#endif // USE_IMGUI

}

void Player::Render(GraphicsPipeline& graphics, float elapsed_time)
{
    glow_time += 1.0f * elapsed_time;
    if (glow_time >= 3.0f) glow_time = 0;
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);

    SkinnedMesh::mesh_tuple mesh_r = std::make_tuple("armor_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple mesh_l = std::make_tuple("armor_L_mdl", threshold_mesh);

    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f }, threshold, glow_time, {1.0f,1.0f,1.0f,1.0f}, mesh_r, mesh_l);

    graphics.set_pipeline_preset(RASTERIZER_STATE::CULL_NONE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
    if (is_awakening)
    {
        mSwordTrail[0].fRender(graphics.get_dc().Get());
        mSwordTrail[1].fRender(graphics.get_dc().Get());
    }
    else
    {
        mSwordTrail[0].fRender(graphics.get_dc().Get());
    }

    player_config->render(graphics.get_dc().Get());
}

void Player::BehindAvoidancePosition()
{
    using namespace DirectX;
    XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
    float length_radius = Math::calc_vector_AtoB_length(p, target);//距離(半径)
    float diameter = length_radius * 2.0f;//(直径)
    DirectX::XMFLOAT3 r{ right };
    //どっちのvelocityで左右判定するか
    if (velocity.x * velocity.x > velocity.z * velocity.z)
    {
        if (velocity.x > 0)
        {
            r = right;
        }
        else
        {
            r.x = -right.x;
            r.y = -right.y;
            r.z = -right.z;
        }
    }
    else
    {
        if (velocity.z > 0)
        {
            r = right;
        }
        else
        {
            r.x = -right.x;
            r.y = -right.y;
            r.z = -right.z;
        }
    }
    //-----------------ゴール地点---------------//
    behind_point_3.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(90.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(90.0f)))) * length_radius);//敵の後ろ側
    behind_point_3.y = position.y;//敵の後ろ側
    behind_point_3.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(90.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(90.0f)))) * length_radius);//敵の後ろ側
    //--------------------------------------------//
    //----------------中継１---------------------//
    behind_point_1.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(290.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(290.0f)))) * length_radius);//敵の後ろ側
    behind_point_1.y = position.y;//敵の後ろ側
    behind_point_1.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(290.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(290.0f)))) * length_radius);//敵の後ろ側
    //--------------------------------------------//
    //----------------中継2---------------------//
    behind_point_2.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(320.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(320.0f)))) * length_radius);//敵の後ろ側
    behind_point_2.y = position.y;//敵の後ろ側
    behind_point_2.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(320.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(320.0f)))) * length_radius);//敵の後ろ側

    behind_point_0 = position;

    //--------------------------------------------//
}

void Player::InterpolateCatmullRomSpline(float elapsed_time)
{
    using namespace DirectX;
    behind_late = easing::Expo::easeInOut(behind_timer, 0, 1.0f, 2.0f);
#if 0
    const float power = 1.0f; // Usually power is 0.5f
    XMVECTOR p0 = XMLoadFloat3(&position);
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
        position = Math::lerp(position, interpolated_point, 2.0f * elapsed_time);
    }
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

#else
    behind_point.emplace_back(position);
    behind_point.emplace_back(behind_point_1);
    behind_point.emplace_back(behind_point_2);
    behind_point.emplace_back(behind_point_3);

    if (behind_timer < 1.0f)
    {
        position = Math::HermiteFloat3(behind_point, behind_timer);
    }
    behind_point.clear();
#endif

}

void Player::InflectionParameters(float elapsed_time)
{
    player_config->set_hp_percent(static_cast<float>(static_cast<float>(player_health) / 100.0f));
    player_config->set_mp_percent(combo_count / 50.0f);
    //攻撃力の変動
    InflectionPower(elapsed_time);
    //コンボの変動
    InflectionCombo(elapsed_time);
    //無敵時間の減少
    invincible_timer -= 1.0f * elapsed_time;
}

void Player::InflectionPower(float elapsed_time)
{
    static float timer = 0;
    timer += 1.0f * elapsed_time;

    if (timer > 0.5f)
    {
        player_attack_power -= 1;
        timer = 0;
    }
    //覚醒状態かどうか
    if (is_awakening)
    {
        player_attack_power = (int)combo_count * 2;
        player_attack_power = Math::clamp(player_attack_power, MIN_PLAYER_ATTACK_POWER, MAX_PLAYER_ATTACK_POWER * 2);
    }
    else
    {
        player_attack_power = (int)combo_count;
        player_attack_power = Math::clamp(player_attack_power, MIN_PLAYER_ATTACK_POWER, MAX_PLAYER_ATTACK_POWER);
    }
}

void Player::InflectionCombo(float elapsed_time)
{
    duration_combo_timer += 1.0f * elapsed_time;
    if (is_awakening)
    {
        combo_count -= elapsed_time * 2.0f;
    }
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);

    //if (duration_combo_timer < 0 && combo_count > 0) duration_combo_timer = 5.0f;
    //コンボ中タイマーが0になったらコンボは0にする
    //if (duration_combo_timer <= 0) combo_count = 0;
}

void Player::BodyCapsule()
{
    {
        DirectX::XMFLOAT3 pos = {}, up = {};
        DirectX::XMFLOAT3 end = {}, e_up = {};

        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[0], pos, up);
        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[1], end, e_up);

        body_capsule_param.start = pos;
        body_capsule_param.end = end;
        body_capsule_param.rasius = 1.5f;
    }
    {
        DirectX::XMFLOAT3 pos = {}, up = {};
        DirectX::XMFLOAT3 end = {}, e_up = {};

        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[0], pos, up);
        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[1], end, e_up);

        charge_capsule_param.start = pos;
        charge_capsule_param.end = end;
        charge_capsule_param.rasius = 1.7f;
    }
}
void Player::SwordCapsule()
{
    DirectX::XMFLOAT3 pos = {}, up = {};
    DirectX::XMFLOAT3 pos_2 = {}, up_2 = {};
    DirectX::XMFLOAT3 end = {}, e_up = {};
    DirectX::XMFLOAT3 end_2 = {}, e_up_2 = {};
    if (is_awakening)
    {

            model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[2], pos, up);
            model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[3], end, e_up);
            model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[4], pos_2, up_2);
            model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[5], end_2, e_up_2);

            sword_capsule_param[0].start = pos;
            sword_capsule_param[0].end = end;
            sword_capsule_param[0].rasius = 1.7f;

            sword_capsule_param[1].start = pos_2;
            sword_capsule_param[1].end = end_2;
            sword_capsule_param[1].rasius = 1.7f;

    }
    else
    {
        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[6], pos, up);
        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[7], end, e_up);

        sword_capsule_param[0].start = pos;
        sword_capsule_param[0].end = end;
        sword_capsule_param[0].rasius = 1.7f;
        sword_capsule_param[1].start = pos;
        sword_capsule_param[1].end = end;
        sword_capsule_param[1].rasius = 1.7f;
    }

    //debug_figure->create_capsule(sword_capsule_param[0].start, sword_capsule_param[0].end, sword_capsule_param[0].rasius, { 1.0f,1.0f,0.0f,1.0f });
    //debug_figure->create_capsule(sword_capsule_param[1].start, sword_capsule_param[1].end, sword_capsule_param[1].rasius, { 1.0f,1.0f,0.0f,1.0f });
}

void Player::SetTarget(const BaseEnemy* target_enemies)
{
    //ターゲットしている敵が死んでいたら次の敵を入れる
    if (target_enemy != nullptr && target_enemy->fGetIsAlive() == false)
    {
        target_enemy = target_enemies;
        //target_count = 0;
        //old_target_count = 0;
    }
    //ターゲットを設定するのはロックオンした瞬間だけ
    if (is_lock_on == false && target_enemies != nullptr)
    {
        target_enemy = target_enemies;
        //target_count++;
        //old_target_count = target_count;
    }
    //else if (is_lock_on && target_enemy == nullptr && target_enemies != nullptr)
    //{
    //    target_enemy = target_enemies;
    //    target_count++;
    //    old_target_count = target_count;
    //}

}

void Player::AddCombo(int count)
{
    if (count != 0)
    {
        //攻撃中じゃないとき
        if (!is_attack)
        {
            //無敵時間がなかったらダメージ受けて無敵時間設定，コンボ0に
            if (invincible_timer < 0)
            {
                invincible_timer = 1.0f;
                player_health -= 1;
            }
        }
        else
        {
            combo_count += static_cast<float>(count);
            if(is_special_surge) special_surge_combo_count += static_cast<float>(count);//ゲージ消費の突進中に当たった数を保存
        }
        is_enemy_hit = true;
    }
    else is_enemy_hit = false;
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);
}

void Player::Damaged(int damage, float InvincibleTime)
{
    //ダメージが0の場合は健康状態を変更する必要がない
    if (damage == 0)return;

    //死亡している場合は健康状態を変更しない
    if (player_health <= 0)return;


    if (invincible_timer > 0.0f)return;

    TransitionDamage();
    //無敵時間設定
    invincible_timer = InvincibleTime;
    //ダメージ処理
    player_health -= damage;

    // 死亡した時の処理
    /*if (player_health <= 0)
    {

    }*/
}

void Player::PlayerKnocKback(float elapsed_time)
{
    velocity.x = (-forward.x * 2.0f)* velocity.x;
    velocity.y = (-forward.y * 2.0f)* velocity.y;
    velocity.z = (-forward.z * 2.0f)* velocity.z;

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
                velocity.x = easing::Quint::easeOut(avoidance_boost_time, avoidance_start.x, avoidance_end.x, avoidance_easing_time);
                velocity.z = easing::Quint::easeOut(avoidance_boost_time, avoidance_start.z, avoidance_end.z, avoidance_easing_time);
            }
            else
            {
                velocity.x = easing::Quint::easeOut(avoidance_boost_time, avoidance_start.x, avoidance_end.x, avoidance_easing_time);
                velocity.y = easing::Quint::easeOut(avoidance_boost_time, avoidance_start.y, avoidance_end.y, avoidance_easing_time);
                velocity.z = easing::Quint::easeOut(avoidance_boost_time, avoidance_start.z, avoidance_end.z, avoidance_easing_time);
            }
        }
        velocity.x = Math::clamp(velocity.x, -35.0f, 35.0f);
        velocity.y = Math::clamp(velocity.y, -35.0f, 35.0f);
        velocity.z = Math::clamp(velocity.z, -35.0f, 35.0f);
}

void Player::ChargeAcceleration(float elapse_time)
{

    DirectX::XMFLOAT3 pos = {}, up = {};

    //位置を補間
    //ロックオンしていたらターゲットに向かって行く
    if (is_lock_on)
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v,DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, target)));
        float length{ Math::calc_vector_AtoB_length(position,target) };

        velocity.x = v.x * length * 5.0f;
        velocity.y = v.y * length * 5.0f;
        velocity.z = v.z * length * 5.0f;
        //position = Math::lerp(position, target, 10.0f * elapse_time);
    }
    else
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, charge_point)));
        float length{ Math::calc_vector_AtoB_length(position,charge_point) };

        velocity.x = v.x * length * 5.0f;
        velocity.y = v.y * length * 5.0f;
        velocity.z = v.z * length * 5.0f;

        //position = Math::lerp(position, charge_point, 7.0f * elapse_time);
    }
}

void Player::ChargeInitAcceleration(float elapse_time)
{
    DirectX::XMFLOAT3 pos = {}, up = {};

    //位置を補間
    //ロックオンしていたらターゲットに向かって行く
    if (is_lock_on)
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, target)));
        float length{ Math::calc_vector_AtoB_length(position,target) };

        velocity.x = v.x * 7.0f;
        velocity.y = v.y * 7.0f;
        velocity.z = v.z * 7.0f;
        //position = Math::lerp(position, target, 10.0f * elapse_time);
    }
    else
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, charge_point)));
        float length{ Math::calc_vector_AtoB_length(position,charge_point) };

        velocity.x = v.x * 10.0f;
        velocity.y = v.y * 10.0f;
        velocity.z = v.z * 10.0f;

        //position = Math::lerp(position, charge_point, 7.0f * elapse_time);
    }
}

void Player::SpecialSurgeAcceleration()
{
    if (is_special_surge)
    {
        if (is_lock_on)
        {
            DirectX::XMFLOAT3 v{};
            DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, target)));
            float length{ Math::calc_vector_AtoB_length(position,target) };

            velocity.x = v.x * length * 4.0f;
            velocity.y = v.y * length * 4.0f;
            velocity.z = v.z * length * 4.0f;

            //position = Math::lerp(position, target, 5.0f * elapse_time);
        }
        else
        {
            DirectX::XMFLOAT3 v{};
            DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, charge_point)));
            float length{ Math::calc_vector_AtoB_length(position,charge_point) };
            velocity.x = v.x * length * 4.0f;
            velocity.y = v.y * length * 4.0f;
            velocity.z = v.z * length * 4.0f;

            //position = Math::lerp(position, charge_point, 5.0f * elapse_time);
        }
    }
}

void Player::LockOn()
{
    //今プレイヤーに一番近い敵が生きている時かつフラスタムの中にいたら
    if (target_enemy != nullptr && target_enemy->fGetIsAlive() && target_enemy->fGetIsFrustum())
    {
        target = target_enemy->fGetPosition();//敵の位置を代入して
        is_enemy = true;//trueにする
    }
    else is_enemy = false;
    enemy_length = Math::calc_vector_AtoB_length(position, target);
    //自分と敵の距離を見る
    float length{ Math::calc_vector_AtoB_length(position, target) };

    if (is_enemy && length < LOCK_ON_LANGE)
    {
        if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER || game_pad->get_trigger_L())
        {
            if (is_lock_on == false)is_camera_lock_on = true;
            is_lock_on = true;
        }
        else
        {
            is_lock_on = false;
            target_count = 0;
        }
    }
    else
    {
        is_camera_lock_on = false;
        is_lock_on = false;
        target_count = 0;

    }

}

void Player::CameraReset()
{
    if (game_pad->get_button_down() & GamePad::BTN_X)
    {
        camera_reset = true;
        orientation = { 0,0,0,1.0f };//期待の向きもリセットしてる(プロトでは)
    }
}