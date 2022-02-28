#pragma once
#include"BaseEnemy.h"
#include"LaserBeam.h"
#include"Common.h"
//****************************************************************
// 
// 中ボス
// 
//****************************************************************
class MiddleBoss final : public BaseEnemy
{
public:
    //****************************************************************
    // 
    // 構造体
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
    // 関数
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
    void fShotStraightBullet(GraphicsPipeline& Graphics_); // 直進する弾をはっしゃ
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    float mTourRadian{}; // 周回中の回転角
    LaserBeam mLaserBeam{};
    AddBulletFunc mfAddFunc;

    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************
    const float TourLength{ 150.0f };  // 周回中の原点からの距離


private:
    //****************************************************************
    // 
    // ステートマシンの関数
    // 
    //****************************************************************
    void fStartInit();
    void fStartUpdate(float elapsedTime_);

    void fTourInit();
    void fTourUpdate(float elapsedTime_);
public:
    void fRender(GraphicsPipeline& graphics_) override;
};
