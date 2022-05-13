#pragma once
#include <random>

#include"BaseEnemy.h"
#include "Common.h"
#include"LaserBeam.h"
//****************************************************************
// 
// ���X�g�{�X
// 
//****************************************************************

  // �O���錾
class EnemyManager;

class LastBoss final : public BaseEnemy
{
public:
    enum class Mode // �{�X�̃��[�h
    {
        None,          // �����Ȃ��i���݂��Ȃ��j
        Ship,          // ��́i�_���[�W�͎󂯂Ȃ��j
        Human,         // �l�^�i�̗͂�100%~20%�j
        HumanToDragon, // �l�^����h���S���ɑJ�ڂ��Ă���r���i�_���[�W�͎󂯂Ȃ��j
        Dragon,        // �h���S���i�̗͂�20%~0%�j
    };

private:
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
        inline static const char* HumanBlowAttack = "HumanBlowAttack";

        inline static const char* HumanSpAway = "HumanSpAway"; // �����ɔ��
        inline static const char* HumanSpWait = "HumanSpWait"; // �҂��Ă���
        inline static const char* HumanSpOver = "HumanSpOver"; // ���Ԑ؂�
        inline static const char* HumanSpCharge = "HumanSpCharge"; // ����
        inline static const char* HumanSpShoot = "HumanSpShoot"; // ����
        inline static const char* HumanSpDamage = "HumanSpDamage"; // �_���[�W

        inline static const char* HumanRush = "HumanRush"; // �_���[�W


        //--------------------<�l�^�̎��S>--------------------//
        inline static const char* HumanDieStart = "HumanDieStart";   // �l�^�̎��S�J�n
        inline static const char* HumanDieMiddle = "HumanDieMiddle"; // �l�^�̎��S�ҋ@
        inline static const char* HumanToDragon = "HumanToDragon";   // �l�^����h���S��

        //--------------------<�h���S��>--------------------//
        inline static const char* DragonIdle = "DragonIdle"; // �ҋ@
        inline static const char* DragonDieStart = "DragonDieStart"; // ���S
        inline static const char* DragonDieEnd = "DragonDieEnd"; // ���S

        inline static const char* DragonHideStart= "DragonHideStart"; // ������
        inline static const char* DragonAppear= "DragonAppear"; // �����
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
        ship_to_human_quick,
        human_to_ship_quick,
        human_to_dragon_quick,
        dragon_to_human_quick,
    };

    enum class AttackKind // �������璊�o����U���̎��
    {
        //--------------------<�l�^>--------------------//
        HumanRotAttack,     // ��]�U��
        HumanAllShot,       // ����
        HumanSpecialAttack  // �K�E�Z
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
        void fSetDissolve(float Dissolve_);

        DirectX::XMFLOAT3 fGetPosition()const;
    private:
        DirectX::XMFLOAT4 mOrientation{0.0f,0.0f,0.0f,1.0f}; // �p�x
        std::unique_ptr<SkinnedMesh> mpModel{ nullptr };
        SkinnedMesh::anim_Parameters mAnimPara{};
        DirectX::XMFLOAT3 mPosition;
        float mDissolve{};
    };
  
    //--------------------<���C�g�̃N���X>--------------------//
    class SecondGun final
    {
    public :
        SecondGun(GraphicsPipeline& Graphics_);
        void fUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
        void fRender(GraphicsPipeline& graphics_,
            const DirectX::XMFLOAT4X4& ParentWorld_,
            const DirectX::XMFLOAT3& Position_);

        void fSetDissolve(float Dissolve_);
    private:
        std::unique_ptr<SkinnedMesh> mpModel{ nullptr };
        SkinnedMesh::anim_Parameters mAnimPara{};
        float mDissolve;
    };


public:
    LastBoss(GraphicsPipeline& Graphics_, 
             const DirectX::XMFLOAT3& EmitterPoint_,
             const EnemyParamPack& ParamPack_,
             EnemyManager* pEnemyManager_);

    LastBoss(GraphicsPipeline& Graphics_);
    ~LastBoss() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fUpdateAttackCapsule() override;
    
    void fSetStun(bool Arg_) override;
     
    //--------------------<�^���b�g�̊֐�>--------------------//
    skeleton::bone mShipFace{}; // �^���b�g�̃{�[���̈ʒu

protected:
    void fRegisterFunctions() override;

