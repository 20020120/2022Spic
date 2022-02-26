#pragma once
#include"BaseEnemy.h"

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
    // 変数
    // 
    //****************************************************************
    float mTourRadian{}; // 周回中の回転角


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


};
