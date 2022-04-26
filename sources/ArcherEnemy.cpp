#include"ArcherEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "NormalEnemy.h"
#include "StraightBullet.h"
#include "Operators.h"
//****************************************************************
//
// ���Ȃ��ʏ�U���̎G���G�̔h���N���X
//
//****************************************************************
ArcherEnemy::ArcherEnemy(GraphicsPipeline& Graphics_,
    const DirectX::XMFLOAT3& EmitterPoint_,
    EnemyParamPack ParamPack_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/enemy_arrow.fbx",
        ParamPack_,
        EmitterPoint_)
{

    // �ʒu��������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.03f,0.03f,0.03f };
    //�p�����[�^�[�̏�����
    fParamInitialize();
    fRegisterFunctions();
    mVernier_effect->play(effect_manager->get_effekseer_manager(), mPosition);

}

ArcherEnemy::~ArcherEnemy()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());

}


void ArcherEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    //--------------------<�X�V����>--------------------//
    fBaseUpdate(elapsedTime_, Graphics_);
}


void ArcherEnemy::fRegisterFunctions()
{
    //�o���Ԃ̓o�^
    InitFunc Ini = [=]()->void
    {
        fSpawnInit();
    };
    UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fSpawnUpdate(elapsedTime_, Graphics_);
    };
    FunctionTuple tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::Start, tuple));

    //�ҋ@��Ԃ̓o�^
    Ini = [=]()->void
    {
        fIdleInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fIdleUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::Idle, tuple));

    

    //�ړ���Ԃ̓o�^
    Ini = [=]()->void
    {
        fMoveInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fmoveUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::Move, tuple));

    //�ڋ߈ړ���Ԃ̓o�^
    Ini = [=]()->void
    {
        fMoveApproachInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fMoveApproachUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::Approach, tuple));

    //��ވړ���Ԃ̓o�^
    Ini = [=]()->void
    {
        fMoveLeaveInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fMoveLeaveUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::Leave, tuple));

    //�U��������Ԃ̓o�^
    Ini = [=]()->void
    {
        fAttackBeginInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fAttackBeginUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::AttackReady, tuple));

    //�U���ҋ@�̓o�^
    Ini = [=]()->void
    {
        fAttackPreActionInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fAttackPreActionUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::AttackIdle, tuple));

    //�U����Ԃ̓o�^
    Ini = [=]()->void
    {
        fAttackEndInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fAttackEndUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::AttackShot, tuple));

    //�Ђ�ݏ�Ԃ̓o�^
    Ini = [=]()->void
    {
        fDamagedInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fDamagedUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DivedState::Damaged, tuple));

    //������
    fChangeState(DivedState::Start);

}

void ArcherEnemy::fParamInitialize()
{
       mStayTimer = 1.0f;
    mAttack_flg = false;
}

void ArcherEnemy::fSpawnInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
    // �ėp�^�C�}�[��������
    mStayTimer = 0.0f;
}

void ArcherEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mStayTimer += elapsedTime_;
    mDissolve -= elapsedTime_;

    // ��莞�Ԍo�߂ňړ��ɑJ��
    if (mStayTimer >= SPAWN_STAY_TIME)
    {
        fChangeState(DivedState::Idle);
    }
}

void ArcherEnemy::fIdleInit()
{
    //mpSkinnedMesh->play_animation(IDLE, true, 0.1f);

}

void ArcherEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mStayTimer -= elapsedTime_;
    if (mStayTimer > 0.0f) return;
    mStayTimer = 0.0f;
    fChangeState(DivedState::Move);
}

void ArcherEnemy::fMoveInit()
{
  
     mpModel->play_animation(mAnimPara, AnimationName::walk, true, 0.1f);
    mAttackingTime = 0.0f;
}

void ArcherEnemy::fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //�v���C���[�̕����ɉ�]
    fTurnToPlayer(elapsedTime_);
    //�����Ă�������ɑS�g
    fMove(elapsedTime_);

    const float LengthFromPlayer = Math::calc_vector_AtoB_length(mPosition, mPlayerPosition);
    //�v���C���[�Ƃ̋������U���\�����ȉ��Ȃ痣��Ă����s�����Ƃ�
    if (LengthFromPlayer < AT_SHORTEST_DISTANCE)
    {
        fChangeState(DivedState::Leave);
        return;
    }

    //�v���C���[�Ƃ̋������U���\�����ȏ�Ȃ�߂Â��s�����Ƃ�
    if (LengthFromPlayer > AT_LONGEST_DISTANCE )
    {
        fChangeState(DivedState::Approach);
    }

}

void ArcherEnemy::fMoveApproachInit()
{

}

