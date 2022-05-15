#include"TutorialEnemy.h"
#include"Operators.h"
TutorialEnemy_NoAttack::TutorialEnemy_NoAttack(GraphicsPipeline& graphics, 
                                               const DirectX::XMFLOAT3& entry_position, const EnemyParamPack& param ): BaseEnemy(graphics, 
                                                                                                                                 "./resources/Models/Enemy/enemy_tutorial.fbx", param, entry_position)
{
    mPosition = entry_position;
    mScale = { 0.06f,0.06f,0.06f };
    fRegisterFunctions();
}

TutorialEnemy_NoAttack::TutorialEnemy_NoAttack(GraphicsPipeline& graphics): BaseEnemy(graphics, "./resources/Models/Enemy/enemy_tutorial.fbx")
{}

void TutorialEnemy_NoAttack::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    elapsedTime_=fBaseUpdate(elapsedTime_, Graphics_);
}

void TutorialEnemy_NoAttack::fUpdateAttackCapsule()
{
    
}

void TutorialEnemy_NoAttack::fRegisterFunctions()
{
    {
        InitFunc Ini = [=]()->void
        {
            fStartInit();
        };
        UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStartUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, Up);
        mFunctionMap.insert(std::make_pair(DivideState::Start, tuple));
    }
    {
        InitFunc Ini = [=]()->void
        {
            fIdleInit();
        };
        UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fIdleUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, Up);
        mFunctionMap.insert(std::make_pair(DivideState::Idle, tuple));
    }
    {
        InitFunc Ini = [=]()->void
        {
            fStunInit();
        };
        UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStunUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, Up);
        mFunctionMap.insert(std::make_pair(DivideState::Stun, tuple));
    }
    {
        InitFunc Ini = [=]()->void
        {
            fMoveInit();
        };
        UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fMoveUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, Up);
        mFunctionMap.insert(std::make_pair(DivideState::Move, tuple));
    }
    fChangeState(DivideState::Start);
}

void TutorialEnemy_NoAttack::fStartInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
    mDissolve = 1.0f;
}

void TutorialEnemy_NoAttack::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve -= elapsedTime_;
    if(mDissolve<0.0f)
    {
        fChangeState(DivideState::Idle);
    }
}

void TutorialEnemy_NoAttack::fIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle);
}

void TutorialEnemy_NoAttack::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivideState::Move);
    }
}

void TutorialEnemy_NoAttack::fMoveInit()
{
   // ˆê’è”ÍˆÍ“à‚ðƒ‰ƒ“ƒ_ƒ€‚Å“®‚­
    const std::uniform_int_distribution<int> RandTargetAdd(0, 5);
    const int randNumber = RandTargetAdd(mt);

   const float radian{ DirectX::XMConvertToRadians(static_cast<float>(randNumber) * (360.0f / 6)) };

    // ‰ñ“]—Ê‚Æ‹——£‚©‚çˆÚ“®æ‚ðŠ„‚èo‚·
    DirectX::XMFLOAT3 randPosition = { cosf(radian),0.0f,sinf(radian) };

    mWonderTarget = { randPosition * 40.0f };
    mWonderBegin = mPosition;
    mTimer = 0.0f;
}

void TutorialEnemy_NoAttack::fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mTimer += elapsedTime_ * 0.5f;
    fTurnToTarget(elapsedTime_, 10.0f, mWonderTarget);
    mPosition = Math::lerp(mWonderBegin, mWonderTarget, mTimer);
    if(mTimer>=1.0f)
    {
        fChangeState(DivideState::Idle);
    }

}

void TutorialEnemy_NoAttack::fStunInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::stun);
    mTimer = mStunTime;
}

void TutorialEnemy_NoAttack::fStunUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mTimer -= elapsedTime_;

    if(mTimer<=0.0f)
    {
        fChangeState(DivideState::Idle);
    }
}
