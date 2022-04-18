#pragma once
#include"BaseEnemy.h"
#include "EnemiesEditor.h"

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
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/,
        ParamGetFunction Func_);
    ~SpearEnemy() override;

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fStopEffect() override;
protected:
    void fRegisterFunctions() override;

private:
    //****************************************************************
    // 
    //  �ϐ�
    // 
    //****************************************************************
    float mWaitTime{}; // �ҋ@����
    DirectX::XMFLOAT3 mThrustTarget{}; // �ːi���̃^�[�Q�b�g
private:
    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const float mIdleSec{ 4.0f };        // �ҋ@����
    const float mStartSec{ 1.0f };        // �ҋ@����
    const float mAttackLength{ 100.0f }; // �v���C���[�Ƃ̋��������̋����ȉ��ɂȂ�����
    const float mMoveSpeed{ 20.0f };     // �ړ����x
    const float mThrustBeginSec{ 1.0f }; // �ːi�����̎���
    const float mThrustMiddleSec{ 0.2f };// ����˂��o������
    const float mThrustEndSec{ 3.0f };   // �ːi���̒���
    const float mThrustSpeed{ 100.0f };   // �ːi�̑���
    const float mThrustDegree{ 30.0f };  // �ːi�̊p�x
private:
    //****************************************************************
    // 
    // �X�e�[�g�}�V��
    // 
    //****************************************************************
    //--------------------<�J�n��>--------------------//
    void fStartInit();
    void fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
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

