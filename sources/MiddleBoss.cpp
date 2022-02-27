#include"MiddleBoss.h"

MiddleBoss::MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_)
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
}

void MiddleBoss::fInitialize()
{
    
}
void MiddleBoss::fUpdate(float elapsedTime_)
{
    fUpdateBase(elapsedTime_);
    mLaserBeam.fUpdate();
}

void MiddleBoss::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Begin("MiddleBoss");
    ImGui::DragFloat("Position", &mPosition.x);
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
        auto up = [=](float elapsedTime_)->void
        {
            fStartUpdate(elapsedTime_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::Start, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fTourInit();
        };
        auto up = [=](float elapsedTime_)->void
        {
            fTourUpdate(elapsedTime_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::Tour, tuple));
    }
    fChangeState(State::Tour);
}


void MiddleBoss::fRender(GraphicsPipeline& graphics)
{
    mLaserBeam.fRender(graphics);
    BaseEnemy::fRender(graphics);
}
