#pragma once
#include"BaseEnemy.h"
//****************************************************************
// 
// 移動だけのザコ敵 
// 
//****************************************************************


class NoneEnemy final : BaseEnemy
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
    // 変数
    // 
    //****************************************************************
    float mDistance{}; // プレイヤーとの距離（一定以下になったら追跡を止める）
    TimerComponent mTimerComponent{};

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


