#include "Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"
//プレイヤーの原点は腰

Player::Player(GraphicsPipeline& graphics)
    :BasePlayer()
{
    model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\Player\\player_twentyfirst.fbx",false,60.0f);
    TransitionIdle();
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
    damage_func = [=](int damage, float invincible)->void {DamagedCheck(damage, invincible); };

    player_bones[0] = model->get_bone_by_name("body_joint");
    player_bones[1] = model->get_bone_by_name("face_joint");
    player_bones[2] = model->get_bone_by_name("largeblade_L_joint");
    player_bones[3] = model->get_bone_by_name("largeblade_L_top_joint");
    player_bones[4] = model->get_bone_by_name("largeblade_R_joint");
    player_bones[5] = model->get_bone_by_name("largeblade_R_top_joint");
    player_bones[6] = model->get_bone_by_name("shortsword_joint");
    player_bones[7] = model->get_bone_by_name("shortsword_top_joint");
    player_bones[8] = model->get_bone_by_name("camera_joint");
    //エフェクト
    player_behind_effec         = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\player_behind.efk");
    player_air_registance_effec = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\air_registance.efk");
    player_slash_hit_effec      = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\player_slash_hit.efk");
}

Player::~Player()
{

}

void Player::Initialize()
{
}

void Player::UpdateTitle(float elapsed_time)
{

    orientation = { 0.0f,1.0f,0.0f,0.002f };
    ExecFuncUpdate(elapsed_time);
    GetPlayerDirections();
    model->update_animation(elapsed_time);
            //モデルを映す
    threshold_mesh = 0.0f;
    threshold_mesh = Math::clamp(threshold_mesh, 0.0f, 1.0f);

    //薄くしておく
    emissive_color.w = 0.7f;
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

            ImGui::Checkbox("start", &start_title_animation);
            ImGui::Checkbox("end", &end_title_animation);

            ImGui::DragFloat("threshold", &threshold, 0.01f, 0, 1.0f);
            ImGui::DragFloat("threshold_mesh", &threshold_mesh, 0.01f, 0, 1.0f);
            ImGui::DragFloat("glow_time", &glow_time);
            ImGui::DragFloat4("emissive_color", &emissive_color.x, 0.1f);
            DirectX::XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
            float length_radius = Math::calc_vector_AtoB_length(p, target);//距離(半径)
            ImGui::DragFloat("l", &length_radius);
            ImGui::End();
        }
    }
#endif // USE_IMGUI
    //TurnTitle(elapsed_time, orientation, position, camera_position);

}

//このアップデートの中に書いていたらExecFuncUpdate関数で
//どの関数が呼ばれていても確実に通る
//アニメーションごとに動きを変えたいならそのアニメーションの時にしか呼ばれない関数で書く

void Player::PlayerClearUpdate(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{
    //カメラのジョイントの位置を更新し続ける
    DirectX::XMFLOAT3 up = {};
    model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[8], event_camera_joint, up);
    //プレイヤーの位置は原点に移動
    position = { 0,0,0 };

    //クリア用モーションがまだ始まってなかったら
    if (is_start_cleear_motion == false)
    {
        TransitionNamelessMotion();
    }
    ExecFuncUpdate(elapsed_time, sky_dome, enemies, graphics);
    model->update_animation(elapsed_time * animation_speed);
    //隠しているメッシュは表示させる
    threshold_mesh = 0.0f;
    //カメラのメッシュは非表示
    threshold_camera_mesh = 1.0f;
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
                ImGui::Checkbox("is_push_lock_on_button", &is_push_lock_on_button);
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
                if (ImGui::TreeNode("just_avoidance_capsule_param"))
                {
                    ImGui::DragFloat3("capsule_parm.start", &just_avoidance_capsule_param.start.x, 0.1f);
                    ImGui::DragFloat3("capsule_parm.end", &just_avoidance_capsule_param.end.x, 0.1f);
                    ImGui::DragFloat("just_avoidance_capsule_param.rasius", &just_avoidance_capsule_param.rasius, 0.1f);
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
            float length_radius = Math::calc_vector_AtoB_length(p, { 0,0,0 });//距離(半径)
            ImGui::DragFloat("l", &length_radius);

            ImGui::DragFloat3("event_camera_eye", &event_camera_eye.x,0.1f);
            ImGui::DragFloat("animation_speed", &animation_speed,0.1f);
            ImGui::DragFloat("threshold_camera_mesh", &threshold_camera_mesh,0.1f);
            ImGui::End();
        }
    }
