#include"LastBoss.h"
#include"Operators.h"
#include"post_effect.h"
#include"BulletManager.h"
#include "CannonballBullet.h"
#include "DragonBreath.h"
#include "EnemyManager.h"
//****************************************************************
// 
// ��̓��[�h 
// 
//****************************************************************
void LastBoss::fShipStartInit()
{
    mDissolve = 1.0f;
    mPosition = { 0.0f,20.0f,0.0f };
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
    mPosition = {};
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_charge_start);

}

void LastBoss::fShipBeamStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // �v���C���[�̕����ɉ�]
    fTurnToPlayer(elapsedTime_, mkRotSpeed);

    // �A�j���[�V�����I���Ɠ����ɑJ��
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::ShipBeamCharge);
    }
}

void LastBoss::fShipBeamChargeInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_charge_idle, true);
    mTimer = mkShipBeamChargeSec;

    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    mpModel->fech_by_bone(mAnimPara, world, mShipFace, mShipFacePosition, up);

    mShipPointer.fSetRadius(0.03f);
    mShipPointer.fSetColor({ 1.0f,0.0f,0.0f,1.0f });
    mShipPointer.fSetAlpha(1.0f);

    // �|�C���^�[�̒�����������
    mPointerLength = 0.0f;
}

void LastBoss::fShipBeamChargeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    // ���[�U�[�|�C���^�[��L�΂�
    mPointerLength += elapsedTime_ * 100.0f;


    mShipPointer.fSetLengthThreshold(1.0f);
    const DirectX::XMFLOAT3 front{ Math::GetFront(mOrientation) };
    mShipPointerEnd = mShipFacePosition + (front * mPointerLength );

    // �|�C���^�[�Ɍv�Z���ꂽ�l��ݒ�
    mShipPointer.fSetPosition(mShipFacePosition, mShipPointerEnd);

    // TODO : �����ŏ����߂̃J�����V�F�C�N
    mTimer -= elapsedTime_;
    if(mTimer<=0.0f)
    {
        // ����
        fChangeState(DivideState::ShipBeamShoot);
    }
}

void LastBoss::fShipBeamShootInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_shot_start);
    mTimer = mkShipBeamShootSec;

    //--------------------<���[�U�[�|�C���^�[������>--------------------//
    mShipPointer.fSetAlpha(0.0f);

    //--------------------<���[�U�[�r�[���𔭎˂���>--------------------//
    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    // �{�[���̈ʒu���擾
    mpModel->fech_by_bone(mAnimPara, world, mShipFace, mShipFacePosition, up);

    // �r�[���̒�����������
    mBeamLength = 0.0f;
    // �r�[���̍U����L��
    mIsAttack = true;
}

void LastBoss::fShipBeamShootUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    // �{�[���̈ʒu���擾
    mpModel->fech_by_bone(mAnimPara, world, mShipFace, mShipFacePosition, up);

    // �r�[���̌����ݒ肷��
    const DirectX::XMFLOAT3 front = Math::GetFront(mOrientation);
    const DirectX::XMFLOAT3 beamEnd{ mShipFacePosition + (front * mBeamLength) };

    // �r�[���̒������X�V����
    mBeamLength += elapsedTime_ * 100.0f;


    // �U��capsule��ݒ�
    mAttackCapsule.mBottom = mShipFacePosition;
    mAttackCapsule.mTop = beamEnd;
    mAttackCapsule.mRadius = 10.0f;


    fTurnToPlayer(elapsedTime_,2.0f);
    // TODO : �����Ō������J�����V�F�C�N
    mTimer -= elapsedTime_;
    if(mTimer<=0.0f)
    {
        fChangeState(DivideState::ShipBeamEnd);
        mIsAttack = false;
        mBeamLength = 0.0f;
    }
}

void LastBoss::fShipBeamEndInit()
{
    
}

void LastBoss::fShipBeamEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    fChangeState(DivideState::ShipToHuman);
}

void LastBoss::fChangeShipToHumanInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ship_to_human);
    mCurrentMode = Mode::ShipToHuman;
    mPosition = { 0.0f,20.0f,0.0f };
}

void LastBoss::fChangeShipToHumanUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::HumanIdle);

        // ���݂̃��[�h��l�^�ɕύX����
        mCurrentMode = Mode::Human;
        mPosition = {};
    }
}

void LastBoss::fHumanIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_idle);
    mAnimationSpeed = 1.5f;
}

