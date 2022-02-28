#pragma once
#include"BaseEnemy.h"
#include "EnemiesEditor.h"
#include"SwordTrail.h"
//****************************************************************
// 
// 移動だけのザコ敵 
// 
//****************************************************************

class ChaseEnemy final :public  BaseEnemy
{
    //enum State
    //{
    //    Start,        // 登場
    //    Chase,        // 追跡
    //    Intimidation, // 威嚇
    //    Damaged,      // 被弾
    //};

    struct  State : public BaseEnemy::StateTag
    {
        inline static const std::string Start = "Start";   
        inline static const std::string Intimidation = "Intimidation";
        inline static const std::string Chase = "Chase";
        
    };
    

    //****************************************************************
    // 
    // 関数
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
    // 変数
    // 
    //****************************************************************
    float mDistance{}; // プレイヤーとの距離（一定以下になったら追跡を止める）
    TimerComponent mTimerComponent{};
    DirectX::XMFLOAT3 mChaseDirection{};  // 追跡するときのベクトル
    DirectX::XMFLOAT3 mStartPosition{};   // 移動の始点
    DirectX::XMFLOAT3 mEndPosition{};     // 移動の終焉 

    //****************************************************************
    // 
    // ステートマシンの関数
    // 
    //****************************************************************

    void fStartInit();                            // 登場時
    void fStartUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);        // 登場時
    void fChaseInit();                           // 追跡
    void fChaseUpdate(float elapsedTime_);        // 追跡
    void fIntimidationInit();                     // 威嚇
    void fIntimidationUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // 威嚇
public:
    void fGuiMenu() override;
};


 