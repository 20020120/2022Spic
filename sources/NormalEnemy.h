#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>

#include "EnemiesEditor.h"
//****************************************************************
//
// ���Ȃ��ʏ�U���̎G���G�̔h���N���X
//
//****************************************************************
class NormalEnemy final :public BaseEnemy
{
    //****************************************************************
    //
    // �֐�
    //
    //****************************************************************
public:

    NormalEnemy(GraphicsPipeline& graphics_,
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/
    ,int UniqueId_, ParamGetFunction Function_
        );

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_) override;
    void fGuiMenu() override;

    void fDamaged(int damage_, float InvinsibleTime_) override;

    void fStopEffect() override;

private:
    // �X�e�[�g�}�V����ǉ�����֐�
    void fRegisterFunctions() override;
    //�p�����[�^�������֐�
    void fParamInitialize();

    //--------------------<�e�X�e�[�g�̊֐�>--------------------//
    void fIdleInit();   // �ҋ@�̏�����
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // �ҋ@�̍X�V����

    void fMoveInit(); //�ړ��̏�����
    void fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�ړ��̍X�V����

    void fAttackInit(); //�U���̏�����
    void fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�U���̍X�V����

    void fDamagedInit(); //�Ђ�݂̏�����
    void fDamagedUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�Ђ�݂̍X�V����


    // �X�e�[�g�̖��O���`����
    struct  State : public BaseEnemy::StateTag
    {
       inline static const std::string Attack = "Attack";
    };
    //****************************************************************
   //
   // �ϐ�
   //
   //****************************************************************
    State mNowState;
    float mStayTimer;
    float mAttackingTime;
    bool mAttack_flg;
};
