#include"LastBoss.h"
#include"Operators.h"
#include"post_effect.h"
#include"BulletManager.h"
#include "CannonballBullet.h"
//****************************************************************
// 
// 戦艦モード 
// 
//****************************************************************
void LastBoss::fShipStartInit()
{
    mDissolve = 1.0f;
}

void LastBoss::fShipStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve -= elapsedTime_ * 2.0f;
    mDissolve = (std::max)(0.0f, mDissolve);
    if (mDissolve <= 0.0f)
    {
        fChangeState(DivideState::ShipIdle);
    }
}

void LastBoss::fShipIdleInit()
{
  //mpModel->play_animation(mAnimPara)
}

void LastBoss::fShipIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
   
}

void LastBoss::fShipAttackInit()
{
  
}

void LastBoss::fShipAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

void LastBoss::fShipBeamStartInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_charge_start);

}

void LastBoss::fShipBeamStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // プレイヤーの方向に回転
    fTurnToPlayer(elapsedTime_, mkRotSpeed);

    // アニメーション終了と同時に遷移
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::ShipBeamCharge);
    }
}

void LastBoss::fShipBeamChargeInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_charge_idle,true);
    mTimer = mkShipBeamChargeSec;

    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    mpModel->fech_by_bone(mAnimPara, world, mShipFace, mPointerBegin, up);

    //--------------------<レーザーポインターを初期化>--------------------//
    mPointerEnd = mPointerBegin;
    // 前方にポインターを伸ばす
    const auto front = Math::GetFront(mOrientation);
    mPointerEnd = mPointerBegin + front * 100.0f;
    mLaserPointer.fSetPosition(mPointerBegin, mPointerEnd);
    mLaserPointer.fSetRadius(0.035f);
    mLaserPointer.fSetColor({ 1.0f,0.0f,0.0f ,1.0f });
    mPointerAlpha = 1.0f;
    mPointerThreshold = 0.0f;
}

void LastBoss::fShipBeamChargeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    // レーザーポインターを伸ばす
    mPointerThreshold += elapsedTime_ * 5.0f;
    mLaserPointer.fSetLengthThreshold(mPointerThreshold);

    // TODO : ここで小さめのカメラシェイク
    mTimer -= elapsedTime_;
    if(mTimer<=0.0f)
    {
        // 発射
        fChangeState(DivideState::ShipBeamShoot);
    }
}

void LastBoss::fShipBeamShootInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_shot_start);
    mTimer = mkShipBeamShootSec;

    //--------------------<レーザーポインターを消す>--------------------//
    mPointerThreshold = 0.0f;
    mLaserPointer.fSetPosition(mPointerBegin, mPointerBegin);
    mLaserPointer.fSetRadius(0.0f);
    mLaserPointer.fSetColor({ 1.0f,0.0f,0.0f ,1.0f });

    //--------------------<レーザービームを発射する>--------------------//
    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    // ボーンの位置を取得
    mpModel->fech_by_bone(mAnimPara, world, mShipFace, mPointerBegin, up);
    const auto front = Math::GetFront(mOrientation);
    mLaserEnd = mLaserBegin + front * 100.0f;
    mLaserThreshold = 0.0f;
    mLaserRadius = 20.0f;
    mBeam.fSetAlpha(1.0f);
    mBeam.fSetPosition(mLaserBegin, mLaserEnd);
    mBeam.fSetLengthThreshold(mLaserThreshold);
    mBeam.fSetColor({ 0.0f,0.0f,1.0f,1.0f });
    mBeam.fSetRadius(mLaserRadius);
}

void LastBoss::fShipBeamShootUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mLaserThreshold += elapsedTime_ * 20.0f;
    mBeam.fSetLengthThreshold(mLaserThreshold);

    // TODO : ここで激しいカメラシェイク
    mTimer -= elapsedTime_;
    if(mTimer<=0.0f)
    {
        fChangeState(DivideState::ShipBeamEnd);
    }
}

void LastBoss::fShipBeamEndInit()
{
    
}

void LastBoss::fShipBeamEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mLaserRadius -= elapsedTime_ * 10.0f;
    mLaserRadius = (std::max)(0.0f, mLaserRadius);
    mBeam.fSetRadius(mLaserRadius);

    if (mLaserRadius <= 0.0f)
    {
        fChangeState(DivideState::ShipToHuman);
    }
}

void LastBoss::fChangeShipToHumanInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_to_human);
}

void LastBoss::fChangeShipToHumanUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::HumanIdle);

        // 現在のモードを人型に変更する
        mCurrentMode = Mode::Human;
    }
}

