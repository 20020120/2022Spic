#include "SkyDome.h"
#include"imgui_include.h"

float SkyDome::purple_threshold = 0.0f;
float SkyDome::red_threshold = 0.0f;

SkyDome::SkyDome(GraphicsPipeline& graphics)
{
    std::string sub_colors[2] =
    {
        "sky.fbm\\sky_purple.png",
        "sky.fbm\\sky_red.png",
    };

    model = std::make_shared<SkinnedMesh>(graphics.get_device().Get(), ".\\resources\\Models\\stage\\sky.fbx", sub_colors, true, 0.0f);
}

SkyDome::~SkyDome()
{
}

void SkyDome::Render(GraphicsPipeline& graphics, float elapsed_time)
{

#ifdef USE_IMGUI
    ImGui::Begin("SkyDome");
    ImGui::DragFloat4("emissive_color", &emissive_color.x);
    ImGui::DragFloat3("angle", &angle.x);
    ImGui::DragFloat("purple_threshold", &purple_threshold, 0.01f);
    ImGui::DragFloat("red_threshold", &red_threshold, 0.01f);
    ImGui::End();
#endif // USE_IMGUI

    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);

    std::string sub_color_mesh_name = "pSphere1";
    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, angle, position),
        { 1,1,1,1 }, 0, 0, emissive_color, 0.8f, purple_threshold, red_threshold, sub_color_mesh_name);
}

bool SkyDome::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::ray_vs_model(start, end, model.get(), position, scale, angle, hit);
}
