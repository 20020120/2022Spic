#include "SkyDome.h"
#include"imgui_include.h"
SkyDome::SkyDome(GraphicsPipeline& graphics)
{
    model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\stage\\sky.fbx", true);
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
    ImGui::End();
#endif // USE_IMGUI

    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);

    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, angle, position), { 1,1,1,1 }, 0, 0, emissive_color);
}

bool SkyDome::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::ray_vs_model(start, end, model.get(), position, scale, angle, hit);
}
