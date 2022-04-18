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
ArcherEnemy::ArcherEnemy(GraphicsPipeline& graphics_, DirectX::XMFLOAT3 EmitterPoint_, int UniqueId_,
    ParamGetFunction Function_, AddBulletFunc Func_)
    :BaseEnemy(graphics_, UniqueId_, "./resources/Models/Enemy/enemy_arrow.fbx")
{
    //�X���X�^�[�G�t�F�N�g
    mVernier_effect = std::make_unique<Effect>(graphics_, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\sluster_enemy2.efk");

    // �ʒu��������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.03f,0.03f,0.03f };
    //�p�����[�^�[�̏�����
    fParamInitialize();
    fGetParam(this, Function_);
    fRegisterFunctions();
    mVernier_effect->play(effect_manager->get_effekseer_manager(), mPosition);
    mVernierBone = mpSkinnedMesh->get_bone_by_name("burner_back_center_fire");

    mfAddFunc = Func_;

}

ArcherEnemy::~ArcherEnemy()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());

}

void ArcherEnemy::fInitialize()
{

}

void ArcherEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    //--------------------<�X�V����>--------------------//
    fUpdateBase(elapsedTime_, Graphics_);
    fSetVernierEffectPos();
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
    mFunctionMap.insert(std::make_pair(State::Start, tuple));

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
    mFunctionMap.insert(std::make_pair(State::Idle, tuple));

    

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
    mFunctionMap.insert(std::make_pair(State::Move, tuple));

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
    mFunctionMap.insert(std::make_pair(State::Approach, tuple));

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
    mFunctionMap.insert(std::make_pair(State::Leave, tuple));

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
    mFunctionMap.insert(std::make_pair(State::AttackReady, tuple));

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
    mFunctionMap.insert(std::make_pair(State::AttackIdle, tuple));

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
    mFunctionMap.insert(std::make_pair(State::AttackShot, tuple));

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
    mFunctionMap.insert(std::make_pair(State::Damaged, tuple));

    //������
    fChangeState(State::Start);

}

void ArcherEnemy::fParamInitialize()
{
    //mParam.mHitPoint = 10;      // �̗�
    mParam.mAttackPower = 10;   // �U����
    //mParam.mMoveSpeed = 10;   // �ړ����x
    //mParam.mAttackSpeed = 2; // �U���Ԋu
    mStayTimer = 1.0f;
    mAttack_flg = false;
}

void ArcherEnemy::fSpawnInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::idle, true);
    // �ėp�^�C�}�[��������
    mStayTimer = 0.0f;
}

void ArcherEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mStayTimer += elapsedTime_;
    mDissolveThreshold -= elapsedTime_;

    // ��莞�Ԍo�߂ňړ��ɑJ��
    if (mStayTimer >= SPAWN_STAY_TIME)
    {
        fChangeState(State::Idle);
    }
}

void ArcherEnemy::fDamaged(int Damage_, float InvinsibleTime_)
{
    //�_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
    if (Damage_ == 0)return;

    //���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
    if (mParam.mHitPoint <= 0)return;


    if (mInvinsibleTimer > 0.0f)return;

    //���G���Ԑݒ�
    mInvinsibleTimer = InvinsibleTime_;
    //�_���[�W����
    mParam.mHitPoint -= Damage_;
    fChangeState(State::Damaged);

    // ���S�����甚���G�t�F�N�g���o��
    if (mParam.mHitPoint <= 0)
    {
        fDieEffect();
    }
}

void ArcherEnemy::fStopEffect()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());

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
    fChangeState(State::Move);
}

void ArcherEnemy::fMoveInit()
{
    max_move_speed = 5.0f;
     mpSkinnedMesh->play_animation(mAnimPara, AnimationName::walk, true, 0.1f);
    mAttackingTime = 0.0f;
}

void ArcherEnemy::fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    //�v���C���[�Ƃ̋������U���\�����ȉ��Ȃ痣��Ă����s�����Ƃ�
    if (mLengthFromPlayer < AT_SHORTEST_DISTANCE)
    {
        fChangeState(State::Leave);
        return;
    }

    //�v���C���[�Ƃ̋������U���\�����ȏ�Ȃ�߂Â��s�����Ƃ�
    if (mLengthFromPlayer > AT_LONGEST_DISTANCE)
    {
        fChangeState(State::Approach);
    }

}

