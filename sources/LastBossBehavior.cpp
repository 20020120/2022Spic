#include"LastBoss.h"
#include"Operators.h"
#include"post_effect.h"
#include"BulletManager.h"
#include "CannonballBullet.h"
#include "EnemyManager.h"
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

void LastBoss::fChangeShipToHumanUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
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
    mpModel->play_animation(mAnimPara, AnimationName::human_idle);
}

void LastBoss::fHumanIdleUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
   // 条件に応じて攻撃手段を分岐させる

    if (mpModel->end_of_animation(mAnimPara)) return;

    //--------------------<アニメーションが終了したら>--------------------//

     // 体力が９割を下回ったら以下きめの必殺技
    if (!mFirstSp && fComputePercentHp() < 0.9f)
    {
        fChangeState(DivideState::HumanSpAway);
        mFirstSp = true;
        return;
    }
    // 体力が５割を下回ったら必殺技
    if (!mSecondSp && fComputePercentHp() <= 0.5f)
    {
        fChangeState(DivideState::HumanSpAway);
        mSecondSp = true;
        return;
    }

    // ステージの限界値を反映
    if(fLimitStageHuman(elapsedTime_))
    {
        return;
    }

    // プレイヤーとの距離が近かったら範囲爆発攻撃を多くする
    const std::uniform_int_distribution<int> RandTargetAdd(0, 9);
    const int randNumber = RandTargetAdd(mt);
    // 遠い
    if (Math::Length(mPlayerPosition - mPosition) > mkDistanceToPlayer*10.0f)
    {
        // さらに距離が遠かったら
        if (randNumber <= 3)
        {
            fChangeState(DivideState::HumanRush);
            return;
        }
    }

    if(Math::Length(mPlayerPosition-mPosition)>mkDistanceToPlayer)
    {
        if(randNumber>=8)
        {
            fChangeState(DivideState::HumanBlowAttack);
            return;
        }
        else  if(randNumber>=5)
        {
            fChangeState(DivideState::HumanAllShot);
            return;
        }
        else if(randNumber>=4)
        {
            fChangeState(DivideState::HumanRush);
            return;
        }
        else
        {
            fChangeState(DivideState::HumanIdle);
            return;
        }
    }
    else
    {
        if (randNumber > 6)
        {
            fChangeState(DivideState::HumanBlowAttack);
            return;
        }
        else if(randNumber>4)
        {
            fChangeState(DivideState::HumanAllShot);
            return;
        }
        else
        {
            fChangeState(DivideState::HumanIdle);
            return;
        }
    }
}

void LastBoss::fHumanMoveInit()
{
 
}

void LastBoss::fHumanMoveUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
   
}

void LastBoss::fHumanAllShotInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_bullet);
    mTimer = 0.0f;
}

void LastBoss::fHumanAllShotUpdate(float elapsedTime_,
    GraphicsPipeline& Graphics_)
{

    fTurnToPlayer(elapsedTime_, 10.0f);

    // 着弾基準点を設定
    DirectX::XMFLOAT3 position{ mPlayerPosition };

    //--------------------<銃口の向きに発射>--------------------//
    mTimer += elapsedTime_;
    if (mTimer < mkHumanAllShotEnd&& mTimer > mkHumanAllShotBegin)
    {
        // ボーンが向いている方向を計算
        DirectX::XMFLOAT3 up{};
        DirectX::XMFLOAT3 leftPosition{};
        DirectX::XMFLOAT4X4 leftRotMat{};
        
        mpModel->fech_by_bone(mAnimPara, Math::calc_world_matrix(mScale,
            mOrientation, mPosition), mTurretBoneLeft, leftPosition,
            up, leftRotMat);
        const DirectX::XMFLOAT3 leftFront  = 
              { leftRotMat._31,leftRotMat._32, leftRotMat._33 };

        // ボーンから着弾点までのベクトル

        // ボーンとのなす角を算出
        DirectX::XMFLOAT3 v = leftPosition - position;
        v.y = 0.0f;
        v = Math::Normalize(v);
        const DirectX::XMFLOAT3 front = 
            Math::Normalize(mPlayerPosition - mPosition);
        float dot = Math::Dot(v, front);

        // ターゲットの位置を基準とした乱数で着弾点を決める
        const std::uniform_real_distribution<float> RandTargetAdd(-70.0f, -30.0f);
        const std::uniform_int_distribution<int> switcher(0, 1);

        DirectX::XMFLOAT3 bulletPosition{};

        bulletPosition.x = RandTargetAdd(mt) * switcher(mt) == 0 ? 1.0f : -1.0f;
        bulletPosition.z = RandTargetAdd(mt) * switcher(mt) == 0 ? 1.0f : -1.0f;
        bulletPosition += position;

        const DirectX::XMFLOAT3 bulletVec{
          Math::Normalize(bulletPosition - leftPosition) };

        const float bulletSpeed = Math::Length(mPlayerPosition - mPosition);


        // 一定間隔で発射
        if (mShotTimer <= 0.0f)
        {
            mfAddBullet(new CannonballBullet(Graphics_,bulletVec ,
               180.0f, leftPosition));
            mShotTimer = mkHumanAllShotDelay;
        }
        else
        {
            mShotTimer -= elapsedTime_;
        }
    }

    if(mpModel->end_of_animation(mAnimPara))
    {
        
        fChangeState(DivideState::HumanIdle);
    }
}


