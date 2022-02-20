#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>

#include "EnemiesEditor.h"
//****************************************************************
// 
// �e�X�g�ƃT���v�������˂��G�̔h���N���X 
// 
//****************************************************************
class TestEnemy final:public BaseEnemy
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    
    TestEnemy(ID3D11Device* pDevice_, 
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/,
        int UniqueId_,
        ParamGetFunction Function_);

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;

    void fGuiMenu()override;
    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
private:
    // �X�e�[�g�}�V����ǉ�����֐�
    void fRegisterFunctions() override;

    //--------------------<�e�X�e�[�g�̊֐�>--------------------//
    void fIdleInit();   // �ҋ@�̏�����
    void fIdleUpdate(float elapsedTime_); // �ҋ@�̍X�V����

    
protected:
    void fSetCapsulePoint() override;
};
