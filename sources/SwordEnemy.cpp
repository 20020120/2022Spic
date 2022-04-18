#include"SwordEnemy.h"
#include"Operators.h"
SwordEnemy::SwordEnemy(GraphicsPipeline& graphics_,
    int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_,ParamGetFunction Func_ )
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_sword.fbx")
{
    //�X���X�^�[�G�t�F�N�g
    mVernier_effect = std::make_unique<Effect>(graphics_, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\enemy_vernier.efk");
    // �ʒu�̏�����
    mPosition = EmitterPoint_;
    mScale = { 0.05f,0.05f,0.05f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    // �X�e�[�g�}�V����������
    SwordEnemy::fRegisterFunctions();
    mParam.mHitPoint = 100;
    fGetParam(this, Func_);
    mVernier_effect->play(effect_manager->get_effekseer_manager(), mPosition);

    // �U���Ɋւ���parameter��������
    mAttackInformation.mInvincible_time = { 1.0f };
     // �{�[����������
    mSwordBone = mpSkinnedMesh->get_bone_by_name("hand_r_joint");
    mVernierBone = mpSkinnedMesh->get_bone_by_name("burner_r_f_fire");

}

void SwordEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_,Graphics_);
    fSetAttackCapsuleCollider(); // �U���p�̃J�v�Z���ʒu���X�V
    fSetVernierEffectPos();//�o�[�j�A�̈ʒu���X�V

}

void SwordEnemy::fRegisterFunctions()
{
    // �֐����X�e�[�g�}�V���ɓo�^
    {
        InitFunc ini = [=]()->void
        {
            fSpawnInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fSpawnUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fWalkInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fWalkUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Move, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fAttackBeginInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackBeginUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackBegin, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fAttackPreActionInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackPreActionUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackMiddle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fAttackEndInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackEndUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackEnd, tuple));
    }

    fChangeState(DivedState::Start);
}

void SwordEnemy::fSetAttackCapsuleCollider()
{
    //--------------------<���̃J�v�Z���̈ʒu�����肷��>--------------------//
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    // �{�[���̖��O����ʒu�Ə�x�N�g�����擾
    mpSkinnedMesh->fech_by_bone(mAnimPara,
        Math::calc_world_matrix(mScale, mOrientation, mPosition),
        mSwordBone, position, up);
    up = Math::Normalize(up);

    mAttackCapsuleCollider.mPointA = position + up * 5.0f;
    mAttackCapsuleCollider.mPointB = position + up * 1.0f;
    mAttackCapsuleCollider.mRadius = 2.0f;

}

void SwordEnemy::fSetVernierEffectPos()
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
    /* DirectX::XMMATRIX R_MAT = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&rotate_mat));
    DirectX::XMFLOAT4X4 r_mat;
    DirectX::XMStoreFloat4x4(&r_mat, R_MAT);*/
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
    //mVernier_effect->set_rotationY(effect_manager->get_effekseer_manager(), DirectX::XMConvertToRadians(ang));
    mVernier_effect->set_posture(effect_manager->get_effekseer_manager(), r_mat,ang);
}

void SwordEnemy::fSpawnInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::idle, true);
    // �ėp�^�C�}�[��������
    mWaitTimer = 0.0f;
}

void SwordEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;

    // ��莞�Ԍo�߂ňړ��ɑJ��
    if(mWaitTimer>=mSpawnDelaySec)
    {
        fChangeState(DivedState::Move);
    }
}

void SwordEnemy::fWalkInit()
{
    // �A�j���[�V�������Đ�
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::walk,true);
    mWaitTimer = 0.0f;
}

void SwordEnemy::fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<�v���C���[�̂�������Ɉړ�>--------------------//
    const auto moveVec = mPlayerPosition - mPosition;
    fMoveVelocity(elapsedTime_, moveVec, mMoveSpeed);
    //--------------------<�v���C���[�̕����ɉ�]>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    // �v���C���[�Ƃ̋��������ȉ��ɂȂ�����
    if(mAttackRange >= Math::Length(mPlayerPosition-mPosition))
    {
        fChangeState(DivedState::AttackBegin);
    }
}

//--------------------<����U��グ��>--------------------//
void SwordEnemy::fAttackBeginInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::attack_idle);
    mWaitTimer = 0.0f;
}
void SwordEnemy::fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToTarget(elapsedTime_, mPlayerPosition, 20.0f);

    // �^�C�}�[�����Z
    mWaitTimer += elapsedTime_;
    if(mWaitTimer>=mAttackBeginTimeSec*mAnimationSpeed)
    {
        fChangeState(DivedState::AttackMiddle);
    }
}
//--------------------<����U�艺�낷�\������>--------------------//
void SwordEnemy::fAttackPreActionInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_up, false, false);
    mWaitTimer = 0.0f;
}
void SwordEnemy::fAttackPreActionUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    if (mWaitTimer >= mAttackPreActionTimeSec * mAnimationSpeed)
    {
        fChangeState(DivedState::AttackEnd);
    }
}
//--------------------<�U�����i�����蔻��ON�j>--------------------//
void SwordEnemy::fAttackEndInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_down, false, false);
    mWaitTimer = 0.0f;
    mAttackInformation.mIsAttack = true;
}
void SwordEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    if(mWaitTimer>=mAttackDownSec)
    {
        fChangeState(DivedState::Start);
        mAttackInformation.mIsAttack = false;
    }
}


void SwordEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SwordEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}
