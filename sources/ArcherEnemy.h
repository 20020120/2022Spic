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

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fGuiMenu() override;

    void fDamaged(int damage_, float InvinsibleTime_) override;

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

	void fMoveApproachInit(); //�ڋ߈ړ��̏�����
    void fMoveApproachUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //��ވړ��̍X�V����

    void fMoveLeaveInit(); //�ڋ߈ړ��̏�����
    void fMoveLeaveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //��ވړ��̍X�V����

    void fAttackInit(); //�ړ��̏�����
    void fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�U���̍X�V����

    void fDamagedInit(); //�ړ��̏�����
    void fDamagedUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //�Ђ�݂̍X�V����


    // �X�e�[�g�̖��O���`����
    struct  State : public BaseEnemy::StateTag
    {
        inline static const std::string Attack = "Attack";
        inline static const std::string Approach = "Approach";
        inline static const std::string Leave = "Leave";
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

};

