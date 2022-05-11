#include"BossUnit.h"
BossUnit::BossUnit(GraphicsPipeline& Graphics_,
    const DirectX::XMFLOAT3& EntryPoint_,
    const EnemyParamPack& ParamPack_,
    AddBulletFunc fAddBullet_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/boss_unit.fbx",
        ParamPack_, EntryPoint_)
{
    mScale = { 0.05f,0.05f,0.05f };
    mDissolve = 0.0f;
}

BossUnit::BossUnit(GraphicsPipeline& Graphics_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/boss_unit.fbx")
{}

void BossUnit::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    
}

void BossUnit::fUpdateAttackCapsule()
{
    throw std::logic_error("Not implemented");
}

bool BossUnit::fDamaged(int damage, float invincible_time)
{
    return BaseEnemy::fDamaged(damage, invincible_time);
}

void BossUnit::fSetStun(bool arg)
{
    BaseEnemy::fSetStun(arg);
}

void BossUnit::fRegisterFunctions()
{
    throw std::logic_error("Not implemented");
}
