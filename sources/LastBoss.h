#pragma once
#include"BaseEnemy.h"
//****************************************************************
// 
// ラストボス
// 
//****************************************************************

class LastBoss final : public BaseEnemy
{
    struct DivideState
    {
        inline static const char* ShipStart  = "ShipStart";
        inline static const char* ShipIdle   = "ShipIdle";
        inline static const char* ShipAttack = "ShipAttack";
        inline static const char* ShipBeam   = "ShipBeam";


    };
public:
    LastBoss(GraphicsPipeline& Graphics_, 
             const DirectX::XMFLOAT3& EmitterPoint_,
             const EnemyParamPack& ParamPack_);

    LastBoss(GraphicsPipeline& Graphics_);
    ~LastBoss() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fUpdateAttackCapsule() override;
    void fDie() override;
    void fDamaged(int Damage_, float InvincibleTime_) override;
    void fSetStun(bool Arg_) override;
protected:
    void fRegisterFunctions() override;

private:
    void fGuiMenu();

public:


private:
    // 戦艦の最初の動き（演出）
    void fShipStartInit();
    void fShipStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 戦艦の待機状態
    void fShipIdleInit();
    void fShipIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 戦艦の攻撃
    void fShipAttackInit();
    void fShipAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 戦艦の最後の攻撃（これを耐えれば次へ）
    void fShipBeamInit();
    void fShipBeamUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
};
