#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>
#include "Common.h"
#include "EnemiesEditor.h"
//****************************************************************
// 
// ���Ȃ��ʏ�U���̎G���G�̔h���N���X 
// 
//****************************************************************
class ArcherEnemy final :public BaseEnemy
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:

    ArcherEnemy(GraphicsPipeline& graphics_,
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/
        , int UniqueId_, ParamGetFunction Function_,  AddBulletFunc Func_
    );
    ~ArcherEnemy();

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fGuiMenu() override;

    void fDamaged(int damage_, float InvinsibleTime_) override;

    void fStopEffect() override;
private:
    // �X�e�[�g�}�V����ǉ�����֐�
    void fRegisterFunctions() override;
    //�p�����[�^�������֐�
    void fParamInitialize();

    //--------------------<�e�X�e�[�g�̊֐�>--------------------//
    void fSpawnInit();   // �o��̏�����
    void fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // �o��̍X�V����

    void fIdleInit();   // �ҋ@�̏�����
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // �ҋ@�̍X�V����

    void fMoveInit(); //�ړ��̏�����
    void fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�ړ��̍X�V����

	void fMoveApproachInit(); //�ڋ߈ړ��̏�����
    void fMoveApproachUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //��ވړ��̍X�V����

    void fMoveLeaveInit(); //�ڋ߈ړ��̏�����
    void fMoveLeaveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //��ވړ��̍X�V����

    void fSetVernierEffectPos();


    // �|������
    void fAttackBeginInit();
    void fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //�|�����đҋ@
    void fAttackPreActionInit();
    void fAttackPreActionUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //�|�����
    void fAttackEndInit();
    void fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fDamagedInit(); //�ړ��̏�����
    void fDamagedUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�Ђ�݂̍X�V����


    // �X�e�[�g�̖��O���`����
    struct  State : public BaseEnemy::StateTag
    {
        inline static const std::string AttackReady = "AttackReady";
        inline static const std::string AttackIdle = "AttackIdle";
        inline static const std::string AttackShot = "AttackShot";
        inline static const std::string Approach = "Approach";
        inline static const std::string Leave = "Leave";
    };
    enum  AnimationName {
        idle,
        walk,
        attack_ready,
        attack_idle,
        attack_shot,
        damage,
    };
    //****************************************************************
   // �@
   // �ϐ� 
   // 
   //****************************************************************
    State mNowState;
    float mStayTimer;
    float mAttackingTime;
    bool mAttack_flg;
    AddBulletFunc mfAddFunc;
    skeleton::bone mVernierBone{  };

    std::unique_ptr<Effect> mVernier_effect;

    //****************************************************************
   // �@
   // �萔 
   // 
   //****************************************************************
    //�o���̑ҋ@����
    const float SPAWN_STAY_TIME = 2.0f;
    //�v���C���[�ɍU�����J�n����ŒZ����
    const float AT_SHORTEST_DISTANCE = 30.0f;
    //�v���C���[�ɍU�����J�n����Œ�����
    const float AT_LONGEST_DISTANCE = 40.0f;
};

