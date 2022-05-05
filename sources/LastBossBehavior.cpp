#include"LastBoss.h"
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
}

void LastBoss::fShipBeamChargeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

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
}

void LastBoss::fShipBeamShootUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
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
    fChangeState(DivideState::ShipToHuman);
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
    }
}

void LastBoss::fHumanInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::human_idle, true);
}

void LastBoss::fHumanUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
   
}
