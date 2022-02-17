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
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/);

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
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


};