void LastBoss::fHumanIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_idle, true);
}

void LastBoss::fHumanIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
   // 条件に応じて攻撃手段を分岐させる

}

void LastBoss::fHumanMoveInit()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanMoveUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanAllShotInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_beam_charge);

    // テストで撃ってみる

}

void LastBoss::fHumanAllShotUpdate(float elapsedTime_,
    GraphicsPipeline& Graphics_)
{
    // ボーンが向いている方向を計算
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    DirectX::XMFLOAT4X4 rotMat{};
    mpModel->fech_by_bone(mAnimPara,Math::calc_world_matrix(mScale,
        mOrientation, mPosition), mTurretBoneLeft, position,
        up,rotMat);
    const DirectX::XMFLOAT3 front = { rotMat._31,rotMat._32,rotMat._33 };

    if(mpModel->end_of_animation(mAnimPara))
    {
        mfAddBullet(new CannonballBullet(Graphics_, front,
            300.0f, position));
        fChangeState(DivideState::HumanIdle);
    }
}

void LastBoss::fHumanRotAttackInit()
{

}

void LastBoss::fHumanRotAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanDieStartInit()
{
    // 位置をリセット
    mPosition = { 0.0f,0.0f,20.0f };
    mpModel->play_animation(mAnimPara, AnimationName::human_die);

    // TODO : エフェクトの類をすべてリセットする

    // TODO : カメラをボスの方に向ける

}

void LastBoss::fHumanDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
   if(mpModel->end_of_animation(mAnimPara))
   {
       fChangeState(DivideState::HumanDieMiddle);
   }
}

void LastBoss::fHumanDieMiddleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_die_idle, true);
    mTimer = mkHumanDieIdleSec;
    mRgbColorPower = 0.0f;
    mHeartTimer = 0.0f;
}

void LastBoss::fHumanDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // RGBずらしで心臓のどくどくを演出する
    mTimer -= elapsedTime_;
<<<<<<< Updated upstream
    
    //PostEffect::boss_awakening_effect({0.5f,0.5f},)
=======

    if(mHeartTimer<=0.0f)
    {
        mRgbColorPower += mRgbColorSpeed * elapsedTime_;
        if (mRgbColorPower > 1.0f)
        {
            mRgbColorSpeed *= -1.0f;
        }
        if (mRgbColorPower < 0.0f)
        {
            //  一周したら待機する
            mHeartTimer = mkWaitHeartEffect;
            mRgbColorSpeed *= -1.0f;
        }
    }
    else
    {
        mHeartTimer -= elapsedTime_;
    }
   
  

    PostEffect::boss_awakening_effect({ 0.5f,0.5f }, mRgbColorPower);
>>>>>>> Stashed changes
    if(mTimer<=0.0f)
    {
        fChangeState(DivideState::HumanToDragon);
        PostEffect::clear_post_effect();
    }
}

void LastBoss::fHumanToDragonInit()
{
    mpModel->play_animation(mAnimPara,AnimationName::human_to_dragon);
}

void LastBoss::fHumanToDragonUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::DragonIdle);
        mCurrentMode = Mode::Dragon;
        // 体力の値をドラゴンモードの初期値にしておく
        mCurrentHitPoint = mMaxHp * mkPercentToDragon;
    }
}

void LastBoss::fDragonIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_idle, true);
}

void LastBoss::fDragonIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

void LastBoss::fDragonDieStartInit()
{
    // 位置をリセット
    mPosition = { 0.0f,0.0f,50.0f };
    mpModel->play_animation(mAnimPara, AnimationName::dragon_die);
    // TODO カメラをボスに注目させる

}

void LastBoss::fDragonDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::DragonDieEnd);
    }
}

void LastBoss::fDragonDieMiddleInit()
{
    mDissolve = 0.0f;
}

void LastBoss::fDragonDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve += elapsedTime_;
    if(mDissolve>=1.0f)
    {
        mIsAlive = false;
    }
}


void LastBoss::fRender(GraphicsPipeline& graphics)
{
    BaseEnemy::fRender(graphics);
    // ビーム類を描画
    mLaserPointer.fRender(graphics);
    mBeam.fRender(graphics);
    //--------------------<タレット描画>--------------------//
    // タレットの親の位置を取得する
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    const DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(mScale,
    mOrientation, mPosition);

    mpModel->fech_by_bone( world,mTurretBoneLeft,position,up);
    mpTurretLeft->fRender(graphics, world, position);
    mpModel->fech_by_bone(world, mTurretBoneRight, position, up);
    mpTurretLeft->fRender(graphics, world, position);

}
