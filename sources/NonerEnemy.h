#pragma once
#include"BaseEnemy.h"
//****************************************************************
// 
// �ړ������̃U�R�G 
// 
//****************************************************************


class NoneEnemy final : BaseEnemy
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
    NoneEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 Point_,int UniqueId_);
    ~NoneEnemy()override = default;
    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
protected:
    void fSetCapsulePoint() override;

private:
    void fMove(float elapsedTime_);
    void fRegisterFunctions() override;

    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    float mDistance{}; // �v���C���[�Ƃ̋����i���ȉ��ɂȂ�����ǐՂ��~�߂�j
    TimerComponent mTimerComponent{};

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


