#include"TutorialEnemy.h"

TutorialEnemy_NoMove::TutorialEnemy_NoMove(GraphicsPipeline& graphics, 
     const DirectX::XMFLOAT3& entry_position, const EnemyParamPack& param ): BaseEnemy(graphics, 
         "./resources/Models/Enemy/enemy_tutorial.fbx", param, entry_position)
{
    mPosition = entry_position;
    mScale = { 0.06f,0.06f,0.06f };
    fRegisterFunctions();
}

TutorialEnemy_NoMove::TutorialEnemy_NoMove(GraphicsPipeline& graphics): BaseEnemy(graphics, "./resources/Models/Enemy/enemy_tutorial.fbx")
{}

void TutorialEnemy_NoMove::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    elapsedTime_=fBaseUpdate(elapsedTime_, Graphics_);
}

void TutorialEnemy_NoMove::fUpdateAttackCapsule()
{
    
}

void TutorialEnemy_NoMove::fRegisterFunctions()
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
    fChangeState(DivideState::Start);
}

void TutorialEnemy_NoMove::fStartInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
    mDissolve = 1.0f;
}

void TutorialEnemy_NoMove::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolve -= elapsedTime_;
    if(mDissolve<0.0f)
    {
        fChangeState(DivideState::Idle);
    }
}

void TutorialEnemy_NoMove::fIdleInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
}

void TutorialEnemy_NoMove::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(mIsStun)
    {
        fChangeState(DivideState::Stun);
    }
}

void TutorialEnemy_NoMove::fStunInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::stun);
    mTimer = mStunTime;
}

void TutorialEnemy_NoMove::fStunUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mTimer -= elapsedTime_;

    if(mTimer<=0.0f)
    {
        fChangeState(DivideState::Idle);
    }
}