#endif // USE_IMGUI

}
void Player::Update(float elapsed_time, GraphicsPipeline& graphics,SkyDome* sky_dome, std::vector<BaseEnemy*> enemies)
{


#ifdef USE_IMGUI
    ImGui::Begin("chain");
    if(ImGui::Button("transition chain behavior"))
    {
        transition_chain_behavior();
    }
    ImGui::End();
#endif // USE_IMGUI
    //クリア演出中じゃないとき
    if (during_clear == false)
    {
        ExecFuncUpdate(elapsed_time, sky_dome, enemies,graphics);
        switch (behavior_state)
        {
        case Player::Behavior::Normal:
            if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
            {
                transition_chain_behavior();
            }
            //ロックオン
            LockOn();
            //カメラリセット
            CameraReset();
            if (is_behind_avoidance == false)
            {
                PlayerJustification(elapsed_time, position);
                if (target_enemy != nullptr)
                {
                    PlayerEnemyJustification(elapsed_time, position, 1.2f, target_enemy->fGetPosition(), target_enemy->fGetBodyCapsule().mRadius);
                }
            }
            break;
        case Player::Behavior::Chain:

            break;
        default:
            break;
        }

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
        if (is_dying_update == false)
        {
            //覚醒状態の時は
            if (is_awakening)
            {
                //モデルを映す
                if (threshold_mesh > 0) threshold_mesh -= 2.0f * elapsed_time;
            }
            else
            {
                //モデルを消す
                if (threshold_mesh < 1) threshold_mesh += 2.0f * elapsed_time;
            }
        }

    }
        if (is_update_animation)model->update_animation(elapsed_time * animation_speed);
        threshold_mesh = Math::clamp(threshold_mesh, 0.0f, 1.0f);

#if 0
    if (is_lock_on)
    {
        //BehindAvoidancePosition();
        for (const auto& point : behind_interpolated_way_points)
        {
            debug_figure->create_sphere(point, 1.0f, { 1,1,1,1 });
        }
        for (const auto& point : behind_way_points)
        {
            debug_figure->create_sphere(point, 1.5f, { 1,0,0,1 });
        }
    }

#endif // 0

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
                ImGui::Checkbox("is_push_lock_on_button", &is_push_lock_on_button);
                ImGui::Checkbox("is_charge", &is_charge);
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
                if (ImGui::TreeNode("just_avoidance_capsule_param"))
                {
                    ImGui::DragFloat3("capsule_parm.start", &just_avoidance_capsule_param.start.x, 0.1f);
                    ImGui::DragFloat3("capsule_parm.end", &just_avoidance_capsule_param.end.x, 0.1f);
                    ImGui::DragFloat("just_avoidance_capsule_param.rasius", &just_avoidance_capsule_param.rasius, 0.1f);
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
                ImGui::DragFloat("attack_time", &attack_time);

                ImGui::DragFloat("duration_combo_timer", &duration_combo_timer);
                ImGui::DragInt("player_attack_power", &player_attack_power);
                ImGui::DragFloat("invincible_timer", &invincible_timer);
                ImGui::TreePop();
            }

            if (ImGui::Button("TransitionStageMove")) TransitionStageMove();
            if (ImGui::Button("TransitionIdle")) TransitionIdle();

            ImGui::DragFloat("threshold", &threshold,0.01f,0,1.0f);
            ImGui::DragFloat("threshold_mesh", &threshold_mesh,0.01f,0,1.0f);
            ImGui::DragFloat("glow_time", &glow_time);
            ImGui::DragFloat4("emissive_color", &emissive_color.x,0.1f);
            DirectX::XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
            float length_radius = Math::calc_vector_AtoB_length(p, {0,0,0});//距離(半径)
            ImGui::DragFloat("l", &length_radius);

            ImGui::DragFloat3("charge_point",&charge_point.x);
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
    SkinnedMesh::mesh_tuple armor_r_mdl = std::make_tuple("armor_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple armor_l_mdl = std::make_tuple("armor_L_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple wing_r_mdl = std::make_tuple("wing_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple wing_l_mdl = std::make_tuple("wing_L_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple largeblade_r_mdl = std::make_tuple("largeblade_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple largeblade_l_mdl = std::make_tuple("largeblade_L_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple prestarmor_mdl = std::make_tuple("prestarmor_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple backpack_mdl = std::make_tuple("backpack_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple camera_mdl = std::make_tuple("camera_mesh", threshold_camera_mesh);

    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f }, threshold, glow_time, emissive_color, 0.8f, armor_r_mdl, armor_l_mdl, wing_r_mdl, wing_l_mdl, largeblade_r_mdl, largeblade_l_mdl, prestarmor_mdl, backpack_mdl, camera_mdl);

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
}

void Player::ConfigRender(GraphicsPipeline& graphics, float elapsed_time)
{
    graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
    //クリア演出中じゃないとき
    if (during_clear == false && is_start_cleear_motion == false)
    {
        player_config->render(graphics.get_dc().Get());
    }
}

void Player::TitleRender(GraphicsPipeline& graphics, float elapsed_time)
{
    glow_time += 1.0f * elapsed_time;
    if (glow_time >= 3.0f) glow_time = 0;
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
    //覚醒状態の時は
    threshold_mesh = Math::clamp(threshold_mesh, 0.0f, 1.0f);
    threshold_camera_mesh = Math::clamp(threshold_camera_mesh, 0.0f, 1.0f);
    SkinnedMesh::mesh_tuple armor_r_mdl = std::make_tuple("armor_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple armor_l_mdl = std::make_tuple("armor_L_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple wing_r_mdl = std::make_tuple("wing_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple wing_l_mdl = std::make_tuple("wing_L_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple largeblade_r_mdl = std::make_tuple("largeblade_R_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple largeblade_l_mdl = std::make_tuple("largeblade_L_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple prestarmor_mdl = std::make_tuple("prestarmor_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple backpack_mdl = std::make_tuple("backpack_mdl", threshold_mesh);
    SkinnedMesh::mesh_tuple camera_mdl = std::make_tuple("camera_mesh", threshold_camera_mesh);

    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f }, threshold, glow_time, emissive_color,1.5f, armor_r_mdl, armor_l_mdl, wing_r_mdl, wing_l_mdl, largeblade_r_mdl, largeblade_l_mdl, prestarmor_mdl, backpack_mdl, camera_mdl);
}


void Player::LerpCameraTarget(float elapsed_time)
{
    //target_lerp_rate += 0.5f * elapsed_time;
    //if (target_lerp_rate < 1.0f)
    //{
    //    //float length{ Math::calc_vector_AtoB_length(end_target,target) };
    //    //if (length < 2.0f)target_lerp_rate = 1.0f;
    //    target = Math::lerp(old_target, end_target, target_lerp_rate);
    //}
    //else
    //{
    //    target = end_target;
    //}
}

void Player::BehindAvoidancePosition()
{
    //もし配列の中に何か入っていたら削除する
    if (!behind_way_points.empty()) behind_way_points.clear();
    if (!behind_interpolated_way_points.empty()) behind_interpolated_way_points.clear();

    using namespace DirectX;
    XMFLOAT3 p{ position.x,position.y + step_offset_y,position.z };
    float length_radius = Math::calc_vector_AtoB_length(p, target);//距離(半径)
    float diameter = length_radius * 0.6f;//(直径)
    //どっちのvelocityで左右判定するか
    DirectX::XMFLOAT3 r{ right };
    DirectX::XMFLOAT3 behind_point_4{};
    if (game_pad->get_axis_LX() < 0)
    {
        r.x = -r.x;
        r.y = -r.y;
        r.z = -r.z;
    }
    ////----------------中継１---------------------//
    behind_point_1.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(300.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(300.0f)))) * (length_radius * 0.6f));//敵の後ろ側
    behind_point_1.y = position.y;//敵の後ろ側
    behind_point_1.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(300.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(300.0f)))) * (length_radius * 0.6f));//敵の後ろ側
    ////--------------------------------------------//
    ////----------------中継2---------------------//
    behind_point_2.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(340.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(340.0f)))) * (length_radius * 0.4f));//敵の後ろ側
    behind_point_2.y = position.y;//敵の後ろ側
    behind_point_2.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(340.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(340.0f)))) * (length_radius * 0.4f));//敵の後ろ側
    //--------------------------------------------//
    behind_point_3.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(20.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(20.0f)))) * (length_radius * 0.4f));//敵の後ろ側
    behind_point_3.y = position.y;//敵の後ろ側
    behind_point_3.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(20.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(20.0f)))) * (length_radius * 0.4f));//敵の後ろ側
    //-----------------ゴール地点---------------//
    behind_point_4.x = target.x + (((r.x * cosf(DirectX::XMConvertToRadians(90.0f))) + (forward.x * sinf(DirectX::XMConvertToRadians(90.0f)))) * (length_radius * 1.0f));//敵の後ろ側
    behind_point_4.y = position.y;//敵の後ろ側
    behind_point_4.z = target.z + (((r.z * cosf(DirectX::XMConvertToRadians(90.0f))) + (forward.z * sinf(DirectX::XMConvertToRadians(90.0f)))) * (length_radius * 1.0f));//敵の後ろ側

    behind_way_points.emplace_back(position); // この時点でのプレイヤーの位置
    behind_way_points.emplace_back(behind_point_1); // この時点でのプレイヤーの位置
    behind_way_points.emplace_back(behind_point_2); // プレイヤーの位置とゴールの位置の中間地点
    behind_way_points.emplace_back(behind_point_3); // 中継地点
    behind_way_points.emplace_back(behind_point_4); //ゴールの位置

        // way_pointsを通るカーブを作成
    CatmullRomSpline curve(behind_way_points);
    curve.interpolate(behind_interpolated_way_points,8);

    behind_transit_index = 0;
    //--------------------------------------------//
}

