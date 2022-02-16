#include "SkyDome.h"

SkyDome::SkyDome(GraphicsPipeline& graphics)
{
    model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\stage\\back_proto.fbx", false);
}

SkyDome::~SkyDome()
{
}

void SkyDome::Render(GraphicsPipeline& graphics, float elapsed_time)
{
    model->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, angle, position), { 1,1,1,1 });

}