private:
    void fGuiMenu();

    void fResetLaser();

    [[nodiscard]] float fComputePercentHp() const; // �ő�̗͂ɑ΂��錻�݂̗̑͂̊�����0.0f~1.0f�ŕԂ�
    void fChangeHumanToDragon();

    void fSpawnChildUnit(GraphicsPipeline& Graphics_, int Amounts_) const;

    // �ړ�����
    bool fLimitStageHuman(float elapsedTime_);
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

    // �l�^�̃r�[��
    LaserBeam mRightBeam{};
    LaserBeam mLeftBeam{};
    float mRightBeamThreshold{};
    float mLeftBeamThreshold{};

    std::mt19937 mt{ std::random_device{}() };

    //--------------------<�{�X�̃^���b�g>--------------------//
    std::unique_ptr<Turret> mpTurretRight{ nullptr };
    std::unique_ptr<Turret> mpTurretLeft{ nullptr };
    skeleton::bone mTurretBoneRight{};  // �^���b�g�̃{�[��
    skeleton::bone mTurretBoneLeft{};   // �^���b�g�̃{�[��

    //--------------------<�{�X�̃Z�J���h�K��>--------------------//
    std::unique_ptr<SecondGun> mpSecondGunRight{ nullptr };
    std::unique_ptr<SecondGun> mpSecondGunLeft{ nullptr };
    skeleton::bone mSecondGunBoneRight{};  // �^���b�g�̃{�[��
    skeleton::bone mSecondGunBoneLeft{};   // �^���b�g�̃{�[��


    LaserBeam mLaserPointer{};
    DirectX::XMFLOAT3 mPointerBegin{}; // ���[�U�[�|�C���^�[�̎n�_
    DirectX::XMFLOAT3 mPointerEnd{};   // ���[�U�[�|�C���^�[�̏I�_
    float mPointerThreshold{};         // ���[�U�[�|�C���^�[�̒���0.0f~1.0f
    float mPointerAlpha{};
    float mRgbColorPower{};
    float mRgbColorSpeed{ 10.0f };
    float mHeartTimer{};

    //--------------------<�l�^�̕K�E�Z>--------------------//
    DirectX::XMFLOAT3 mAwayBegin{}; // ��т̂��n�_�̎n�_
    const DirectX::XMFLOAT3 mAwayEnd{0.0f,40.0f,200.0f}; // ��т̂��n�_�̏I�_
    float mAwayLerp{}; // Lerp�W��
    DirectX::XMFLOAT4 mBeginOrientation{};
    DirectX::XMFLOAT4 mEndOrientation{ 0.0f,0.0f,0.0f,1.0f };

    EnemyManager* mpEnemyManager{nullptr};

public:
    // ���݂̃��[�h
    Mode mCurrentMode{ Mode::Ship };

private:
    // �e��
    AddBulletFunc mfAddBullet{};
    float mShotTimer{ 0.0f };

    // �v���C���[���痣���
    DirectX::XMFLOAT3 mMoveBegin{};
    DirectX::XMFLOAT3 mMoveEnd{};
    float mMoveThreshold{};
    bool mFirstSp{ false };
    bool mSecondSp{ false };

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

    const float mkWaitHeartEffect = 0.5f;
    const float mkHumanAllShotDelay{ 0.1f };
    const float mkHumanAllShotEnd{ 1.3f };
    const float mkHumanAllShotBegin{ 0.3f };
   
    const float mkHumanSpWaitTime{ 15.0f };
    const float mkHumanSpBeamTime{ 5.0f };
    const float mkSpChargeTime = 7.0f;
    const float mkDistanceToPlayer{ 80.0f };
    const float mkLimitStage{ 300.0f };
    const float mkTimerBlow{ 0.8f };
    const float mkDragonHideTime{ 0.5f };
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
    void fShipAttackUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    // ��͂̍Ō�̍U���i�����ς���Ύ��ցj
    void fShipBeamStartInit();
    void fShipBeamStartUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fShipBeamChargeInit();
    void fShipBeamChargeUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fShipBeamShootInit();
    void fShipBeamShootUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fShipBeamEndInit();
    void fShipBeamEndUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fChangeShipToHumanInit();
    void fChangeShipToHumanUpdate(float elapsedTime_,
        GraphicsPipeline& Graphics_);

    //--------------------<�l�^>--------------------//
    void fHumanIdleInit();
    void fHumanIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanMoveInit();
    void fHumanMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanAllShotInit();
    void fHumanAllShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanBlowAttackInit();
    void fHumanBlowAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fMoveAwayInit();
    void fMoveAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanRushInit();
    void fHumanRushUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<�l�^�K�E�Z>--------------------//
    void fHumanSpAttackAwayInit(); // ��т̂�
    void fHumanSpAttackAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackSummonInit(); // �G������
    void fHumanSpAttackSummonUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackWaitInit(); // ��莞�ԑҋ@
    void fHumanSpAttackWaitUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackCancelInit(); // �S�G��|���ꂽ��C�x���g
    void fHumanSpAttackCancelUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackTimeOverInit(); // �������Ԃ𒴂�����U��
    void fHumanSpAttackTimeOverUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fHumanSpAttackChargeInit();
    void fHumanSpAttackChargeUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);

    void fHumanSpBeamShootInit();
    void fHumanSpBeamShootUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);
    
    void fHumanSpDamageInit();
    void fHumanSpDamageUpdate(float elapsedTime_,
        GraphicsPipeline& Graphics_);



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

    // �f�B�]���u�ŏ�����
    void fDragonFastBreathStartInit();
    void fDragonFastBreathStartUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);

    // �����
    void fDragonBreathAppearInit();
    void fDragonBreathAppearUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<�h���S�����S�G�t�F�N�g>--------------------//
    void fDragonDieStartInit();
    void fDragonDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fDragonDieMiddleInit();
    void fDragonDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    

public:
    void fRender(GraphicsPipeline& Graphics_) override;
};
