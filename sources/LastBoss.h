#pragma once
#include"BaseEnemy.h"
#include"LaserBeam.h"
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
        inline static const char* HumanIdle = "HumanIdle"; // �ҋ@
        inline static const char* HumanMove = "HumanMove"; // �ړ�
        inline static const char* HumanAllShot = "HumanAllShot"; // ���˂���
        inline static const char* HumanRotAttack = "HumanRotAttack"; // ��]�U��
        inline static const char* HumanWithdrawal_Begin = "HumanWithdrawal_Begin"; // ��U�Δ䂷��
        inline static const char* HumanWithdrawal_Middle = "HumanWithdrawal_Middle"; // �͈͊O�ő҂�
        // ��莞�ԓ��Ɏ艺���R�U�点�Ȃ���ΕK�E�Z
        inline static const char* HumanWithdrawal_End = "HumanWithdrawal_End";

        //--------------------<�l�^�̎��S>--------------------//
        inline static const char* HumanDieStart = "HumanDieStart";   // �l�^�̎��S�J�n
        inline static const char* HumanDieMiddle = "HumanDieMiddle"; // �l�^�̎��S�ҋ@
        inline static const char* HumanToDragon = "HumanToDragon";   // �l�^����h���S��

        //--------------------<�h���S��>--------------------//
        inline static const char* DragonIdle = "DragonIdle"; // �ҋ@
        inline static const char* DragonDieStart = "DragonDieStart"; // ���S
        inline static const char* DragonDieEnd = "DragonDieEnd"; // ���S
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

    enum class AttackKind // �������璊�o����U���̎��
    {
        //--------------------<�l�^>--------------------//
        HumanRotAttack,     // ��]�U��
        HumanAllShot,       // ����
        HumanSpecialAttack  // �K�E�Z
    };

    enum class Mode // �{�X�̃��[�h
    {
        Ship,          // ��́i�_���[�W�͎󂯂Ȃ��j
        Human,         // �l�^�i�̗͂�100%~20%�j
        HumanToDragon, // �l�^����h���S���ɑJ�ڂ��Ă���r���i�_���[�W�͎󂯂Ȃ��j
        Dragon,        // �h���S���i�̗͂�20%~0%�j
    };
    //--------------------<�C�g�̃N���X>--------------------//
    class Turret final
    {
    public:
        Turret(GraphicsPipeline& Graphics_);
        void fUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
        void fRender(GraphicsPipeline& graphics_, 
                     const DirectX::XMFLOAT4X4& ParentWorld_,
                     const DirectX::XMFLOAT3& Position_);

    private:
        DirectX::XMFLOAT4 mOrientation{0.0f,0.0f,0.0f,1.0f}; // �p�x
        std::unique_ptr<SkinnedMesh> mpModel{ nullptr };
        SkinnedMesh::anim_Parameters mAnimPara{};
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
    skeleton::bone mShipFace{}; // �^���b�g�̃{�[���̈ʒu

protected:
    void fRegisterFunctions() override;

private:
    void fGuiMenu();


    AttackKind fRandomAttackFromHp(); // ���݂̗̑͂̊�������s�������߂�
    [[nodiscard]] float fComputePercentHp() const; // �ő�̗͂ɑ΂��錻�݂̗̑͂̊�����0.0f~1.0f�ŕԂ�
    void fChangeHumanToDragon();

private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    float mTimer{}; // �ėp�^�C�}�[

    // �r�[��
    LaserBeam mBeam{};
    DirectX::XMFLOAT3 mLaserBegin{};   // �r�[���̎n�_
    DirectX::XMFLOAT3 mLaserEnd{};     // �r�[���̏I�_
    float mLaserThreshold{};           // �r�[���̒���0.0f~1.0f
    float mLaserAlpha{};
    float mLaserRadius{};              // �r�[���̑���

    //--------------------<�{�X�̃^���b�g>--------------------//
    std::unique_ptr<Turret> mpTurretRight{ nullptr };
    std::unique_ptr<Turret> mpTurretLeft{ nullptr };
    skeleton::bone mTurretBoneRight{};  // �^���b�g�̃{�[��
    skeleton::bone mTurretBoneLeft{};   // �^���b�g�̃{�[��


    LaserBeam mLaserPointer{};
    DirectX::XMFLOAT3 mPointerBegin{}; // ���[�U�[�|�C���^�[�̎n�_
    DirectX::XMFLOAT3 mPointerEnd{};   // ���[�U�[�|�C���^�[�̏I�_
    float mPointerThreshold{};         // ���[�U�[�|�C���^�[�̒���0.0f~1.0f
    float mPointerAlpha{};

    // ���݂̃��[�h
    Mode mCurrentMode{ Mode::Ship };

    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const float mkRotSpeed { 20.0f }; // ��]���x
    const float  mkShipBeamChargeSec{ 5.0f }; // ��͂̃r�[�����`���[�W���钷��
    const float  mkShipBeamShootSec{ 5.0f }; // ��͂̃r�[���𔭎˂��Ă��钷��
    const float  mkHumanDieIdleSec{ 5.0f };  // �l�^�̎��S����

    const float mkPercentToDragon{ 0.2f }; // �h���S���`�ԂɑJ�ڂ���̗͂̊���
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
    void fHumanIdleInit();
    void fHumanIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanMoveInit();
    void fHumanMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanAllShotInit();
    void fHumanAllShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanRotAttackInit();
    void fHumanRotAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<�l�^�̎��S�G�t�F�N�g>--------------------//
    void fHumanDieStartInit();
    void fHumanDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanDieMiddleInit();
    void fHumanDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanToDragonInit();
    void fHumanToDragonUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<�h���S��>--------------------//
    void fDragonIdleInit();
    void fDragonIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<�h���S�����S�G�t�F�N�g>--------------------//
    void fDragonDieStartInit();
    void fDragonDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fDragonDieMiddleInit();
    void fDragonDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    

public:
    void fRender(GraphicsPipeline& Graphics_) override;
};
