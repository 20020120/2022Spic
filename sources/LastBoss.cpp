#include"LastBoss.h"

LastBoss::LastBoss(GraphicsPipeline& Graphics_, const DirectX::XMFLOAT3& EmitterPoint_,
    const EnemyParamPack& ParamPack_)
    :BaseEnemy(Graphics_,
        "./resources/Models/Enemy/boss_animation_second.fbx",
        ParamPack_, EmitterPoint_)
{}

LastBoss::LastBoss(GraphicsPipeline& Graphics_)
    : BaseEnemy(Graphics_, "./resources/Models/Enemy/boss_animation_second.fbx")
{}

LastBoss::~LastBoss()
{
}

void LastBoss::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fUpdateAttackCapsule()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fDie()
{
    BaseEnemy::fDie();
}

void LastBoss::fDamaged(int damage, float invincible_time)
{
    BaseEnemy::fDamaged(damage, invincible_time);
}

void LastBoss::fSetStun(bool arg)
{
    BaseEnemy::fSetStun(arg);
}

void LastBoss::fRegisterFunctions()
{
    throw std::logic_error("Not implemented");
}