bool Player::BehindAvoidanceMove(float elapsed_time, int& index, DirectX::XMFLOAT3& position, float speed,
    const std::vector<DirectX::XMFLOAT3>& points, float play)
{
    using namespace DirectX;
#if 0
    behind_late = easing::Expo::easeInOut(behind_timer, 0, 1.0f, 1.5f);
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
        DirectX::XMFLOAT3 old_pos{ position };
        position = Math::HermiteFloat3(behind_point, behind_timer);
        velocity.x = position.x - old_pos.x;
        velocity.y = position.y - old_pos.y;
        velocity.z = position.z - old_pos.z;

    }
    behind_point.clear();
#endif

#endif // 0
    assert(!points.empty() && "ポイントのサイズが0です");
    //index(配列の中の自分の位置)がゴールの位置にきたらtrueを返す
    //(この後の計算でindexの次の値を使うからサイズと同じ大きさでもダメ)
    if (index >= points.size() - 1) return true;

    XMFLOAT3 velo{};
    //自分の位置とウェイポイントのベクトルを求める
    //+1しているのはindex番目の値が自分の位置だからその次の値を取得するため
    XMVECTOR vec = XMLoadFloat3(&points.at(index + 1)) - XMLoadFloat3(&position);
    XMVECTOR norm_vec = XMVector3Normalize(vec);
    XMStoreFloat3(&velo, norm_vec);
    //長さを求めて
    XMVECTOR length_vec = DirectX::XMVector3Length(vec);
    //その値を取得(4dベクトルのxの値を取ってくる関数)
    float length = DirectX::XMVectorGetX(length_vec);
    //もし長さが設定されてる値よりも小さくなったら
    if (length <= play)
    {
        //indexを１増やして
        ++index;
        //自分の位置に代入する(+1された値がゴールだったから)
        position = points.at(index);
    }

    position.x += velo.x * speed * elapsed_time;
    position.y += velo.y * speed * elapsed_time;
    position.z += velo.z * speed * elapsed_time;

    return false;
}