void ArcherEnemy::fMoveApproachUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fMove(elapsedTime_);
    fTurnToPlayer(elapsedTime_);
    const float LengthFromPlayer = Math::calc_vector_AtoB_length(mPosition, mPlayerPosition);

    if (LengthFromPlayer > AT_SHORTEST_DISTANCE && LengthFromPlayer < AT_LONGEST_DISTANCE)
    {
        fChangeState(DivedState::AttackReady);
        return;
    }

    if (LengthFromPlayer < AT_SHORTEST_DISTANCE)
    {
        fChangeState(DivedState::Leave);
    }
}

void ArcherEnemy::fMoveLeaveInit()
{
}

void ArcherEnemy::fMoveLeaveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    using namespace DirectX;
    //�G�l�~�[����v���C���[�ւ̋t�������Z�o
    XMVECTOR pos_vec = XMLoadFloat3(&mPosition);//�����̈ʒu
    DirectX::XMFLOAT3 t = mPlayerPosition;
    XMVECTOR target_vec = XMLoadFloat3(&t);
    //�����̃|�W�V��������v���C���[�̃|�W�V�������������ƂŃv���C���[�Ɣ��Ε����̃x�N�g�����o��
    XMVECTOR tar = XMVector3Normalize(pos_vec - target_vec);
    XMVectorScale(tar, 3.0f);
    XMFLOAT3 t_vec{};
    XMStoreFloat3(&t_vec, tar);
    XMFLOAT3 target_pos = mPosition + t_vec;

    fMove(elapsedTime_);
    fTurnToPlayer(elapsedTime_);

    const float LengthFromPlayer = Math::calc_vector_AtoB_length(mPosition, mPlayerPosition);
    if (LengthFromPlayer > AT_SHORTEST_DISTANCE && LengthFromPlayer < AT_LONGEST_DISTANCE)
    {
        fChangeState(DivedState::AttackReady);
        return;
    }

    if (LengthFromPlayer > AT_LONGEST_DISTANCE)
    {
        fChangeState(DivedState::Approach);
    }
}

void ArcherEnemy::fAttackBeginInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::attack_ready);
    mStayTimer = 0.0f;
}

void ArcherEnemy::fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_);
    if (mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivedState::AttackIdle);
    }
}

void ArcherEnemy::fAttackPreActionInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::attack_idle);
    mStayTimer = 0.0f;

}

void ArcherEnemy::fAttackPreActionUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToPlayer(elapsedTime_);
    if (mpModel->end_of_animation(mAnimPara))
    {
        fChangeState(DivedState::AttackShot);
    }
}

void ArcherEnemy::fAttackEndInit()
{
    mpModel->play_animation(mAnimPara,AnimationName::attack_shot);
    mAttackingTime = 0.0f;
}

void ArcherEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    DirectX::XMFLOAT3 tar_pos = { mPlayerPosition.x, mPlayerPosition.y + 3.5f, mPlayerPosition.z };
    fTurnToPlayer(elapsedTime_);
    //�e���U
        float bullet_speed = 1.0f * 0.2f;
        auto straightBullet = new StraightBullet(Graphics_,
            mPosition, Math::GetFront(mOrientation) * bullet_speed);
        //�p�����[�^�ݒ�
        straightBullet->fSetBulletData(0.5f, 0.5f, 0.5f, 1, 1.0f);
        //�o�^
        mfAddFunc(straightBullet);

        //�ҋ@���Ԑݒ�
        mStayTimer = 3.0f;
        fChangeState(DivedState::Idle);
    
}

void ArcherEnemy::fDamagedInit()
{
}


void ArcherEnemy::fDamagedUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    using namespace DirectX;
    XMVECTOR P_Pos = XMLoadFloat3(&mPlayerPosition);
    XMVECTOR E_Pos = XMLoadFloat3(&mPosition);
    XMVECTOR Vec = E_Pos - P_Pos;
    Vec = XMVector3Normalize(Vec);
    XMFLOAT3 v;
    XMStoreFloat3(&v, Vec);
   

    fChangeState(DivedState::Idle);
}

void ArcherEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text("Name : arch");
    ImGui::DragFloat3("position", &mPosition.x);
    ImGui::DragFloat3("angle", &mOrientation.x);
    const char* state_list[] = { "IDLE","MOVE","ATTACK","DAUNTED" };
    /* std::string state =  state_list[static_cast<int>(mNowState)];
     ImGui::Text("State"); ImGui::SameLine();
     ImGui::Text(state.c_str());*/
    ImGui::Checkbox("Attack", &mAttack_flg);
    if (ImGui::Button("dameged", { 70.0f,30.0f }))
    {
        fDamaged(1, 0.6f);
    }
#endif

}

void ArcherEnemy::fMove(float elapsed_time)
{
    //�^�[�Q�b�g�Ɍ������ĉ�]
    DirectX::XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&mOrientation);
    DirectX::XMVECTOR forward;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    forward = { m4x4._31, m4x4._32, m4x4._33 };
    forward = DirectX::XMVector3Normalize(forward);
    DirectX::XMFLOAT3 f;
    DirectX::XMStoreFloat3(&f, forward);


    mPosition += f * MAX_MOVE_SPEED * elapsed_time;

}
