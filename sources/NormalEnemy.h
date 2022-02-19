#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>
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

    NormalEnemy(ID3D11Device* pDevice_,
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/
    ,int UniqueId_
        );

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
    void fGuiMenu() override;
   
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
    void fAttackUpdate(float elapsedTime_); //�ړ��̍X�V����

    void fSetCapsulePoint() override{};
    enum State
    {
	    IDLE,
        MOVE,
        ATTACK
    };
    //****************************************************************
   // 
   // �ϐ� 
   // 
   //****************************************************************
    State mNowState;
    float mStayTimer;
    bool mAttack_flg;
};