void Player::InflectionParameters(float elapsed_time)
{
    //プレイヤーが死んでるかどうか
    PlayerAlive();
    player_config->set_hp_percent(static_cast<float>(static_cast<float>(player_health) / MAX_HEALTH));
    player_config->set_mp_percent(combo_count / MAX_COMBO_COUNT);
    //攻撃力の変動
    InflectionPower(elapsed_time);
    //コンボの変動
    InflectionCombo(elapsed_time);
    //体の大きさのカプセルパラメータ設定
    BodyCapsule();
    //剣の大きさのカプセルのパラメータ
    SwordCapsule();
    //ジャスト回避の範囲スタンのパラメータ
    StunSphere();
    //無敵時間の減少
    invincible_timer -= 1.0f * elapsed_time;
}

void Player::TutorialInflectionParameters(float elpased_time)
{
    player_config->set_hp_percent(static_cast<float>(static_cast<float>(player_health) / MAX_HEALTH));
    player_config->set_mp_percent(combo_count / MAX_COMBO_COUNT);
    //攻撃力の変動(今は使ってない)
    InflectionPower(elpased_time);
    //コンボの変動
    InflectionCombo(elpased_time);
    //体の大きさのカプセルパラメータ設定
    BodyCapsule();
    //剣の大きさのカプセルのパラメータ
    SwordCapsule();
    //ジャスト回避の範囲スタンのパラメータ
    StunSphere();
    //無敵時間の減少
    invincible_timer -= 1.0f * elpased_time;
}

