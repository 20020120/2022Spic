#include"MiddleBoss.h"

//****************************************************************
// 
// 中型のボスのステートマシンを定義する 
// 
//****************************************************************


void MiddleBoss::fStartInit()
{
    // ボス登場時のイベントシーン（未実装）
}

void MiddleBoss::fStartUpdate(float elapsedTime_)
{
    // ボス登場時のイベントシーン（未実装）
}

void MiddleBoss::fTourInit()
{
    // ステージの周囲を周回する動き
    mPosition = { 0.0f,150.0f,150.0f };
}

void MiddleBoss::fTourUpdate(float elapsedTime_)
{
    // ステージの原点を中心にぐるぐる回る

    // 位置計算
    mPosition = {
        sinf(mTourRadian) * TourLength,
        mPosition.y ,
        cosf(mTourRadian) * TourLength
    };

    // 角度を更新
    mTourRadian += elapsedTime_;

    // ビームテスト
    auto endPoint = mPlayerPosition;
    endPoint.y += 2.0f;
    mLaserBeam.fSetPosition(mPosition, endPoint);
    mLaserBeam.fSetRadius(0.02f);
}