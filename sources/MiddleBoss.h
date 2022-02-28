#pragma once
#include"BaseEnemy.h"
#include"LaserBeam.h"
#include"Common.h"
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
    MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_,AddBulletFunc Func_);
    ~MiddleBoss() override = default;

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_) override;
    void fGuiMenu(GraphicsPipeline& Graphics_);
    void fDamaged(int Damage_, float InvinsibleTime_) override;
protected:
    void fSetCapsulePoint() override;
    void fRegisterFunctions() override;
    void fShotStraightBullet(GraphicsPipeline& Graphics_); // ���i����e���͂�����
private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    float mTourRadian{}; // ���񒆂̉�]�p
    LaserBeam mLaserBeam{};
    AddBulletFunc mfAddFunc;

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
public:
    void fRender(GraphicsPipeline& graphics_) override;
};
