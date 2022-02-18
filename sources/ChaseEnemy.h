#pragma once
#include"BaseEnemy.h"
//****************************************************************
// 
// �ړ������̃U�R�G 
// 
//****************************************************************


class ChaseEnemy final :public  BaseEnemy
{
    enum State
    {
        Start,       // �o��
        Chase,       // �ǐ�
        Intimidation // �Њd
    };

    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    ChaseEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 Point_,int UniqueId_);
    ~ChaseEnemy()override = default;
    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
protected:
    void fSetCapsulePoint() override;

private:
    void fMove(float elapsedTime_);
    void fRotate(float elapsedTime_);
    void fRegisterFunctions() override;

    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    float mDistance{}; // �v���C���[�Ƃ̋����i���ȉ��ɂȂ�����ǐՂ��~�߂�j
    TimerComponent mTimerComponent{};
    DirectX::XMFLOAT3 mChaseDirection{};  // �ǐՂ���Ƃ��̃x�N�g��
    DirectX::XMFLOAT3 mStartPosition{};   // �ړ��̎n�_
    DirectX::XMFLOAT3 mEndPosition{};     // �ړ��̏I�� 

    //****************************************************************
    // 
    // �X�e�[�g�}�V���̊֐�
    // 
    //****************************************************************

    void fStartInit();                            // �o�ꎞ
    void fStartUpdate(float elapsedTime_);        // �o�ꎞ
    void fChaseInit();                            // �ǐ�
    void fChaseUpdate(float elapsedTime_);        // �ǐ�
    void fIntimidationInit();                     // �Њd
    void fIntimidationUpdate(float elapsedTime_); // �Њd

};


 