void Player::InflectionPower(float elapsed_time)
{

    //覚醒状態かどうか
    if (is_awakening)
    {
        //player_attack_power *= 2;
        //player_attack_power = Math::clamp(player_attack_power, MIN_PLAYER_ATTACK_POWER, MAX_PLAYER_ATTACK_POWER * 2);
    }
    else
    {
        //5コンボで1攻撃力が上がるようにする
        //player_attack_power = (int)combo_count / 5;
        //player_attack_power = Math::clamp(player_attack_power, MIN_PLAYER_ATTACK_POWER, MAX_PLAYER_ATTACK_POWER);
    }
}

void Player::InflectionCombo(float elapsed_time)
{
    duration_combo_timer += 1.0f * elapsed_time;
    if (is_awakening)
    {
        combo_count -= elapsed_time * 5.0f;
    }
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);

    //if (duration_combo_timer < 0 && combo_count > 0) duration_combo_timer = 5.0f;
    //コンボ中タイマーが0になったらコンボは0にする
    //if (duration_combo_timer <= 0) combo_count = 0;
}

void Player::PlayerAlive()
{
    player_health = Math::clamp(player_health, 0, MAX_HEALTH);
    if (player_health <= 0 && condition_state == ConditionState::Alive)
    {
        TransitionDie();
    }
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
        charge_capsule_param.rasius = 4.0f;
    }
    {
        DirectX::XMFLOAT3 pos = {}, up = {};
        DirectX::XMFLOAT3 end = {}, e_up = {};

        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[0], pos, up);
        model->fech_by_bone(Math::calc_world_matrix(scale, orientation, position), player_bones[1], end, e_up);

        just_avoidance_capsule_param.start = pos;
        just_avoidance_capsule_param.end = end;
        just_avoidance_capsule_param.rasius = 4.6f;
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

void Player::StunSphere()
{
    //ジャスト回避中なら
    if (is_just_avoidance)
    {
        //覚醒状態なら
        if (is_awakening)sphere_radius = 17.0f;
        //覚醒状態じゃないなら
        else sphere_radius = 4.0f;
    }
    else sphere_radius = 0;
}

void Player::SetTarget( BaseEnemy* target_enemies)
{
    //if (target_enemies == nullptr)
    //{
    //    target_enemy = nullptr;
    //}
    //ロックオンボタンを押したとき
    if (game_pad->get_trigger_L())
    {
        //カメラ内にいる敵がいないなら(target_enemiesがnullptrの時はカメラのカリングに入っていない)
        if (target_enemies == nullptr)
        {
            //ロックオンボタンを押しているフラグをon
            is_push_lock_on_button = true;
        }
    }
    //押されていなかったらoff
    else is_push_lock_on_button = false;
    if (target_enemy != nullptr)
    {
        //ターゲットしている敵が死んでいるかスタンしていたら
        if (target_enemy->fGetIsAlive() == false || target_enemy->fGetStun() == true)
        {
            //倒した敵の位置を保存
            //if (target_lerp_rate > 1.0f)old_target = target;
            //補間率の初期化
            target_lerp_rate = 0;
            //一番近い敵を保存する
            target_enemy = target_enemies;
        }
    }
    //ターゲットを設定するのはロックオンした瞬間だけ
    if (is_lock_on == false && target_enemies != nullptr)
    {
        target_enemy = target_enemies;
    }

}

void Player::AddCombo(int count)
{
    if (count != 0)
    {
#if 0
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
            if (is_special_surge) special_surge_combo_count += static_cast<float>(count);//ゲージ消費の突進中に当たった数を保存
        }
        is_enemy_hit = true;

#endif // 0
        combo_count += static_cast<float>(count);
        //if (is_special_surge) special_surge_combo_count += static_cast<float>(count);//ゲージ消費の突進中に当たった数を保存
        is_enemy_hit = true;

    }
    else is_enemy_hit = false;
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);
}

