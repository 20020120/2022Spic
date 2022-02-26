#pragma once
#include"BaseEnemy.h"

//****************************************************************
// 
// ���{�X
// 
//****************************************************************
class MiddleBoss final : public BaseEnemy
{
public:
    //****************************************************************
    // 
    // �\����
    // 
    //****************************************************************
    struct  State: public StateTag
    {
        inline static const std::string Start = "Start";
        inline static const std::string Tour = "Tour";
        inline static const std::string TourShot = "TourShot";
        inline static const std::string TourBeam = "TourBeam";
    };


    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_);
    ~MiddleBoss() override = default;

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
    void fGuiMenu() override;
    void fDamaged(int Damage_, float InvinsibleTime_) override;
protected:
    void fSetCapsulePoint() override;
    void fRegisterFunctions() override;
private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    float mTourRadian{}; // ���񒆂̉�]�p


    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const float TourLength{ 150.0f };  // ���񒆂̌��_����̋���


private:
    //****************************************************************
    // 
    // �X�e�[�g�}�V���̊֐�
    // 
    //****************************************************************
    void fStartInit();
    void fStartUpdate(float elapsedTime_);

    void fTourInit();
    void fTourUpdate(float elapsedTime_);


};
