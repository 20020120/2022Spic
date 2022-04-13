#pragma once

#include"BaseEnemy.h"

class SpearEnemy final  : public BaseEnemy
{
    //****************************************************************
    // 
    //  �\����
    // 
    //****************************************************************
    struct DivedState : public StateTag
    {
        inline static const std::string ThrustBegin = "ThrustBegin";    // �ːi�O�̍\��
        inline static const std::string ThrustMiddle = "ThrustMiddle";  // �ːi��
        inline static const std::string ThrustEnd = "ThrustEnd";        // �ːi��̌�
        inline static const std::string Die = "Die";
    };
    enum AnimationName {
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
    SpearEnemy(GraphicsPipeline& graphics_, 
        int UniqueId_, 
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/);
    ~SpearEnemy() override;

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
protected:
    void fRegisterFunctions() override;

private:
    //****************************************************************
    // 
    //  �ϐ�
    // 
    //****************************************************************
    float mWaitTime{}; // �ҋ@����
private:
    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const float mIdleSec{ 2.0f };       // �ҋ@����
    const float mAttackLength{ 30.0f }; // �v���C���[�Ƃ̋��������̋����ȉ��ɂȂ�����
    const float mMoveSpeed{ 20.0f };    // �ړ����x
    const float mThrustBeginSec{ 1.0f }; // �ːi�����̎���

private:
    //****************************************************************
    // 
    // �X�e�[�g�}�V��
    // 
    //****************************************************************
    //--------------------<�ҋ@>--------------------//
    void fIdleInit();
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<�ړ�>--------------------//
    void fMoveInit();
    void fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<�˂��\������>--------------------//
    void fThrustBeginInit();
    void fThrustBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<�˂���>--------------------//
    void fThrustMiddleInit();
    void fThrustMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<�˂��̌㌄>--------------------//
    void fThrustEndInit();
    void fThrustEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<�_���[�W>--------------------//
    void fDamageInit();
    void fDamageUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<���S>--------------------//
    void fDieInit();
    void fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

};
