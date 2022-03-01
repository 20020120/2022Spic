#pragma once
#include"BaseEnemy.h"
#include"LaserBeam.h"
#include"Common.h"
#include"UserLua.h"
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
        inline static const std::string TourBeamReady = "TourBeamReady";
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
    LaserBeam mLaserPointer{};   // 照準のレーザー
    LaserBeam mLaserBeam{};      // レーザービーム
    AddBulletFunc mfAddFunc;

private:  // ビームに関する変数
    float mLaserPointerLength{}; // 照準の長さ
    float mLaserBeamLength{};    // ビームの長さ
    float mBeamRadius{};         // ビームの太さ
    float mLaserShotTimer{};     // ビーム発射準備から発射での時間

private:  // 全体に関係する変数
    float mTimer{}; 



    //--------------------<Luaスクリプト>--------------------//
    LuaWorld mLuaWorld{};

    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************
    const float TourLaserTimer{ 6.0f }; // 発射までの時間
    const float TourBulletSpeed{ 500.0f };// 弾の速度 

private:
    //****************************************************************
    // 
    // ステートマシンの関数
    // 
    //****************************************************************
    //最初の登場シーン
    void fStartInit();  
    void fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 第一形態ステージの上をぐるぐる回る
    void fTourInit();
    void fTourUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 第一形態ビーム準備
    void fTourLaserReadyInit();
    void fTourLaserReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 第一形態ビーム
    void fTourLaserInit();
    void fTourLaserUpdate(float elapsedTime_, GraphicsPipeline& Graphic_);
    // 第一形態単発射撃
    void fTourShotInit();
    void fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);


public:
    void fRender(GraphicsPipeline& graphics_) override;
};
