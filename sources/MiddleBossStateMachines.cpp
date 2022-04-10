#include"MiddleBoss.h"
#include"Operators.h"
#include"StraightBullet.h"
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
    // 回転前の基準ベクトル
    DirectX::XMFLOAT3 baseForward{ 0.0f,0.0f,1.0f };
    // 回転後のベクトル
    DirectX::XMFLOAT3 forward{};

    DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(0.0f, 150.0f, 0.0f) - mPosition;
    up = Math::Normalize(up);

    // ステージの原点を中心にぐるぐる回る
    mLuaWorld.fDestroyStack();
    mLuaWorld.fSetFunction("fTourMove");
    mLuaWorld.fSetNumeric(elapsedTime_);
    mLuaWorld.fCallFunc(1, 4);

    // 値をLuaから受け取り
    mPosition.x = static_cast<float>(mLuaWorld.fGetDouble(1));
    mPosition.z = static_cast<float>(mLuaWorld.fGetDouble(2));
    forward.x = static_cast<float>(mLuaWorld.fGetDouble(3));
    forward.z = static_cast<float>(mLuaWorld.fGetDouble(4));

    // 正規化
    baseForward = Math::Normalize(baseForward);
    forward = Math::Normalize(forward);

    // 回転量を算出
    float dot = Math::Dot(baseForward, forward);
    dot = acosf(dot);

    // 前と上のベクトルから回転軸を算出
    auto rotAxis = Math::Cross(baseForward, forward, true);

    mOrientation = Math::RotQuaternion(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), rotAxis, dot);


    //// 各攻撃に遷移するときの条件
    //// ５秒ごとに抽選
    //if(static_cast<int>(mTimer)%5==0)
    //{
    //    const int selectedNum = rand() % 4;
    //    if(selectedNum==0)
    //    {
    //        fChangeState(State::TourBeamReady);
    //    }
    //    else if(selectedNum<=2)
    //    {
    //        fChangeState(State::TourShot);
    //    }

    //}


}

//--------------------<ビームの準備>--------------------//
void MiddleBoss::fTourLaserReadyInit()
{
    // 照準を初期化
    mLaserBeamLength = 0.0f;
    mLaserShotTimer = 0.0f;
    mLuaWorld.fSetFunction("fTourLaserReadyInit");
    mLuaWorld.fCallFunc(0, 1);
}
void MiddleBoss::fTourLaserReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mLuaWorld.fDestroyStack();
    mLuaWorld.fSetFunction("fTourLaserReady");
    mLuaWorld.fSetNumeric(elapsedTime_);
    mLuaWorld.fCallFunc(1, 3);

    //mLuaWorld.fDebugShowStack();
    // ポインターの透明度を更新
    mLaserPointer.fSetAlpha(mLuaWorld.fGetDouble(-2));
    // ポインターの長さを更新
    mLaserPointer.fSetLengthThreshold(mLuaWorld.fGetDouble(-1));

    // 条件を満たしていたらステートを変更
    if (mLuaWorld.fGetBool(-3))
    {
        fChangeState(State::TourBeam);
        mLaserPointer.fSetLengthThreshold(0.0f);
    }
}
//--------------------<ビーム発射>--------------------//
void MiddleBoss::fTourLaserInit()
{
    mLaserPointerLength = 0.0f;
    mLaserBeamLength = 0.0f;
    mLaserShotTimer = 0.0f;
    mBeamRadius = 3.0f;
    // 終点を計算
    auto endPoint = mPlayerPosition;
    endPoint.y += 1.5f;

    // ベクトルを算出
    DirectX::XMFLOAT3 v = endPoint - mPosition;
    v = Math::Normalize(v);
    endPoint = mPosition + (v * 500.0f);

    mLaserBeam.fSetPosition(mPosition, endPoint);
}
void MiddleBoss::fTourLaserUpdate(float elapsedTime_, GraphicsPipeline& Graphic_)
{
    // ビームを伸ばす
    if (mLaserBeamLength < 1.0f)
    {
        mLaserBeamLength += (elapsedTime_ * 5.0f);
    }
    else // 最大なら固定
    {
        mLaserBeamLength = 1.0f;
    }


    mLaserShotTimer += elapsedTime_;
    if(mLaserShotTimer>=5.0f)
    {
        fChangeState(State::Tour);
    }
    else if(mLaserShotTimer>=3.0f)
    {
        if (mBeamRadius > 0.0f)
        {
            mBeamRadius -= (elapsedTime_ * 10.0f);
        }
        else
        {
            mBeamRadius = 0.0f;
        }
    }
}

//--------------------<単発射撃>--------------------//
void MiddleBoss::fTourShotInit()
{
    // 初期化
}
void MiddleBoss::fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // 射撃方向を算出
    fShotToTarget(mPlayerPosition,Graphics_);
    fChangeState(State::Tour);

}

void MiddleBoss::fTourToSecondModeInit()
{
    // 呼び出された敵がいなくなったら

    // 位置をステージ正面に移動
    mPosition = { 0.0f,0.0f,150.0f };

    // 変形
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::inv_transform);
}

void MiddleBoss::fTourToSecondModeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // 変形が終了したら待機状態に遷移
    if (mpSkinnedMesh->end_of_animation(mAnimPara))
    {
        fChangeState(State::SecondIdle);
    }
}

void MiddleBoss::fSecondIdleInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::wait, true);
}

void MiddleBoss::fSecondIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

}

void MiddleBoss::fSecondShotInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::Shot_Recoil);
    mShotTimer = 0.0f;

}

void MiddleBoss::fSecondShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mShotTimer += elapsedTime_;

    if (mShotTimer > 0.6f)
    {
        fShotToTarget(mPlayerPosition,Graphics_);
        // 次の状態に遷移させるかどうかのフラグ
        mLuaWorld.fSetFunction("SelectedNextState");
        mLuaWorld.fCallFunc(0, 1);
        const bool goNextState = mLuaWorld.fGetBool(-1);

        fChangeState(goNextState ? State::SecondIdle : State::SecondShot);
    }
}
