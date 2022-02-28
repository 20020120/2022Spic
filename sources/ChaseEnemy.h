#pragma once
#include"BaseEnemy.h"
#include "EnemiesEditor.h"
#include"SwordTrail.h"
//****************************************************************
// 
// �ړ������̃U�R�G 
// 
//****************************************************************

class ChaseEnemy final :public  BaseEnemy
{
    //enum State
    //{
    //    Start,        // �o��
    //    Chase,        // �ǐ�
    //    Intimidation, // �Њd
    //    Damaged,      // ��e
    //};

    struct  State : public BaseEnemy::StateTag
    {
        inline static const std::string Start = "Start";   
        inline static const std::string Intimidation = "Intimidation";
        inline static const std::string Chase = "Chase";
        
    };
    

    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    ChaseEnemy(GraphicsPipeline& graphics_, DirectX::XMFLOAT3 Point_,int UniqueId_, ParamGetFunction Function_);
    ~ChaseEnemy()override = default;
    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_) override;
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
    void fStartUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);        // �o�ꎞ
    void fChaseInit();                           // �ǐ�
    void fChaseUpdate(float elapsedTime_);        // �ǐ�
    void fIntimidationInit();                     // �Њd
    void fIntimidationUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // �Њd
public:
    void fGuiMenu() override;
};


 