void Player::AwakingAddCombo(int hit_count1, int hit_count2)
{
    if (hit_count1 != 0 || hit_count2 != 0)
    {
        combo_count += static_cast<float>(hit_count1 + hit_count2);
        //if (is_special_surge) special_surge_combo_count += static_cast<float>(count);//ゲージ消費の突進中に当たった数を保存
        is_enemy_hit = true;

    }
    else is_enemy_hit = false;
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);

}

void Player::DamagedCheck(int damage, float InvincibleTime)
{
    if (behavior_state == Behavior::Chain && during_chain_attack()) return;
    //ダメージが0の場合は健康状態を変更する必要がない
    if (damage == 0)return;
    //死亡している場合は健康状態を変更しない
    if (player_health <= 0)return;

    if (invincible_timer > 0.0f)return;
    //攻撃状態ならダメージを受けない
    if (is_attack) return;
    //もし回避中じゃなかったら怯む
    if(is_avoidance == false) TransitionDamage();
    else
    {
        //回避中ならひるまずダメージが下がって受ける
        damage = damage - 1;
    }
    //無敵時間設定
    invincible_timer = InvincibleTime;
    //ダメージ処理
    player_health -= damage;
    if(GameFile::get_instance().get_vibration())game_pad->set_vibration(1.0f, 1.0f, 0.2f);

    // 死亡した時の処理
    /*if (player_health <= 0)
    {

    }*/
}

void Player::TutorialDamagedCheck(int damage, float InvincibleTime)
{
    if (behavior_state == Behavior::Chain && during_chain_attack()) return;
    //チュートリアル中はダメージを受けない
    damage = 0;
    //ジャスト回避
    //ダメージが0の場合は健康状態を変更する必要がない
    if (damage == 0)return;
    //死亡している場合は健康状態を変更しない
    if (player_health <= 0)return;

    if (invincible_timer > 0.0f)return;
    //攻撃状態ならダメージを受けない
    if (is_attack) return;
    //もし回避中じゃなかったら怯む
    if (is_avoidance == false) TransitionTutorialDamage();
    else
    {
        //回避中ならひるまずダメージが下がって受ける
        damage = damage - 1;
    }
    //無敵時間設定
    invincible_timer = InvincibleTime;
    //ダメージ処理
    player_health -= damage;
    if (GameFile::get_instance().get_vibration())game_pad->set_vibration(1.0f, 1.0f, 0.2f);

}

void Player::PlayerKnocKback(float elapsed_time)
{
    velocity.x = (-forward.x * 2.0f)* velocity.x;
    velocity.y = (-forward.y * 2.0f)* velocity.y;
    velocity.z = (-forward.z * 2.0f)* velocity.z;
}

void Player::PlayerJustAvoidance(bool hit)
{
    if (hit && is_attack == false)
    {
        is_just_avoidance_capsul = true;
    }
    else
    {
        is_just_avoidance_capsul = false;
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
    //位置を補間
    //ロックオンしていたらターゲットに向かって行く
#if 0
    if (is_lock_on)
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, target)));
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
#else

    //velocity = Math::lerp(velocity, acceleration_velocity, lerp_rate * elapse_time);

#endif // 0
}
//velocityに直接値を入れる関数に変更
void Player::SetAccelerationVelocity()
{
    //位置を補間
//ロックオンしていたらターゲットに向かって行く
    if (is_lock_on)
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, target)));
        float length{ Math::calc_vector_AtoB_length(position,target) };

        velocity.x = v.x * charge_length_magnification;
        velocity.y = v.y * charge_length_magnification;
        velocity.z = v.z * charge_length_magnification;
        //position = Math::lerp(position, target, 10.0f * elapse_time);
    }
    else
    {
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, charge_point)));
        float length{ Math::calc_vector_AtoB_length(position,charge_point) };

        velocity.x = v.x * 100.0f;
        velocity.y = v.y * 100.0f;
        velocity.z = v.z * 100.0f;

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

            velocity.x = v.x * length * 3.0f;
            velocity.y = v.y * length * 3.0f;
            velocity.z = v.z * length * 3.0f;

            //position = Math::lerp(position, target, 5.0f * elapse_time);
        }
        else
        {
            DirectX::XMFLOAT3 v{};
            DirectX::XMStoreFloat3(&v, DirectX::XMVector3Normalize(Math::calc_vector_AtoB(position, charge_point)));
            float length{ Math::calc_vector_AtoB_length(position,charge_point) };
            velocity.x = v.x * length * 3.0f;
            velocity.y = v.y * length * 3.0f;
            velocity.z = v.z * length * 3.0f;

            //position = Math::lerp(position, charge_point, 5.0f * elapse_time);
        }
    }
}