void LastBoss::fHumanIdleUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 5.0f);

   // �����ɉ����čU����i�𕪊򂳂���

    if (!mpModel->end_of_animation(mAnimPara)) return;

    //--------------------<�A�j���[�V�������I��������>--------------------//

     // �̗͂��X�������������ȉ����߂̕K�E�Z
    if (!mFirstSp && fComputePercentHp() < 0.9f)
    {
        fChangeState(DivideState::HumanSpAway);
        mFirstSp = true;
        mAnimationSpeed = 1.0f;
        return;
    }
    // �̗͂��T�������������K�E�Z
    if (!mSecondSp && fComputePercentHp() <= 0.5f)
    {
        fChangeState(DivideState::HumanSpAway);
        mSecondSp = true;
        mAnimationSpeed = 1.0f;
        return;
    }

    // �X�e�[�W�̌��E�l�𔽉f
    if(fLimitStageHuman(elapsedTime_))
    {
        return;
    }

    // �v���C���[�Ƃ̋������߂�������͈͔����U���𑽂�����
    const std::uniform_int_distribution<int> RandTargetAdd(0, 9);
    const int randNumber = RandTargetAdd(mt);
    // ����
    if (Math::Length(mPlayerPosition - mPosition) > mkDistanceToPlayer*10.0f)
    {
        // ����ɋ���������������
        if (randNumber <= 3)
        {
            fChangeState(DivideState::HumanRush);
            mAnimationSpeed = 1.0f;
            return;
        }
    }

    if(Math::Length(mPlayerPosition-mPosition)>mkDistanceToPlayer)
    {
        if(randNumber>=8)
        {
            fChangeState(DivideState::HumanBlowAttack);
            mAnimationSpeed = 1.0f;
            return;
        }
        else  if(randNumber>=6)
        {
            fChangeState(DivideState::HumanAllShot);
            mAnimationSpeed = 1.0f;
            return;
        }
        else if(randNumber>=3)
        {
            fChangeState(DivideState::HumanRush);
            mAnimationSpeed = 1.0f;
            return;
        }
        else
        {
            fChangeState(DivideState::HumanIdle);
            mAnimationSpeed = 1.0f;
            return;
        }
    }
    else
    {
        if (randNumber > 6)
        {
            fChangeState(DivideState::HumanBlowAttack);
            mAnimationSpeed = 1.0f;
            return;
        }
        else if(randNumber>4)
        {
            fChangeState(DivideState::HumanAllShot);
            mAnimationSpeed = 1.0f;
            return;
        }
        else
        {
            fChangeState(DivideState::HumanIdle);
            mAnimationSpeed = 1.0f;
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

    // ���e��_��ݒ�
    DirectX::XMFLOAT3 position{ mPlayerPosition };

    //--------------------<�e���̌����ɔ���>--------------------//
    mTimer += elapsedTime_;
    if (mTimer < mkHumanAllShotEnd&& mTimer > mkHumanAllShotBegin)
    {
        // �{�[���������Ă���������v�Z
        DirectX::XMFLOAT3 up{};
        DirectX::XMFLOAT3 leftPosition{};
        DirectX::XMFLOAT4X4 leftRotMat{};
        
        mpModel->fech_by_bone(mAnimPara, Math::calc_world_matrix(mScale,
            mOrientation, mPosition), mTurretBoneLeft, leftPosition,
            up, leftRotMat);
        const DirectX::XMFLOAT3 leftFront  = 
              { leftRotMat._31,leftRotMat._32, leftRotMat._33 };

        // �{�[�����璅�e�_�܂ł̃x�N�g��

        // �{�[���Ƃ̂Ȃ��p���Z�o
        DirectX::XMFLOAT3 v = leftPosition - position;
        v.y = 0.0f;
        v = Math::Normalize(v);
        const DirectX::XMFLOAT3 front = 
            Math::Normalize(mPlayerPosition - mPosition);
        float dot = Math::Dot(v, front);

        // �^�[�Q�b�g�̈ʒu����Ƃ��������Œ��e�_�����߂�
        const std::uniform_real_distribution<float> RandTargetAdd(-70.0f, -30.0f);
        const std::uniform_int_distribution<int> switcher(0, 1);

        DirectX::XMFLOAT3 bulletPosition{};

        bulletPosition.x = RandTargetAdd(mt) * switcher(mt) == 0 ? 1.0f : -1.0f;
        bulletPosition.z = RandTargetAdd(mt) * switcher(mt) == 0 ? 1.0f : -1.0f;
        bulletPosition += position;

        const DirectX::XMFLOAT3 bulletVec{
          Math::Normalize(bulletPosition - leftPosition) };

        const float bulletSpeed = Math::Length(mPlayerPosition - mPosition);


        // ���Ԋu�Ŕ���
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
    mpAllAttackEffect->play(effect_manager->get_effekseer_manager(),
        mPosition, 5.0f);
    mIsAttack = true;
    mAttackCapsule.mRadius = 0.0f;
    mTimer = 0.0f;
}

void LastBoss::fHumanBlowAttackUpdate(float elapsedTime_,
    GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 5.0f);

    mTimer += elapsedTime_;

    if (mTimer > mkTimerBlow)
    {
        constexpr DirectX::XMFLOAT3 capsuleLength{ 0.0f,20.0f,0.0f };
        mAttackCapsule.mRadius += elapsedTime_ * 50.0f;
        mAttackCapsule.mTop = mPosition + capsuleLength;
        mAttackCapsule.mBottom = mPosition - capsuleLength;
    }


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
    // �ړ��ڕW��ݒ�
    mMoveEnd = mPlayerPosition;
    mMoveBegin = mPosition;
    mMoveThreshold = 0.0f;
    mpModel->play_animation(mAnimPara, AnimationName::human_rush_idle);
}

void LastBoss::fHumanRushUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    const float limitLength = Math::Length(mMoveBegin - mMoveEnd);
    const float currentLength = Math::Length(mPosition - mMoveBegin);

    // �I������
    if (currentLength > limitLength * 1.2f)
    {
        fChangeState(DivideState::HumanIdle);
        return;
    }

    // �v���C���[�̕����Ɉړ�����x�N�g�����쐬����

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
    // �X�v���C���Ȑ��Ńx�N�g�����擾
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
   // ��т̂��n�_���Z�b�g
    mAwayBegin = mPosition;
    mAwayLerp = 0.0f;

    mBeginOrientation = mOrientation;

    mpModel->play_animation(mAnimPara, AnimationName::human_to_ship_quick);
}

