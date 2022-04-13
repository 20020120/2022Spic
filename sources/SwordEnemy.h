#pragma once
#include"BaseEnemy.h"
#include "EnemiesEditor.h"
#include"EventFlag.h"
class SwordEnemy :public BaseEnemy
{
public:
    //****************************************************************
    // 
    // �\����
    // 
    //****************************************************************
     struct DivedState: public StateTag
     {
         inline static const std::string AttackBegin = "AttackBegin"; // �U��グ
         inline static const std::string AttackMiddle = "AttackMiddle";  // ����
         inline static const std::string AttackEnd = "AttackEnd"; // �U�艺�낵
         inline static const std::string Die = "Die";
     };
     enum  AnimationName {
         idle,
         walk,
         attack_idle,
         attack_up,
         attack_down,
         damage,
     };
public:
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
    SwordEnemy(GraphicsPipeline& graphics_, int UniqueId_, 
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/,
        ParamGetFunction Func_);
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;

    void fRegisterFunctions() override; // �X�e�[�g��o�^

    void fSetAttackCapsuleCollider();

    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
private:
    float mWaitTimer{}; // �҂�����

    //****************************************************************
    // 
    // �萔 
    // 
    //****************************************************************
    const float mMoveSpeed{ 10.0f };      // �ړ����x
    const float mAttackRange{ 10.0f };    // �U���͈�
    const float mAttackDelaySec{ 1.0f };  // �U����̌��̒����i�b�j
    const float mSpawnDelaySec{ 1.0f };   // �o���̒����i�b�j

    //--------------------<�e�X�e�[�g�̑҂�����>--------------------//
    const float mAttackBeginTimeSec{ 0.85f };    // ����U�肠����܂ł̎���
    const float mAttackPreActionTimeSec{0.1f};   // ����U�艺�낷�\������ �@�@
    const float mAttackDownSec{ 1.0f };          // ����U�艺�낷

private:
    //****************************************************************
    // 
    // �X�e�[�g�}�V��
    // 
    //****************************************************************
    //--------------------<���̓G�̋��ʂ̓���>--------------------//
    // �X�|�[��
    void fSpawnInit();
    void fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
 
    // ����
    void fWalkInit();
    void fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // ����U��グ��
    void fAttackBeginInit();
    void fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // ����U�艺�낷�\������
    void fAttackPreActionInit();
    void fAttackPreActionUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);;

    // �U��
    void fAttackEndInit();
    void fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // ����
    void fDieInit();
    void fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
public:
    void fInitialize() override{}
};



