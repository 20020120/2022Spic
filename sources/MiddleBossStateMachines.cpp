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

void MiddleBoss::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // ボス登場時のイベントシーン（未実装）
}

void MiddleBoss::fTourInit()
{
    // ステージの周囲を周回する動き
    mPosition = { 0.0f,150.0f,150.0f };
}

void MiddleBoss::fTourUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
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
 
}

//--------------------<ビームの準備>--------------------//
void MiddleBoss::fTourLaserReadyInit()
{
    // 照準を初期化
    mLaserPointerLength = 0.0f;
    mLaserShotTimer = 0.0f;

}
void MiddleBoss::fTourLaserReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // ビームの照準を伸ばす
    if(mLaserPointerLength<1.0f)
    {
        mLaserPointerLength += elapsedTime_;
    }
    else // 最大なら固定
    {
        mLaserPointerLength = 1.0f;
    }
    // タイマーを更新する
    mLaserShotTimer += elapsedTime_;
    // 時間を過ぎたら発射する
    if (mLaserShotTimer >= TourLaserTimer)
    {
        fChangeState(State::TourBeam);
        return;
    }
    else
    {
        // 残り時間に応じて点滅速度を変える
        if (TourLaserTimer - mLaserShotTimer > 3.0f)
        {
            // 遅い
            switch (static_cast<int>(mLaserShotTimer*7) % 2)
            {
            case 0:
                mLaserPointer.fSetAlpha(1.0f);
                break;
            case 1:
                mLaserPointer.fSetAlpha(0.0f);
                break;
            default:;
            }
        }
        else
        {
            // 早い
            // 
            switch (static_cast<int>(mLaserShotTimer * 20) % 2)
            {
            case 0:
                mLaserPointer.fSetAlpha(1.0f);
                break;
            case 1:
                mLaserPointer.fSetAlpha(0.0f);
                break;
            default:;
            }
        }
    }
}
//--------------------<ビーム発射>--------------------//
void MiddleBoss::fTourLaserInit()
{
  
}
void MiddleBoss::fTourLaserUpdate(float elapsedTime_, GraphicsPipeline& Graphic_)
{
    
}

//--------------------<単発射撃>--------------------//
void MiddleBoss::fTourShotInit()
{
    
}
void MiddleBoss::fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}
