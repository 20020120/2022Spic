#include"MiddleBoss.h"
#include"StraightBullet.h"
#include "Operators.h"
MiddleBoss::MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_, AddBulletFunc Func_)
    :BaseEnemy(Graphics_,0, "./resources/Models/Enemy/MiddleBoss.fbx")
{
    mTimer = 0.0f;
    mScale = { 0.03f,0.03f,0.03f };
    mPosition = { 0.0f,0.0f,0.0f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    fGetParam(this, Function_);
    fRegisterFunctions();

    // ビームを初期化
    mLaserPointer.fInitialize(Graphics_.get_device().Get(),
        L"./resources/TexMaps/SwordTrail/trajectory_.png");
    mLaserBeam.fInitialize(Graphics_.get_device().Get(),
        L"./resources/TexMaps/SwordTrail/trajectory_.png");

    mfAddFunc = Func_;

    //--------------------<LuaLibの呼び出し>--------------------//
    mLuaWorld.fLoadFile("./resources/Data/MiddleBoss.lua");

    //--------------------<アニメーション再生>--------------------//
    mpSkinnedMesh->play_animation(wing, true);

}

void MiddleBoss::fInitialize()
{
    
}
void MiddleBoss::fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_)
{
    // 汎用タイマーを更新
    mTimer += elapsedTime_;

    // 基底クラスの更新処理
    fUpdateBase(elapsedTime_,Graphics_);
    // このクラスの更新処理
    mLaserPointer.fUpdate();
    mLaserBeam.fUpdate();
    // 照準は常にプレイヤーの方向に合わせておく
    auto endPoint = mPlayerPosition;
    endPoint.y += 1.5f;
    mLaserPointer.fSetPosition(mPosition, endPoint);
    mLaserPointer.fSetRadius(0.02f);
    mLaserBeam.fSetLengthThreshold(mLaserBeamLength);
    mLaserBeam.fSetRadius(mBeamRadius);
    mLaserBeam.fSetColor({ 1.0f,1.0f,0.0f,1.0f });

    fGuiMenu(Graphics_);
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

    if(ImGui::CollapsingHeader("StateMachine"))
    {
       if (ImGui::Button("TourLaserReady"))
       {
           fChangeState(State::TourBeamReady);
       }

       if(ImGui::Button("SecondMode"))
       {
           fChangeState(State::ModeChange);
       }

       if (ImGui::Button("SecondShot"))
       {
           fChangeState(State::SecondShot);
       }
    }
    if(ImGui::Button("LuaReload"))
    {
        mLuaWorld.fLoadFile("./resources/Data/MiddleBoss.lua");
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
    {
        auto Ini = [=]()->void
        {
            fTourLaserReadyInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fTourLaserReadyUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::TourBeamReady, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fTourLaserInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fTourLaserUpdate(elapsedTime_, Graphics_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::TourBeam, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fTourShotInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fTourShotUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::TourShot, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fTourToSecondModeInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fTourToSecondModeUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::ModeChange, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fSecondIdleInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fSecondIdleUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::SecondIdle, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fSecondShotInit();
        };
        auto up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fSecondShotUpdate(elapsedTime_, Graphics_);
        };
        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::SecondShot, tuple));
    }

    fChangeState(State::Tour);
}

void MiddleBoss::fShotStraightBullet(GraphicsPipeline& Graphics_)
{
    auto straightBullet = new StraightBullet(Graphics_,
        mPosition, Math::GetFront(mOrientation) * 10.0f);
    mfAddFunc(straightBullet);
}

void MiddleBoss::fShotToTarget(DirectX::XMFLOAT3 TargetPosition_,GraphicsPipeline& Graphics_)
{
    auto v = TargetPosition_ - mPosition;
    v = Math::Normalize(v);
    //v* TourBulletSpeed;
    mfAddFunc(new StraightBullet(Graphics_, mPosition, v));
}


void MiddleBoss::fRender(GraphicsPipeline& graphics)
{
    mLaserPointer.fRender(graphics);
    mLaserBeam.fRender(graphics);
    BaseEnemy::fRender(graphics);
}