void ArcherEnemy::fMoveApproachInit()
{

}

void ArcherEnemy::fMoveApproachUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, mPlayerPosition);
    if (mLengthFromPlayer > AT_SHORTEST_DISTANCE && mLengthFromPlayer < AT_LONGEST_DISTANCE)
    {
        fChangeState(State::AttackReady);
        return;
    }

    if (mLengthFromPlayer < AT_SHORTEST_DISTANCE)
    {
        fChangeState(State::Leave);
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
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, target_pos);

    if (mLengthFromPlayer > AT_SHORTEST_DISTANCE && mLengthFromPlayer < AT_LONGEST_DISTANCE)
    {
        fChangeState(State::AttackReady);
        return;
    }

    if (mLengthFromPlayer > AT_LONGEST_DISTANCE)
    {
        fChangeState(State::Approach);
    }
}

void ArcherEnemy::fSetVernierEffectPos()
{
    //--------------------<�o�[�j�A�̂̈ʒu�����肷��>--------------------//
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    DirectX::XMFLOAT4X4 q{};


    // �{�[���̖��O����ʒu�Ə�x�N�g�����擾
    mpSkinnedMesh->fech_by_bone(mAnimPara,
        Math::calc_world_matrix(mScale, mOrientation, mPosition),
        mVernierBone, position, up, q);

    mVernier_effect->set_position(effect_manager->get_effekseer_manager(), position);
    DirectX::XMFLOAT4X4 corfinate_mat = Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP);
    auto transformQuaternionToRotMat = [&](DirectX::XMFLOAT4X4& q,
        float qx, float qy, float qz, float qw)
    {
        q._11 = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
        q._12 = 2.0f * qx * qy + 2.0f * qw * qz;
        q._13 = 2.0f * qx * qz - 2.0f * qw * qy;

        q._21 = 2.0f * qx * qy - 2.0f * qw * qz;
        q._22 = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
        q._23 = 2.0f * qy * qz + 2.0f * qw * qx;

        q._31 = 2.0f * qx * qz + 2.0f * qw * qy;
        q._32 = 2.0f * qy * qz - 2.0f * qw * qx;
        q._33 = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;
    };
    DirectX::XMFLOAT4X4 r_mat;

    transformQuaternionToRotMat(r_mat, mOrientation.x, mOrientation.y, mOrientation.z, mOrientation.w);
    static float ang = 0;
    mVernier_effect->set_posture(effect_manager->get_effekseer_manager(), r_mat, ang);
}

void ArcherEnemy::fAttackBeginInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_ready);
    mStayTimer = 0.0f;
}

void ArcherEnemy::fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToTarget(elapsedTime_, mPlayerPosition, 20.0f);
    if (mpSkinnedMesh->end_of_animation(mAnimPara))
    {
        fChangeState(State::AttackIdle);
    }
}

void ArcherEnemy::fAttackPreActionInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_idle);
    mStayTimer = 0.0f;

}

void ArcherEnemy::fAttackPreActionUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToTarget(elapsedTime_, mPlayerPosition, 20.0f);
    if (mpSkinnedMesh->end_of_animation(mAnimPara))
    {
        fChangeState(State::AttackShot);
    }
}

void ArcherEnemy::fAttackEndInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::attack_shot);
    mAttackingTime = 0.0f;
    fSetAttackPower(2, 1.5f);
}

void ArcherEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    DirectX::XMFLOAT3 tar_pos = { mPlayerPosition.x, mPlayerPosition.y + 3.5f, mPlayerPosition.z };
    fTurnToTarget(elapsedTime_, tar_pos);
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
        fChangeState(State::Idle);
    
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
    velocity.x = 20.0f * v.x;
    velocity.y = 20.0f * v.y;
    velocity.z = 20.0f * v.z;

    fChangeState(State::Idle);
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
