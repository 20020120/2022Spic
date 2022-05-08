#include"LastBoss.h"

LastBoss::LastBoss(GraphicsPipeline& Graphics_, const DirectX::XMFLOAT3& EmitterPoint_,
    const EnemyParamPack& ParamPack_)
    :BaseEnemy(Graphics_,
        "./resources/Models/Enemy/boss_animation_second.fbx",
        ParamPack_, EmitterPoint_)
{
    // タレットのモデルを初期化
    mpTurret = std::make_unique<SkinnedMesh>(Graphics_.get_device().Get(),
        "./resources/Models/Enemy/boss_turret.fbx");
    mScale = { 0.8f,0.8f,0.8f };
    fRegisterFunctions();
    mDissolve = 0.0f;
    // ボーンを初期化
    mShipFace = mpModel->get_bone_by_name("shipface_top_joint");
    mCurrentMode = Mode::Ship;

    // laserを初期化
    mBeam.fInitialize(Graphics_.get_device().Get(),L"");
    mLaserPointer.fInitialize(Graphics_.get_device().Get(),L"");
}

LastBoss::LastBoss(GraphicsPipeline& Graphics_)
    : BaseEnemy(Graphics_, "./resources/Models/Enemy/boss_animation_second.fbx")
{
    // タレットのモデルを初期化
    mpTurret = std::make_unique<SkinnedMesh>(Graphics_.get_device().Get(),
        "./resources/Models/Enemy/boss_turret.fbx");

    mScale = { 0.05f,0.05f,0.05f };
}

LastBoss::~LastBoss()
{
}

void LastBoss::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fBaseUpdate(elapsedTime_, Graphics_);
    fGuiMenu();
    mBeam.fUpdate();
    mLaserPointer.fSetAlpha(mPointerAlpha);
    mLaserPointer.fUpdate();

    // 各モードから次のモードに遷移する
    switch (mCurrentMode) {
    case Mode::Ship:
        break;
    case Mode::Human:
        // 体力が特定の割合を下回ったら
        fChangeHumanToDragon();
         break;
    case Mode::Dragon:
         break;
    default: ; }
}

void LastBoss::fUpdateAttackCapsule()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fDie()
{
    BaseEnemy::fDie();
}

void LastBoss::fDamaged(int damage, float invincible_time)
{
    BaseEnemy::fDamaged(damage, invincible_time);
}

void LastBoss::fSetStun(bool arg)
{
    BaseEnemy::fSetStun(arg);
}

void LastBoss::fRegisterFunctions()
{
    {
        InitFunc ini = [=]()->void
        {
            fShipStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShipStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipStart, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShipIdleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShipIdleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipIdle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShipBeamStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShipBeamStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipBeamStart, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShipBeamChargeInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShipBeamChargeUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipBeamCharge, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShipBeamShootInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShipBeamShootUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipBeamShoot, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShipBeamEndInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShipBeamEndUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipBeamEnd, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fChangeShipToHumanInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fChangeShipToHumanUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::ShipToHuman, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fHumanIdleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanIdleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanIdle, tuple));
    }

    {
        InitFunc ini = [=]()->void
        {
            fHumanDieStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanDieStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanDieStart, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fHumanDieMiddleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanDieMiddleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanDieMiddle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fHumanToDragonInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanToDragonUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanToDragon, tuple));
    }
    // ドラゴン：待機
    {
        InitFunc ini = [=]()->void
        {
            fDragonIdleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fDragonIdleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::DragonIdle, tuple));
    }

    fChangeState(DivideState::ShipStart);
}

void LastBoss::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Begin("LastBoss");
    ImGui::DragFloat3("Position", &mPosition.x);
    ImGui::DragFloat3("Scale", &mScale.x);
    if (ImGui::Button("Beam"))
    {
        fChangeState(DivideState::ShipBeamStart);
    }

    // 現在のModeを表記
    switch (mCurrentMode) {
    case Mode::Ship:
        ImGui::Text("Mode::Ship");
        break;
    case Mode::Human:
        ImGui::Text("Mode::Human");
        break;
    case Mode::Dragon:
        ImGui::Text("Mode::Dragon");
        break;
    case Mode::HumanToDragon:
        ImGui::Text("Mode::HumanToDragon");
        break;
    default: ;
    }

    if (ImGui::TreeNode("Hp"))
    {
        ImGui::InputInt("CurrentHp", &mCurrentHitPoint);
        float p = fComputePercentHp();
        ImGui::SliderFloat("Percent",&p,0.0f,1.0f);
        ImGui::TreePop();
    }
    
    

    ImGui::End();
#endif
}

LastBoss::AttackKind LastBoss::fRandomAttackFromHp()
{
    // 最大体力に対する今の体力の割合を算出する(0.0f~1.0f)
    float percentFromMAxHp = fComputePercentHp();

    

    return {};
}

float LastBoss::fComputePercentHp() const
{
    return static_cast<float>(mCurrentHitPoint) / static_cast<float>(mMaxHp);
}

void LastBoss::fChangeHumanToDragon()
{
    // 体力の割合が20%を下回ったらモードをドラゴンに変える
    if(fComputePercentHp()<=mkPercentToDragon)
    {
        fChangeState(DivideState::HumanDieStart);
        // 変形中モードに遷移（ダメージは受けない）
        mCurrentMode = Mode::HumanToDragon;
    }
}

