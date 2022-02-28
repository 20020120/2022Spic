#include "BaseBullet.h"
#include "effect_manager.h"
BaseBullet::BaseBullet(GraphicsPipeline& Graphics_, const char* FileName_)
{
    mpEffect = std::make_unique<Effect>(
        Graphics_,
        effect_manager->get_effekseer_manager(),
        FileName_);
}

void BaseBullet::fUpdate(float elapsedTime_)
{
    mpEffect->set_position(effect_manager->get_effekseer_manager(), mPosition);
    mpEffect->set_quaternion(effect_manager->get_effekseer_manager(), mOrientation);
    mpEffect->set_scale(effect_manager->get_effekseer_manager(), mScale);
}