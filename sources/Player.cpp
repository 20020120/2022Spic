#include "Player.h"
#include"imgui_include.h"
#include"user.h"

Player::Player(GraphicsPipeline& graphics)
{
    model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\Player\\player_proto.fbx");
    scale = { 0.03f,0.03f,0.03f };
}

Player::~Player()
{

}

void Player::Initialize()
{
}

void Player::Update(float elapsed_time)
{
    UpdateVelocity(elapsed_time, position, orientation,camera_forward,camera_right);
#ifdef USE_IMGUI
    static bool display_scape_imgui;
    imgui_menu_bar("Player", "Player", display_scape_imgui);
    if (ImGui::Begin("Player"))
    {
        if (ImGui::TreeNode("transform"))
        {
            ImGui::DragFloat3("position", &position.x);
            ImGui::DragFloat3("scale", &scale.x);
            ImGui::DragFloat4("orientation", &orientation.x);
            ImGui::TreePop();
        }
        ImGui::DragFloat3("velocity", &velocity.x);
        ImGui::InputFloat3("camera_f", &camera_forward.x);
        ImGui::InputFloat3("camera_r", &camera_right.x);
        ImGui::End();
    }
#endif // USE_IMGUI

}

void Player::Render(GraphicsPipeline& graphics, float elapsed_time)
{
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, orientation, position), { 1.0f,1.0f,1.0f,1.0f });
}
