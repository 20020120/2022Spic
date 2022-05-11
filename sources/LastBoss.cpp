#include"LastBoss.h"

#include "BulletManager.h"
#include"EnemyManager.h"

LastBoss::LastBoss(GraphicsPipeline& Graphics_,
    const DirectX::XMFLOAT3& EmitterPoint_,
    const EnemyParamPack& ParamPack_,
    EnemyManager* pEnemyManager_)
    :BaseEnemy(Graphics_,
        "./resources/Models/Enemy/boss_animation_third.fbx",
        ParamPack_, EmitterPoint_),mpEnemyManager(pEnemyManager_)
{
    // �^���b�g�̃��f����������
    fRegisterFunctions();
   
    // �{�[����������
    mShipFace = mpModel->get_bone_by_name("shipface_top_joint");
    mCurrentMode = Mode::Ship;

    // laser��������
    mBeam.fInitialize(Graphics_.get_device().Get(),L"");
    mLaserPointer.fInitialize(Graphics_.get_device().Get(),L"");

    // �^���b�g��������
    mpTurretLeft = std::make_unique<Turret>(Graphics_);
    mpTurretRight = std::make_unique<Turret>(Graphics_);
    // �^���b�g�̃{�[����������
    mTurretBoneLeft = mpModel->get_bone_by_name
    ("armor_L_fire_joint");
    mTurretBoneRight= mpModel->get_bone_by_name
    ("armor_R_fire_joint");

    mpSecondGunLeft = std::make_unique<SecondGun>(Graphics_);
    mpSecondGunRight = std::make_unique<SecondGun>(Graphics_);
    mSecondGunBoneLeft = mpModel->get_bone_by_name
    ("secondarygun_L_root_joint");
    mSecondGunBoneRight = mpModel->get_bone_by_name
    ("secondarygun_R_root_joint");


    // �e���˗p�̊֐����擾
    mfAddBullet= BulletManager::Instance().fGetAddFunction();

}

LastBoss::LastBoss(GraphicsPipeline& Graphics_)
    : BaseEnemy(Graphics_, "./resources/Models/Enemy/boss_animation_second.fbx")
{

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

    // �e���[�h���玟�̃��[�h�ɑJ�ڂ���
    switch (mCurrentMode) {
    case Mode::Ship:
        break;
    case Mode::Human:
        // �̗͂�����̊��������������
        fChangeHumanToDragon();
        break;
    case Mode::Dragon:
        break;
    default:;
    }

    // �^���b�g���X�V
    mpTurretLeft->fUpdate(elapsedTime_, Graphics_);
    mpTurretRight->fUpdate(elapsedTime_, Graphics_);



    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    DirectX::XMFLOAT4X4 rotMat{};
    mpModel->fech_by_bone(mAnimPara, Math::calc_world_matrix(mScale,
        mOrientation, mPosition), mTurretBoneLeft, position,
        up, rotMat);
    debug_figure->create_sphere(position, 20.0f, { 0.0f,0.0f,1.0f,1.0f });
}

void LastBoss::fUpdateAttackCapsule()
{
    throw std::logic_error("Not implemented");
}

void LastBoss::fDie()
{
    fChangeState(DivideState::DragonDieStart);
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
            fHumanAllShotInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanAllShotUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanAllShot, 
            tuple));
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

    {
        InitFunc ini = [=]()->void
        {
            fHumanBlowAttackInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanBlowAttackUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanBlowAttack, tuple));
    }

    {
        InitFunc ini = [=]()->void
        {
            fHumanSpAttackAwayInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fHumanSpAttackAwayUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::HumanSpAway, tuple));
    }



    // �h���S���F�ҋ@
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


    // �h���S���F���S
    {
        InitFunc ini = [=]()->void
        {
            fDragonDieStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fDragonDieStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::DragonDieStart, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fDragonDieMiddleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fDragonDieMiddleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivideState::DragonDieEnd, tuple));
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

    // ���݂�Mode��\�L
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

    if (ImGui::TreeNode("State"))
    {
        if (ImGui::Button("HumanAllShot"))
        {
            fChangeState(DivideState::HumanAllShot);
        }
        if (ImGui::Button("HumanBlow"))
        {
            fChangeState(DivideState::HumanBlowAttack);
        }
        if (ImGui::Button("HumanAway"))
        {
            fChangeState(DivideState::HumanSpAway);
        }
        ImGui::TreePop();
    }
    
    

    ImGui::End();
#endif
}

LastBoss::AttackKind LastBoss::fRandomAttackFromHp()
{
    // �ő�̗͂ɑ΂��鍡�̗̑͂̊������Z�o����(0.0f~1.0f)
    float percentFromMAxHp = fComputePercentHp();

    

    return {};
}

float LastBoss::fComputePercentHp() const
{
    return static_cast<float>(mCurrentHitPoint) / static_cast<float>(mMaxHp);
}

void LastBoss::fChangeHumanToDragon()
{
    // �̗͂̊�����20%����������烂�[�h���h���S���ɕς���
    if(fComputePercentHp()<=mkPercentToDragon)
    {
        fChangeState(DivideState::HumanDieStart);
        // �ό`�����[�h�ɑJ�ځi�_���[�W�͎󂯂Ȃ��j
        mCurrentMode = Mode::HumanToDragon;
    }
}

void LastBoss::fSpawnChildUnit(GraphicsPipeline& Graphics_,int Amounts_)
{
    // ���j�b�g����������

    // �萔
    float SummonRadius = { 30.0f };
    DirectX::XMFLOAT3 SummonCenterPosition = { 0.0f,0.0f,0.0f };

    // ��̓�����̉�]�p���Z�o����
    const float peaceOfRotation = 360.0f / static_cast<float>(Amounts_);
    for (int i = 0; i < Amounts_; ++i)
    {
        // ��]�p�ɉ������ʒu�����肷��
        const float rot = DirectX::XMConvertToRadians(peaceOfRotation * i);
        SummonCenterPosition = { cosf(rot),0.0f,sinf(rot) };
        
        mpEnemyManager->

    }


}