void LastBoss::fHumanSpAttackAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // ���ʂ������悤�ɉ�]
    if(mpModel->end_of_animation(mAnimPara))
    {
        mAwayLerp += elapsedTime_ * 2.0f;
        mAwayLerp = (std::min)(1.0f, mAwayLerp);

        // �ړ�
        mPosition = Math::lerp(mAwayBegin, mAwayEnd, mAwayLerp);
        mOrientation = Math::lerp(mBeginOrientation, mEndOrientation, mAwayLerp);
    }

    if(mAwayLerp>=1.0f)
    {
        fChangeState(DivideState::HumanSpWait);
        fSpawnChildUnit(Graphics_, 6);
    }
}

void LastBoss::fHumanSpAttackSummonInit()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanSpAttackSummonUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}


void LastBoss::fHumanSpAttackWaitInit()
{
    mTimer = 0.0f;
}

void LastBoss::fHumanSpAttackWaitUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 5.0f);
    mTimer += elapsedTime_;
    if(mTimer>=mkHumanSpWaitTime)
    {
        fChangeState(DivideState::HumanSpOver);
    }

    if(mpEnemyManager->fGetEnemyCounts()<=4)
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

    // laser��������
    mRightPointer.fSetRadius(0.3f);  // ���a
    mLeftPointer.fSetRadius(0.3f);

    mRightPointer.fSetColor({1.0f,0.0f,0.0f,1.0f}); // �F
    mLeftPointer. fSetColor({1.0f,0.0f,0.0f,1.0f});

    mRightPointer.fSetLengthThreshold(1.0f); // 臒l
    mLeftPointer. fSetLengthThreshold(1.0f);

    mRightPointer.fSetAlpha(1.0f); // �s�����x
    mLeftPointer. fSetAlpha(1.0f);

    mHumanBeamTarget = mPosition;

}

void LastBoss::fHumanSpAttackChargeUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    mTimer -= elapsedTime_;


    // �|�C���^�[���v���C���[�̈ʒu�ɉ�]������
    mHumanBeamTarget = Math::lerp(mHumanBeamTarget, mPlayerPosition, 
        30.0f * elapsedTime_);

    mRightPointer.fSetPosition(mpTurretRight->fGetPosition(), mHumanBeamTarget);
    mLeftPointer.fSetPosition(mpTurretLeft->fGetPosition(), mHumanBeamTarget);
    fTurnToPlayer(elapsedTime_, 5.0f);
   


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

    mRightPointer.fSetAlpha(0.0f);
    mLeftPointer.fSetAlpha(0.0f);

    mAwayBegin = mPosition;
    mAwayLerp = 0.0f;

    mIsAttack = true;
}

