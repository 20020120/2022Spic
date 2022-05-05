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
        //--------------------<���>--------------------//
        inline static const char* ShipStart  = "ShipStart";
        inline static const char* ShipIdle   = "ShipIdle";
        inline static const char* ShipAttack = "ShipAttack";
        inline static const char* ShipBeamStart   = "ShipBeamStart";
        inline static const char* ShipBeamCharge   = "ShipBeamCharge";
        inline static const char* ShipBeamShoot   = "ShipBeamShoot";
        inline static const char* ShipBeamEnd   = "ShipBeamEnd";
        inline static const char* ShipToHuman   = "ShipToHuman";

        //--------------------<�l�^>--------------------//
        inline static const char* HumanIdle = "HumanIdle";

    };

    enum  AnimationName {
        ship_idle,
        ship_beam_charge_start,
        ship_beam_charge_idle,
        ship_beam_shot_start,
        ship_beam_shot_idle,
        ship_to_human,
        human_idle,
        human_move,
        human_beam_charge,
        human_beam_shoot,
        human_beam_end,
        human_bullet,
        human_shockwave,
        human_damage,
        human_die,
        human_die_idle,
        human_to_dragon,
        dragon_idle,
        dragon_dive_ready,
        dragon_dive,
        dragon_dive_end,
        dragon_hide,
        dragon_beam_ready,
        dragon_beam_charge,
        dragon_beam_start,
        dragon_beam_idle,
        dragon_beam_end,
        dragon_breath_ready,
        dragon_breath_start,
        dragon_breath_idle,
        dragon_breath_end,
        dragon_damage,
        dragon_die,
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
     
    //--------------------<�^���b�g�̊֐�>--------------------//
    skeleton::bone mTurretBorn{}; // �^���b�g�̃{�[���̈ʒu
protected:
    void fRegisterFunctions() override;

private:
    void fGuiMenu();

private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    std::unique_ptr<SkinnedMesh> mpTurret{ nullptr }; // �^���b�g�̃��f��
    DirectX::XMFLOAT3 mTurretPosition{}; // �^���b�g�̈ʒu
    float mTimer{}; // �ėp�^�C�}�[

    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const float mkRotSpeed { 20.0f }; // ��]���x
    const float  mkShipBeamChargeSec{ 5.0f }; // ��͂̃r�[�����`���[�W���钷��
    const float  mkShipBeamShootSec{ 5.0f }; // ��͂̃r�[���𔭎˂��Ă��钷��
private:

    //****************************************************************
    // 
    // �X�e�[�g�}�V��
    // 
    //****************************************************************
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
    void fShipBeamStartInit();
    void fShipBeamStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fShipBeamChargeInit();
    void fShipBeamChargeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fShipBeamShootInit();
    void fShipBeamShootUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fShipBeamEndInit();
    void fShipBeamEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fChangeShipToHumanInit();
    void fChangeShipToHumanUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);

    //--------------------<�l�^>--------------------//
    void fHumanInit();
    void fHumanUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

};
