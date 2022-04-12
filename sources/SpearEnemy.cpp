#include"SpearEnemy.h"
#include"Operators.h"
SpearEnemy::SpearEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_spear.fbx")
{
   // �p����������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.05f,0.05f,0.05f };
    mParam.mHitPoint = 1;

}

void SpearEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    
}

void SpearEnemy::fRegisterFunctions()
{
    
}
//--------------------<�ҋ@>--------------------//
void SpearEnemy::fIdleInit()
{
    // �A�j���[�V�������Đ�
    mpSkinnedMesh->play_animation(AnimationName::idle, true);
    
}
void SpearEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
    if(mWaitTime>=mIdleSec)
    {
        fChangeState(DivedState::Move);
    }
}
//--------------------<�ړ�>--------------------//
void SpearEnemy::fMoveInit()
{
    mpSkinnedMesh->play_animation(AnimationName::walk, true);
}
void SpearEnemy::fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<�v���C���[�̂�������Ɉړ�>--------------------//
    auto moveVec = mPlayerPosition - mPosition;
    moveVec = Math::Normalize(moveVec);
    velocity = moveVec * mMoveSpeed;
    mPosition += velocity * elapsedTime_;

    //--------------------<�v���C���[�̕����ɉ�]>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    // ��苗���ɂȂ�����ːi���[�V�����ɓ���
    if(mLengthFromPlayer>=mAttackLength)
    {
        fChangeState(DivedState::ThrustBegin);
    }
}
//--------------------<�ːi����>--------------------//
void SpearEnemy::fThrustBeginInit()
{
    mpSkinnedMesh->play_animation(AnimationName::attack_idle);
    mWaitTime = 0.0f;
}
void SpearEnemy::fThrustBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
   if(mWaitTime>=mThrustBeginSec)
   {
       fChangeState(DivedState::ThrustMiddle);
   }
}
//--------------------<�ːi��>--------------------//
void SpearEnemy::fThrustMiddleInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fThrustMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}
//--------------------<�ːi�I���>--------------------//
void SpearEnemy::fThrustEndInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fThrustEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDamageInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDamageUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

inline SpearEnemy::~SpearEnemy()
{}

void SpearEnemy::fInitialize()
{
    
}
