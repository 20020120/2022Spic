#include"SpearEnemy.h"
#include"Operators.h"
SpearEnemy::SpearEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_spear.fbx")
{
   // 姿勢を初期化
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
//--------------------<待機>--------------------//
void SpearEnemy::fIdleInit()
{
    // アニメーションを再生
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
//--------------------<移動>--------------------//
void SpearEnemy::fMoveInit()
{
    mpSkinnedMesh->play_animation(AnimationName::walk, true);
}
void SpearEnemy::fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<プレイヤーのいる向きに移動>--------------------//
    auto moveVec = mPlayerPosition - mPosition;
    moveVec = Math::Normalize(moveVec);
    velocity = moveVec * mMoveSpeed;
    mPosition += velocity * elapsedTime_;

    //--------------------<プレイヤーの方向に回転>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    // 一定距離になったら突進モーションに入る
    if(mLengthFromPlayer>=mAttackLength)
    {
        fChangeState(DivedState::ThrustBegin);
    }
}
//--------------------<突進準備>--------------------//
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
//--------------------<突進中>--------------------//
void SpearEnemy::fThrustMiddleInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fThrustMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}
//--------------------<突進終わり>--------------------//
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
