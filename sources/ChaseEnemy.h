#pragma once
#include"BaseEnemy.h"
//****************************************************************
// 
// 移動だけのザコ敵 
// 
//****************************************************************


class ChaseEnemy final :public  BaseEnemy
{
    enum State
    {
        Start,       // 登場
        Chase,       // 追跡
        Intimidation // 威嚇
    };

    //****************************************************************
    // 
    // 関数
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
    void fStartUpdate(float elapsedTime_);        // 登場時
    void fChaseInit();                            // 追跡
    void fChaseUpdate(float elapsedTime_);        // 追跡
    void fIntimidationInit();                     // 威嚇
    void fIntimidationUpdate(float elapsedTime_); // 威嚇

};


 