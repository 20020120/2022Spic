#pragma once
#include"BaseEnemy.h"
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
         inline static const std::string AttackBegin = "AttackBegin";
         inline static const std::string AttackMiddle = "AttackMiddle";
         inline static const std::string AttackEnd = "AttackEnd";
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
    SwordEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/);
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;

    void fRegisterFunctions() override; // �X�e�[�g��o�^

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

    // �U���͂���
    void fAttackBeginInit();
    void fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // �U��
    void fAttackInit();
    void fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // �U���I���
    void fAttackEndInit();
    void fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // ����
    void fDieInit();
    void fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
public:
    void fInitialize() override{};
};