void LastBoss::fHumanBlowAttackInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_shockwave);
    mIsAttack = true;
    mAttackCapsule.mRadius = 0.0f;
}

void LastBoss::fHumanBlowAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    const DirectX::XMFLOAT3 capsuleLength{ 0.0f,20.0f,0.0f };
    mAttackCapsule.mRadius += elapsedTime_ * 20.0f;
    mAttackCapsule.mTop = mPosition + capsuleLength;
    mAttackCapsule.mBottom = mPosition - capsuleLength;

    if(mpModel->end_of_animation(mAnimPara))
    {
        const std::uniform_int_distribution<int> RandTargetAdd(0, 9);
        if (const auto num = RandTargetAdd(mt); num < 2)
        {
            fChangeState(DivideState::HumanMove);
            mIsAttack = false;
            mAttackCapsule.mRadius = 0.0f;
        }
        else
        {
            fChangeState(DivideState::HumanIdle);
            mIsAttack = false;
            mAttackCapsule.mRadius = 0.0f;
        }
    }
}

void LastBoss::fMoveAwayInit()
{
    mMoveBegin = mPosition;
    const auto normalV = Math::Normalize(mPosition - mPlayerPosition);
    mMoveEnd = mPosition + normalV * 50.0f;
    mMoveThreshold = 0.0f;
}

void LastBoss::fMoveAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mMoveThreshold += elapsedTime_ * 2.0f;
    mPosition = Math::lerp(mMoveBegin, mMoveEnd, mMoveThreshold);
    if(mMoveThreshold>=1.0f)
    {
        fChangeState(DivideState::HumanIdle);
    }
}

void LastBoss::fHumanRushInit()
{
    // 移動目標を設定
    mMoveEnd = mPlayerPosition;
    mMoveBegin = mPosition;
    mMoveThreshold = 0.0f;
    mpModel->play_animation(mAnimPara, AnimationName::human_move);
}

void LastBoss::fHumanRushUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    const float limitLength = Math::Length(mMoveBegin - mMoveEnd);
    const float currentLength = Math::Length(mPosition - mMoveBegin);

    // 終了条件
    if (currentLength > limitLength * 1.2f)
    {
        fChangeState(DivideState::HumanIdle);
        return;
    }

    // プレイヤーの方向に移動するベクトルを作成する

    const DirectX::XMFLOAT3 iniVec{
        Math::Normalize(mMoveEnd - mMoveBegin) };
    const DirectX::XMFLOAT3 currentVec{
        Math::Normalize(mPlayerPosition - mMoveBegin) };

    DirectX::XMFLOAT3 endPoint{};
    {
        const float d = Math::Dot(currentVec, mMoveEnd - mMoveBegin);
        const DirectX::XMFLOAT3 end = mMoveBegin + (currentVec * d);
        endPoint=Math::lerp(mMoveBegin, end, mMoveThreshold);
    }


    mMoveThreshold += elapsedTime_ * 0.5f;


    const DirectX::XMFLOAT3 prePosition = mPosition;
    // スプライン曲線でベクトルを取得
    mPosition=Math::fBezierCurve(mMoveBegin, mMoveEnd, endPoint,mMoveThreshold);

    const DirectX::XMFLOAT3 subVec = Math::Normalize(mPosition - prePosition);

    fTurnToTarget(elapsedTime_, 10.0f, mPosition + subVec);

    if(mMoveThreshold>=1.0f)
    {
        fChangeState(DivideState::HumanBlowAttack);
    }
}

void LastBoss::fHumanSpAttackAwayInit()
{
   // 飛びのく地点をセット
    mAwayBegin = mPosition;
    mAwayLerp = 0.0f;

    mBeginOrientation = mOrientation;

    mpModel->play_animation(mAnimPara, AnimationName::human_to_ship_quick);
}

void LastBoss::fHumanSpAttackAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // 正面を向くように回転
    if(mpModel->end_of_animation(mAnimPara))
    {
        mAwayLerp += elapsedTime_ * 2.0f;
        mAwayLerp = (std::min)(1.0f, mAwayLerp);

        // 移動
        mPosition = Math::lerp(mAwayBegin, mAwayEnd, mAwayLerp);
        mOrientation = Math::lerp(mBeginOrientation, mEndOrientation, mAwayLerp);
    }

    if(mAwayLerp>=1.0f)
    {
        fChangeState(DivideState::HumanSpWait);
        fSpawnChildUnit(Graphics_, 1);
    }
}

void LastBoss::fHumanSpAttackWaitInit()
{
    mTimer = 0.0f;
}

void LastBoss::fHumanSpAttackWaitUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 20.0f);
    mTimer += elapsedTime_;
    if(mTimer>=mkHumanSpWaitTime)
    {
        fChangeState(DivideState::HumanSpOver);
    }

    if(mpEnemyManager->fGetEnemyCounts()<=1)
    {
        fChangeState(DivideState::HumanSpDamage);
    }
}

void LastBoss::fHumanSpAttackCancelInit()
{
    
}

void LastBoss::fHumanSpAttackCancelUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

void LastBoss::fHumanSpAttackTimeOverInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_to_human_quick);
}

void LastBoss::fHumanSpAttackTimeOverUpdate(float elapsedTime_,
    GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::HumanSpCharge);
    }
}

void LastBoss::fHumanSpAttackChargeInit()
{
    mAnimationSpeed = 0.7f;
    mpModel->play_animation(mAnimPara, AnimationName::human_beam_charge, true);
    mTimer = mkSpChargeTime;
}

void LastBoss::fHumanSpAttackChargeUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    mTimer -= elapsedTime_;

    if(mTimer>=2.0f)
    {
        fTurnToPlayer(elapsedTime_, 5.0f);
        mRightBeam.fSetPosition(mpTurretRight->fGetPosition(), mPlayerPosition);
        mLeftBeam.fSetPosition(mpTurretLeft->fGetPosition(), mPlayerPosition);
    }
   


    if (mTimer <= 0.0f)
    {
        fChangeState(DivideState::HumanSpShoot);
        mAnimationSpeed = 1.0f;
    }
}

void LastBoss::fHumanSpBeamShootInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_beam_shoot);
    mTimer = 0.0f;

    mAwayBegin = mPosition;
    mAwayLerp = 0.0f;
}

void LastBoss::fHumanSpBeamShootUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    mTimer += elapsedTime_;
    if(mTimer>mkHumanSpBeamTime)
    {
        mPosition = Math::lerp(mAwayBegin, { 0.0f,0.0f,0.0f }, mAwayLerp);
        mAwayLerp += elapsedTime_ * 2.0f;
        fResetLaser();
    }
    else
    {
        mLeftBeamThreshold += elapsedTime_ * 10.0f;
        mLeftBeamThreshold = (std::min)(2.0f, mLeftBeamThreshold);
        mRightBeamThreshold += elapsedTime_ * 10.0f;
        mRightBeamThreshold = (std::min)(2.0f, mRightBeamThreshold);

        mLeftBeam.fSetLengthThreshold(mLeftBeamThreshold);
        mRightBeam.fSetLengthThreshold(mRightBeamThreshold);
    }
    if (mAwayLerp > 1.0f)
    {
        fChangeState(DivideState::HumanIdle);
    }
}

void LastBoss::fHumanSpDamageInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_damage);
    mAwayBegin = mPosition;
    mAwayLerp = 0.0f;
}

void LastBoss::fHumanSpDamageUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        mPosition = Math::lerp(mAwayBegin, { 0.0f,0.0f,0.0f }, mAwayLerp);
        mAwayLerp += elapsedTime_ * 2.0f;
        fResetLaser();
    }
    if (mAwayLerp > 1.0f)
    {
        // ダメージを与える
        mCurrentHitPoint -= (mMaxHp * 0.3f);
        fChangeState(DivideState::HumanIdle);
    }
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
    
    PostEffect::boss_awakening_effect({ 0.5f,0.5f }, mRgbColorPower);
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

    //graphics.set_pipeline_preset(SHADER_TYPES::PBR);
    
    //--------------------<タレット描画>--------------------//
    // タレットの親の位置を取得する
    DirectX::XMFLOAT3 rightPosition{};
    DirectX::XMFLOAT3 leftPosition{};
    DirectX::XMFLOAT3 up{};
    const DirectX::XMFLOAT4X4 world =Math::calc_world_matrix(
        mScale,mOrientation, mPosition);

    mpModel->fech_by_bone(mAnimPara, world, mTurretBoneLeft, leftPosition,
        up);
    mpModel->fech_by_bone(mAnimPara, world, mTurretBoneRight, rightPosition,
        up);

    mpTurretLeft->fRender(graphics, world, leftPosition);
    mpTurretRight->fRender(graphics, world, rightPosition);

    //--------------------<セカンドガン描画>--------------------//
    DirectX::XMFLOAT4X4 rightMat{};
    DirectX::XMFLOAT4X4 leftMat{};
    mpModel->fech_by_bone(mAnimPara, world, mSecondGunBoneLeft,leftMat);
    mpModel->fech_by_bone(mAnimPara, world, mSecondGunBoneRight, rightMat);

    mpSecondGunLeft->fRender(graphics, leftMat, leftPosition);
    mpSecondGunRight->fRender(graphics, rightMat, rightPosition);


    // ビーム類を描画
    mLaserPointer.fRender(graphics);
    mBeam.fRender(graphics);
    mRightBeam.fRender(graphics);
    mLeftBeam. fRender(graphics);
}