void LastBoss::fHumanSpBeamShootUpdate(float elapsedTime_, 
    GraphicsPipeline& Graphics_)
{
    mHumanBeamTarget = Math::lerp(mHumanBeamTarget, mPlayerPosition,
        1.0f * elapsedTime_);

    mAttackCapsule.mBottom = mPosition;
    mAttackCapsule.mTop = mHumanBeamTarget;
    mAttackCapsule.mRadius = 5.0f;

    mTimer += elapsedTime_;
    if(mTimer>mkHumanSpBeamTime)
    {
        mIsAttack = false;
        mPosition = Math::lerp(mAwayBegin, { 0.0f,0.0f,0.0f }, mAwayLerp);
        mAwayLerp += elapsedTime_ * 2.0f;
    }
    else
    {

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

    }
    if (mAwayLerp > 1.0f)
    {
        // �_���[�W��^����
        mCurrentHitPoint -= (mMaxHp * 0.1f);
        fChangeState(DivideState::HumanIdle);
    }
}


void LastBoss::fHumanDieStartInit()
{
    // �ʒu�����Z�b�g
    mPosition = { 0.0f,0.0f,20.0f };
    mpModel->play_animation(mAnimPara, AnimationName::human_die);
    // TODO : �G�t�F�N�g�̗ނ����ׂă��Z�b�g����

    // TODO : �J�������{�X�̕��Ɍ�����

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
    // RGB���炵�ŐS���̂ǂ��ǂ������o����
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
            //  ���������ҋ@����
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
        // �̗͂̒l���h���S�����[�h�̏����l�ɂ��Ă���
        mCurrentHitPoint = mMaxHp * mkPercentToDragon;
    }
}

void LastBoss::fDragonIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_idle);
    mTimer = 0.0f;
    mCurrentMode = Mode::Dragon;
    mDragonBreathCount = 0;
    mAnimationSpeed = 5.0f;
}

void LastBoss::fDragonIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if (mpModel->end_of_animation(mAnimPara) == false) return;;

    const std::uniform_int_distribution<int> RandTargetAdd(0, 9);
    const int randNumber = RandTargetAdd(mt);

    if(randNumber>8)
    {
        fChangeState(DivideState::DragonIdle);
    }
    else if (randNumber > 6)
    {
        fChangeState(DivideState::DragonRushHide);
    }
    else if(randNumber>4)
    {
        fChangeState(DivideState::DragonBeamStart);
    }
    else
    {
        fChangeState(DivideState::DragonHideStart);
    }
    mAnimationSpeed = 1.0f;
}

void LastBoss::fDragonFastBreathStartInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_hide);
    mTimer = mkDragonHideTime;
    mDissolve = 0.0f;
    mpSecondGunRight->fSetDissolve(mDissolve);
    mpSecondGunLeft->fSetDissolve (mDissolve);
    mpTurretLeft->fSetDissolve(mDissolve);
    mpTurretRight->fSetDissolve(mDissolve);
}

void LastBoss::fDragonFastBreathStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
    mTimer -= elapsedTime_;
    mDissolve += elapsedTime_ * 1.2f;
    mpSecondGunRight->fSetDissolve(mDissolve);
    mpSecondGunLeft->fSetDissolve(mDissolve);
    if(mTimer<0.0f)
    {
        // �f�B�]���u�I��
        fChangeState(DivideState::DragonAppear);
        mDissolve = 1.0f;
        mpSecondGunRight->fSetDissolve(mDissolve);
        mpSecondGunLeft->fSetDissolve(mDissolve);
        mpTurretLeft->fSetDissolve(mDissolve);
        mpTurretRight->fSetDissolve(mDissolve);
    }
}

