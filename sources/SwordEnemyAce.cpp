#include"SwordEnemyAce.h"
#include"Operators.h"
inline SwordEnemy_Ace::SwordEnemy_Ace(GraphicsPipeline& Graphics_, const DirectX::XMFLOAT3& EmitterPoint_,
    const EnemyParamPack& ParamPack_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/SwordEnemyAce.fbx")
{
    SwordEnemy_Ace::fRegisterFunctions();
    // ƒ{[ƒ“‚ð‰Šú‰»
    mScale = { 0.05f,0.05f,0.05f };
}

SwordEnemy_Ace::SwordEnemy_Ace(GraphicsPipeline& Graphics_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/SwordEnemyAce.fbx")
{}

SwordEnemy_Ace::~SwordEnemy_Ace()
{}

void SwordEnemy_Ace::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fBaseUpdate(elapsedTime_,Graphics_);
}

void SwordEnemy_Ace::fUpdateAttackCapsule()
{
    throw std::logic_error("Not implemented");
}

void SwordEnemy_Ace::fDie()
{
    BaseEnemy::fDie();
}

void SwordEnemy_Ace::fRegisterFunctions()
{
    {
        InitFunc ini = [=]()->void
        {
            fStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fIdleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fIdleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::Idle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fMoveInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fMoveUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::Move, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fCounterStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fCounterStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::CounterStart, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fCounterMiddleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fCounterMiddleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::CounterMiddle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fCounterEndInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fCounterEndUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::CounterAttack, tuple));
    }
}

void SwordEnemy_Ace::fStartInit()
{
    mpModel->play_animation(mAnimPara, 0, false, false);
}

void SwordEnemy_Ace::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve -= elapsedTime_;
    if(mDissolve<=0.0f)
    {
        fChangeState(DivideState::Idle);
    }
}

void SwordEnemy_Ace::fIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
    mWaitTimer = 0.0f;
}

void SwordEnemy_Ace::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    if(mWaitTimer>=3.0f)
    {
        fChangeState(DivideState::Move);
    }
}

void SwordEnemy_Ace::fCounterStartInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ace_attack_ready);
}

void SwordEnemy_Ace::fCounterStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
  if(mpModel->end_of_animation())
  {
      fChangeState(DivideState::CounterMiddle);
  }
}

void SwordEnemy_Ace::fCounterMiddleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::ace_attack_idle, true);
    mWaitTimer = 0.0f;
}

void SwordEnemy_Ace::fCounterMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mWaitTimer>=3.0f)
    {
        fChangeState(DivideState::Idle);
    }
    
}

void SwordEnemy_Ace::fCounterEndInit()
{
   
}

void SwordEnemy_Ace::fCounterEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

void SwordEnemy_Ace::fMoveInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::walk);
}

void SwordEnemy_Ace::fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_, 10.0f);
    DirectX::XMFLOAT3 vec = mPlayerPosition - mPosition;
    vec = Math::Normalize(vec);
    mPosition += vec * elapsedTime_ * 30.0f;

    if(fGetLengthFromPlayer()<=10.0f)
    {
        fChangeState(DivideState::CounterStart);
    }
}

