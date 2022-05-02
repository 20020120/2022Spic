#pragma once
#include"BaseEnemy.h"
//****************************************************************
// 
// ���X�g�{�X
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
    // ��͂̍ŏ��̓����i���o�j
    void fShipStartInit();
    void fShipStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // ��͂̑ҋ@���
    void fShipIdleInit();
    void fShipIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // ��͂̍U��
    void fShipAttackInit();
    void fShipAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // ��͂̍Ō�̍U���i�����ς���Ύ��ցj
    void fShipBeamInit();
    void fShipBeamUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
};
