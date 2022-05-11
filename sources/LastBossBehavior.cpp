#include"LastBoss.h"
#include"Operators.h"
#include"post_effect.h"
#include"BulletManager.h"
#include "CannonballBullet.h"
#include "EnemyManager.h"
//****************************************************************
// 
// ��̓��[�h 
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
    mpModel->play_animation(mAnimPara, AnimationName::ship_beam_charge_idle,true);
    mTimer = mkShipBeamChargeSec;

    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    mpModel->fech_by_bone(mAnimPara, world, mShipFace, mPointerBegin, up);

    //--------------------<���[�U�[�|�C���^�[��������>--------------------//
    mPointerEnd = mPointerBegin;
    // �O���Ƀ|�C���^�[��L�΂�
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

    // ���[�U�[�|�C���^�[��L�΂�
    mPointerThreshold += elapsedTime_ * 5.0f;
    mLaserPointer.fSetLengthThreshold(mPointerThreshold);

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
    mPointerThreshold = 0.0f;
    mLaserPointer.fSetPosition(mPointerBegin, mPointerBegin);
    mLaserPointer.fSetRadius(0.0f);
    mLaserPointer.fSetColor({ 1.0f,0.0f,0.0f ,1.0f });

    //--------------------<���[�U�[�r�[���𔭎˂���>--------------------//
    const auto world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    DirectX::XMFLOAT3 up{};
    // �{�[���̈ʒu���擾
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

    // TODO : �����Ō������J�����V�F�C�N
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

        // ���݂̃��[�h��l�^�ɕύX����
        mCurrentMode = Mode::Human;
    }
}

void LastBoss::fHumanIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_idle, true);
}

void LastBoss::fHumanIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
   // �����ɉ����čU����i�𕪊򂳂���

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
    mpModel->play_animation(mAnimPara, AnimationName::human_bullet);
    mTimer = 0.0f;
}

void LastBoss::fHumanAllShotUpdate(float elapsedTime_,
    GraphicsPipeline& Graphics_)
{

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

        // ���Ԋu�Ŕ���
        if (mShotTimer <= 0.0f)
        {
            mfAddBullet(new CannonballBullet(Graphics_,leftFront ,
                100.0f, leftPosition));
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
}

void LastBoss::fHumanBlowAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::HumanIdle);
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
        // �_���[�W��^����
        mCurrentHitPoint -= (mMaxHp * 0.3f);
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
    mpModel->play_animation(mAnimPara, AnimationName::dragon_idle, true);
}

void LastBoss::fDragonIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
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
    const DirectX::XMFLOAT4X4 world =Math::calc_world_matrix(
        mScale,mOrientation, mPosition);

    mpModel->fech_by_bone(mAnimPara, world, mTurretBoneLeft, leftPosition,
        up);
    mpModel->fech_by_bone(mAnimPara, world, mTurretBoneRight, rightPosition,
        up);

    mpTurretLeft->fRender(graphics, world, leftPosition);
    mpTurretRight->fRender(graphics, world, rightPosition);

    //--------------------<�Z�J���h�K���`��>--------------------//
    DirectX::XMFLOAT4X4 rightMat{};
    DirectX::XMFLOAT4X4 leftMat{};
    mpModel->fech_by_bone(mAnimPara, world, mSecondGunBoneLeft,leftMat);
    mpModel->fech_by_bone(mAnimPara, world, mSecondGunBoneRight, rightMat);

    mpSecondGunLeft->fRender(graphics, leftMat, leftPosition);
    mpSecondGunRight->fRender(graphics, rightMat, rightPosition);


    // �r�[���ނ�`��
    mLaserPointer.fRender(graphics);
    mBeam.fRender(graphics);
    mRightBeam.fRender(graphics);
    mLeftBeam. fRender(graphics);
}
