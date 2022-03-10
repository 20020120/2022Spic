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
    :BaseEnemy(graphics_, UniqueId_, "./resources/Models/Enemy/character_4.fbx")
{
    // �ʒu��������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.03f,0.03f,0.03f };
    //�p�����[�^�[�̏�����
    fParamInitialize();
    fGetParam(this, Function_);
    fRegisterFunctions();

    mfAddFunc = Func_;

}

void ArcherEnemy::fInitialize()
{

}

void ArcherEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    //--------------------<�X�V����>--------------------//
    fUpdateBase(elapsedTime_, Graphics_);
}


void ArcherEnemy::fRegisterFunctions()
{
    //�ҋ@��Ԃ̓o�^
    InitFunc Ini = [=]()->void
    {
        fIdleInit();
    };
    UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fIdleUpdate(elapsedTime_, Graphics_);
    };
    FunctionTuple tuple = std::make_tuple(Ini, Up);
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

    //�U����Ԃ̓o�^
    Ini = [=]()->void
    {
        fAttackInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fAttackUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Attack, tuple));

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
    fChangeState(State::Idle);

}

void ArcherEnemy::fParamInitialize()
{
    //mParam.mHitPoint = 10;      // �̗�
    //mParam.mAttackPower = 10;   // �U����
    //mParam.mMoveSpeed = 10;   // �ړ����x
    //mParam.mAttackSpeed = 2; // �U���Ԋu
    mStayTimer = 1.0f;
    mAttack_flg = false;
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


void ArcherEnemy::fIdleInit()
{
    //mpSkinnedMesh->play_animation(IDLE, true, 0.1f);

}

void ArcherEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    fChangeState(State::Move);
}

void ArcherEnemy::fMoveInit()
{
    max_move_speed = mParam.mMoveSpeed;
    // mpSkinnedMesh->play_animation(MOVE, true, 0.1f);
    mAttackingTime = 0.0f;
}

void ArcherEnemy::fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    //�v���C���[�Ƃ̋������W�ȉ��Ȃ痣��Ă����s�����Ƃ�
    if (mLengthFromPlayer < 8.0f)
    {
        fChangeState(State::Leave);
        return;
    }

    //�v���C���[�Ƃ̋�����15�ȏ�Ȃ�߂Â��s�����Ƃ�
    if (mLengthFromPlayer > 15.0f)
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
    if (mLengthFromPlayer > 15.0f  && mLengthFromPlayer < 30.0f)
    {
        fChangeState(State::Attack);
        //�e���U
        auto straightBullet = new StraightBullet(Graphics_,
            mPosition, Math::GetFront(mOrientation) * 10.0f);
        mfAddFunc(straightBullet); 
        return;
    }

    if(mLengthFromPlayer < 14.0f)
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

    if (mLengthFromPlayer > 15.0f && mLengthFromPlayer < 30.0f)
    {
        fChangeState(State::Attack);

        //�e���U
        auto straightBullet = new StraightBullet(Graphics_,
            mPosition, Math::GetFront(mOrientation) * 10.0f);
        mfAddFunc(straightBullet);
        return;
    }

    if (mLengthFromPlayer > 30.0f)
    {
        fChangeState(State::Approach);
    }
}

void ArcherEnemy::fAttackInit()
{
    // mpSkinnedMesh->play_animation(ATTACK, true, 0.1f);
    mAttackingTime = 0.0f;
    fSetAttackPower(2, 1.5f);

}

void ArcherEnemy::fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fAttackStart();
    DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&forward);
    DirectX::XMFLOAT3 attack_pos;
    DirectX::XMStoreFloat3(&attack_pos, DirectX::XMVectorScale(Forward, 2.0f));
    fSetAttackRange(attack_pos, up, 1.5f, 1.5f);

    mAttackingTime += elapsedTime_;
    if (mAttackingTime > 2.0f)
    {
        fAttackEnd();

        fChangeState(State::Idle);
    }
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
    ImGui::Text("Name : Normal");
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