void Player::LockOn()
{
#if 0
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
            //攻撃の加速の設定
            SetAccelerationVelocity();
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
#else
    //今プレイヤーに一番近い敵が生きている時かつフラスタムの中にいる場合
    if (target_enemy != nullptr)
    {
        //ロックオンボタンを押していないときにしか絶対入らない
        if (is_push_lock_on_button == false && target_enemy->fGetIsAlive() && target_enemy->fComputeAndGetIntoCamera())
        {
            //敵の位置を補完のゴールターゲットに入れる
#if 0
            end_target = target_enemy->fGetPosition();
#else
            target = target_enemy->fGetPosition();
#endif // 0
            //敵と自分の距離を求める
#if 0
            float length{ Math::calc_vector_AtoB_length(position,end_target) };
#else
            float length{ Math::calc_vector_AtoB_length(position,target) };
#endif // 0
            //敵との距離がロックオン出来る距離よりも短かい場合
            if (length < LOCK_ON_LANGE)
            {
                //ロックオンするボタンを押したら
                if (game_pad->get_trigger_L())
                {
                    //まだロックオンしていなかったらカメラに渡す用の変数にtrueを入れる
                    if (is_lock_on == false)
                    {
                        //ターゲットに入れる(最初の一回だけ)
                        //old_target = end_target;
                        target_lerp_rate = 0;
                        is_camera_lock_on = true;
                    }
                    //ロックオンしたかどうかを設定
                    is_lock_on = true;
                    //攻撃の加速の設定
                    //SetAccelerationVelocity();
                }
                else
                {
                    if (is_behind_avoidance == false)
                    {
                        is_lock_on = false;
                        is_camera_lock_on = false;
                    }
                }
            }
            else
            {
                is_lock_on = false;
                is_camera_lock_on = false;
            }
        }
        else
        {
            is_lock_on = false;
            is_camera_lock_on = false;
        }
    }
    else
    {
        is_lock_on = false;
        is_camera_lock_on = false;
    }

#endif // 0

}

void Player::TutorialLockOn()
{
#if 0
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
            //攻撃の加速の設定
            SetAccelerationVelocity();
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
#else
    //今プレイヤーに一番近い敵が生きている時かつフラスタムの中にいる場合
    if (target_enemy != nullptr)
    {
        if (is_push_lock_on_button == false && target_enemy->fGetIsAlive() && target_enemy->fComputeAndGetIntoCamera())
        {
            //敵の位置を補完のゴールターゲットに入れる
#if 0
            end_target = target_enemy->fGetPosition();
#else
            target = target_enemy->fGetPosition();
#endif // 0
            //敵と自分の距離を求める
#if 0
            float length{ Math::calc_vector_AtoB_length(position,end_target) };
#else
            float length{ Math::calc_vector_AtoB_length(position,target) };
#endif // 0
            //敵との距離がロックオン出来る距離よりも短かい場合
            if (length < LOCK_ON_LANGE)
            {
                //ロックオンするボタンを押したら
                if (game_pad->get_trigger_L())
                {
                    //まだロックオンしていなかったらカメラに渡す用の変数にtrueを入れる
                    if (is_lock_on == false)
                    {
                        //ターゲットに入れる(最初の一回だけ)
                        //old_target = end_target;
                        target_lerp_rate = 0;
                        is_camera_lock_on = true;
                    }
                    //ロックオンしたかどうかを設定
                    is_lock_on = true;
                    //攻撃の加速の設定
                    //SetAccelerationVelocity();
                    //もしチュートリアルがロックオンなら
                    if (tutorial_state == TutorialState::LockOnTutorial) is_next_tutorial = true;
                }
                else
                {
                    is_lock_on = false;
                    is_camera_lock_on = false;
                }
            }
            else
            {
                is_lock_on = false;
                is_camera_lock_on = false;
            }
        }
        else
        {
            is_lock_on = false;
            is_camera_lock_on = false;
        }
    }
    else
    {
        is_lock_on = false;
        is_camera_lock_on = false;
    }

#endif // 0

}

void Player::CameraReset()
{
    if (game_pad->get_button_down() & GamePad::BTN_X)
    {
        camera_reset = true;
        orientation = { 0,0,0,1.0f };//期待の向きもリセットしてる(プロトでは)
    }
}
