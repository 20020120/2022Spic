#pragma once
#include"BaseEnemy.h"
#include"BulletManager.h"
class BossUnit final : public BaseEnemy
{
public:
    BossUnit(GraphicsPipeline& Graphics_,
        const DirectX::XMFLOAT3& EntryPoint_,
        const EnemyParamPack& ParamPack_,
        AddBulletFunc fAddBullet_);
    BossUnit(GraphicsPipeline& Graphics_);

    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fUpdateAttackCapsule() override;
    bool fDamaged(int Damage_, float InvincibleTime_) override;
    void fSetStun(bool Arg_) override;
protected:
    void fRegisterFunctions() override;

private:
    //--------------------<ステートマシン>--------------------//



};
