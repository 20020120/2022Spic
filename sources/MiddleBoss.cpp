#include"MiddleBoss.h"
#include"StraightBullet.h"
#include "Operators.h"
MiddleBoss::MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_, AddBulletFunc Func_)
    :BaseEnemy(Graphics_,0, "./resources/Models/Enemy/enemy_proto.fbx")
{
    mScale = { 0.03f,0.03f,0.03f };
    mPosition = { 0.0f,0.0f,0.0f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    fGetParam(this, Function_);
    fRegisterFunctions();
    // ƒr[ƒ€‚ð‰Šú‰»
    mLaserBeam.fInitialize(Graphics_.get_device().Get(),
        L"./resources/TexMaps/SwordTrail/trajectory_.png");
    mfAddFunc = Func_;
}

void MiddleBoss::fInitialize()
{
    
}
void MiddleBoss::fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_)
{
    fUpdateBase(elapsedTime_,Graphics_);
    mLaserBeam.fUpdate();
    fGuiMenu(Graphics_);
    fShotStraightBullet(Graphics_);
}

void MiddleBoss::fGuiMenu(GraphicsPipeline& Graphics_)
{
#ifdef USE_IMGUI
    ImGui::Begin("MiddleBoss");
    ImGui::DragFloat("Position", &mPosition.x);
    if (ImGui::Button("Shot"))
    {
        auto straightBullet = new StraightBullet(Graphics_,
            mPosition, Math::GetFront(mOrientation)*10.0f);
        mfAddFunc(straightBullet);
    }
    ImGui::End();
#endif
}

void MiddleBoss::fDamaged(int damage, float invinsible_time)
{
    BaseEnemy::fDamaged(damage, invinsible_time);
}

void MiddleBoss::fSetCapsulePoint()
{
    BaseEnemy::fSetCapsulePoint();
}

void MiddleBoss::fRegisterFunctions()
{
    {
        auto Ini = [=]()->void
        {
            fStartInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStartUpdate(elapsedTime_, Graphics_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::Start, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fTourInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fTourUpdate(elapsedTime_, Graphics_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::Tour, tuple));
    }
    fChangeState(State::Tour);
}

void MiddleBoss::fShotStraightBullet(GraphicsPipeline& Graphics_)
{
    auto straightBullet = new StraightBullet(Graphics_,
        mPosition, Math::GetFront(mOrientation) * 10.0f);
    mfAddFunc(straightBullet);
}


void MiddleBoss::fRender(GraphicsPipeline& graphics)
{
    mLaserBeam.fRender(graphics);
    BaseEnemy::fRender(graphics);
}