void LastBoss::fDragonBreathAppearInit()
{
    // �ʒu���ړ�������

    int areaSeed{};
    // �X�e�[�W���l�������č��ǂ��ɂ��邩����i���}�̓G���A�ԍ��j

    /////////////////////////
    //         |           //
    //     0   |     1     //
    //         |           //
    //---------|-----------//
    //         |           //
    //     2   |     3     //
    //         |           //
    /////////////////////////

    if(mPosition.x>0.0f)
    {
        areaSeed++;
    }
    if(mPosition.z<0.0f)
    {
        areaSeed += 2;
    }

    std::vector<int> vec{};
    vec.reserve(3);
    for(int i=0;i<4;++i)
    {
        if (i == areaSeed) continue;;
        vec.emplace_back(i);
    }
    const std::uniform_int_distribution<int> RandTargetAdd(0, 2);

    // �ڕW�n�_�̃C���f�b�N�X���擾
    const int areaIndex = vec.at(RandTargetAdd(mt));

    float rad = DirectX::XMConvertToRadians(45.0f);
    switch (areaIndex)
    {
    case 0:
        mPosition = { -cosf(rad),0.0f,sinf(rad) };
        break;
    case 1 :
        mPosition = { cosf(rad),0.0f,sinf(rad) };
        break;
    case 2:
        mPosition = { -cosf(rad),0.0f,-sinf(rad) };
        break;
    case 3:
        mPosition = { cosf(rad),0.0f,-sinf(rad) };
        break;
    default: ;
    }

    mPosition *= (mkLimitStage * 0.5f * 0.7f);

    mDissolve = 1.0f;

    }

void LastBoss::fDragonBreathAppearUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 10.0f);
    mDissolve -= elapsedTime_ * 1.3f;
    if(mDissolve<0.0f)
    {
        fChangeState(DivideState::DragonBreathCharge);
    }
}

void LastBoss::fDragonBreathChargeInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_breath_ready);
}

void LastBoss::fDragonBreathChargeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 10.0f);

    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::DragonBreathShot);
    }
}

void LastBoss::fDragonBreathShotInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_breath_start);
    mDragonBreathCount++;
    mTimer = 0.0f;
    mIsShotBreath = false;
}

void LastBoss::fDragonBreathShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mIsShotBreath==false)
    {
        auto p = mPosition;
        p.y += 30.0f;
        mfAddBullet(new DragonBreath(Graphics_, p, 60.0f, mPlayerPosition));
        mIsShotBreath = true;
    }

    if(mpModel->end_of_animation(mAnimPara))
    {
        // �m���ł�����x���i�ő�O��j
        const std::uniform_int_distribution<int> RandTargetAdd(0, 9);
        const int randNumber = RandTargetAdd(mt);
        if (std::pow(mDragonBreathCount, 2) < randNumber)
        {
            fChangeState(DivideState::DragonHideStart);
        }
        else
        {
            fChangeState(DivideState::DragonIdle);
        }
        
    }
}

void LastBoss::fDragonRushHideInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_hide);
    mDissolve = 0.0f;
}

void LastBoss::fDragonRushHideUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve += elapsedTime_;
    mpSecondGunLeft->fSetDissolve(mDissolve);
    mpSecondGunRight->fSetDissolve(mDissolve);
    mpTurretLeft->fSetDissolve(mDissolve);
    mpTurretRight->fSetDissolve(mDissolve);

    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::DragonRushWait);
        // ���b�N�I������Ȃ��悤�ɉ����Ɉړ�������
        mPosition = { 0.0f,500.0f,0.0f };
    }

}

void LastBoss::fDragonRushWaitInit()
{
    mTimer = mkDragonRushWaitTime;
}

void LastBoss::fDragonRushWaitUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    mTimer -= elapsedTime_;
    // ��b���Ƃɏo��������
    if(mTimer<=3.5f&&mRushVec[0]->fGetIsStart()==false)
    {
        constexpr float rad = { DirectX::XMConvertToRadians(0.0f) };
        constexpr float length = 200.0f * 0.7f;
        mRushVec[0]->fStartAppear(
            {
                cosf(rad) * length,0.0f,sinf(rad) * length
            });
    }
    if (mTimer <= 2.0f && mRushVec[1]->fGetIsStart() == false)
    {
        constexpr float rad = { DirectX::XMConvertToRadians(120.0f) };
        constexpr float length = 200.0f * 0.7f;
        mRushVec[1]->fStartAppear(
            {
                cosf(rad)*length,0.0f,sinf(rad)*length
            });
    }
    if (mTimer <= 0.5f && mRushVec[2]->fGetIsStart() == false)
    {
        constexpr float rad = { DirectX::XMConvertToRadians(240.0f) };
        constexpr float length = 200.0f * 0.7f;
        mRushVec[2]->fStartAppear(
            {
                cosf(rad) * length,0.0f,sinf(rad) * length
            });
    }

    if(mTimer<-2.0f)
    {
        fChangeState(DivideState::DragonRushAppear);
    }
}

