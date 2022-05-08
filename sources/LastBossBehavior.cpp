#include"LastBoss.h"
#include"Operators.h"
//****************************************************************
// 
// ��̓��[�h 
// 
//****************************************************************
void LastBoss::fShipStartInit()
{
   
}

void LastBoss::fShipStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fChangeState(DivideState::ShipIdle);
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

void LastBoss::fHumanMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanAllShotInit()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanAllShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanRotAttackInit()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fHumanRotAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
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
}

void LastBoss::fHumanDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mTimer -= elapsedTime_;
    if(mTimer<=0.0f)
    {
        fChangeState(DivideState::HumanToDragon);
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
    }
}

void LastBoss::fDragonIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::dragon_idle, true);
}

void LastBoss::fDragonIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}


void LastBoss::fRender(GraphicsPipeline& graphics)
{
    BaseEnemy::fRender(graphics);
    mLaserPointer.fRender(graphics);
    
    mBeam.fRender(graphics);
}
