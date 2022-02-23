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
    void fUpdate(float elapsedTime_) override;
    void fGuiMenu() override;

    void fDamaged(int damage_, float InvinsibleTime_) override;
 
private:
    // �X�e�[�g�}�V����ǉ�����֐�
    void fRegisterFunctions() override;
    //�p�����[�^�������֐�
    void fParamInitialize();

    //--------------------<�e�X�e�[�g�̊֐�>--------------------//
    void fIdleInit();   // �ҋ@�̏�����
    void fIdleUpdate(float elapsedTime_); // �ҋ@�̍X�V����

    void fMoveInit(); //�ړ��̏�����
    void fmoveUpdate(float elapsedTime_); //�ړ��̍X�V����

    void fAttackInit(); //�ړ��̏�����
    void fAttackUpdate(float elapsedTime_); //�U���̍X�V����

    void fDamagedInit(); //�ړ��̏�����
    void fDamagedUpdate(float elapsedTime_); //�Ђ�݂̍X�V����

  
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