void LastBoss::fDragonRushAppearInit()
{
    mPosition = { 0.0f,0.0f,0.0f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mpModel->play_animation(mAnimPara, AnimationName::dragon_idle);
    mDissolve = 1.0f;
}

void LastBoss::fDragonRushAppearUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve -= elapsedTime_;
    mpSecondGunLeft->fSetDissolve(mDissolve);
    mpSecondGunRight->fSetDissolve(mDissolve);
    mpTurretLeft->fSetDissolve(mDissolve);
    mpTurretRight->fSetDissolve(mDissolve);
    mPosition = { 0.0f,0.0f,0.0f };
    if(mDissolve<0.0f)
    {
        fChangeState(DivideState::DragonIdle);
    }
}

void LastBoss::fDragonBeamMoveInit()
{
    mDragonMoveThreshold = 0.0f;
    mMoveBegin = mPosition;
    mpModel->play_animation(mAnimPara, AnimationName::dragon_idle, true);
}

void LastBoss::fDragonBeamMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
   // �����܂ňړ�
    mDragonMoveThreshold += elapsedTime_;
    mPosition = Math::lerp(mMoveBegin, { 0.0f,0.0f,0.0f }, mDragonMoveThreshold);

    if(mDragonMoveThreshold >=1.0f)
    {
        fChangeState(DivideState::DragonBeamStart);
    }
}

void LastBoss::fDragonBeamStartInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_beam_ready);
}

void LastBoss::fDragonBeamStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 10.0f);
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::DragonBeamCharge);
    }
}

void LastBoss::fDragonBeamChargeInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_beam_charge);
    mTimer = mkDragonBeamChargeTime;
}

void LastBoss::fDragonBeamChargeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 10.0f);
    mTimer -= elapsedTime_;
    if (mTimer < 0.0f)
    {
        fChangeState(DivideState::DragonBeamShoot);
    }
}
void LastBoss::fDragonBeamShotInit()
{
    // ���݂̊e�c�����߂�

    auto front = Math::Normalize(Math::GetFront(mOrientation));
    DirectX::XMFLOAT3 worldFront = { 0.0f,0.0f,1.0f };
    float dot = Math::Dot(front, worldFront);
    //mBeamStartRadian = acosf(dot);
    //mAddRadian = 0.0f;
}

void LastBoss::fDragonBeamShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    /*mAddRadian += DirectX::XMConvertToRadians(360.0f / 5.0f * elapsedTime_);

    const float radian = mBeamStartRadian + mAddRadian;

    const DirectX::XMFLOAT3 beamFront = { cosf(radian),0.0f,sinf(radian) };
    fTurnToTarget(elapsedTime_, 10.0f, beamFront);

    if (mAddRadian >= DirectX::XMConvertToRadians(360.0f))
    {
        fChangeState(DivideState::DragonIdle);
    }*/
}


void LastBoss::fDragonBeamEndInit()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fDragonBeamEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fDragonDieStartInit()
{
    // �ʒu�����Z�b�g
    mPosition = { 0.0f,0.0f,50.0f };
    mpModel->play_animation(mAnimPara, AnimationName::dragon_die);
    // TODO �J�������{�X�ɒ��ڂ�����
    
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

    //--------------------<�^���b�g�`��>--------------------//
    // �^���b�g�̐e�̈ʒu���擾����
    DirectX::XMFLOAT3 rightPosition{};
    DirectX::XMFLOAT3 leftPosition{};
    DirectX::XMFLOAT3 up{};
    const DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(
        mScale, mOrientation, mPosition);

    mpModel->fech_by_bone(mAnimPara, world, mTurretBoneLeft, leftPosition,
        up);
    mpModel->fech_by_bone(mAnimPara, world, mTurretBoneRight, rightPosition,
        up);

    mpTurretLeft->fRender(graphics, world, leftPosition);
    mpTurretRight->fRender(graphics, world, rightPosition);

    //--------------------<�Z�J���h�K���`��>--------------------//
    DirectX::XMFLOAT4X4 rightMat{};
    DirectX::XMFLOAT4X4 leftMat{};
    mpModel->fech_by_bone(mAnimPara, world, mSecondGunBoneLeft, leftMat);
    mpModel->fech_by_bone(mAnimPara, world, mSecondGunBoneRight, rightMat);

    mpSecondGunLeft->fRender(graphics, leftMat, leftPosition);
    mpSecondGunRight->fRender(graphics, rightMat, rightPosition);

    //--------------------<�r�[����`��>--------------------//
    mShipPointer.fRender(graphics);
    mRightPointer.fRender(graphics);
    mLeftPointer.fRender(graphics);

}
