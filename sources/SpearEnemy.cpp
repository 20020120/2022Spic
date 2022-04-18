#include"SpearEnemy.h"
#include"Operators.h"
SpearEnemy::SpearEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_,
    ParamGetFunction Func_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_spear.fbx")
{
   // �p����������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.05f,0.05f,0.05f };
    mParam.mHitPoint = 1;
    fRegisterFunctions();
    fGetParam(this, Func_);

    //�X���X�^�[�G�t�F�N�g
    mVernier_effect = std::make_unique<Effect>(graphics_, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\sluster_enemy2.efk");
    mVernier_effect->play(effect_manager->get_effekseer_manager(), mPosition);
    // �{�[����������
    mVernierBone = mpSkinnedMesh->get_bone_by_name("burner_back_center_fire");
}


void SpearEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_, Graphics_);
    fSetEffectPosition();
}

void SpearEnemy::fStopEffect()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());
}

void SpearEnemy::fSetEffectPosition()
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
    mVernier_effect->set_posture(effect_manager->get_effekseer_manager(), r_mat, ang);
}

void SpearEnemy::fRegisterFunctions()
{
    // �֐����X�e�[�g�}�V���ɓo�^
    {
        InitFunc ini = [=]()->void
        {
            fIdleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fIdleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Idle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fMoveInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fMoveUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Move, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fThrustBeginInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fThrustBeginUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ThrustBegin, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fThrustMiddleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fThrustMiddleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ThrustMiddle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fThrustEndInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fThrustEndUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ThrustEnd, tuple));
    }
    fChangeState(DivedState::Start);
}

//--------------------<�J�n��>--------------------//
void SpearEnemy::fStartInit()
{
    
}

void SpearEnemy::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolveThreshold -= elapsedTime_;
    if (mDissolveThreshold <= 0.0f)
    {
        fChangeState(DivedState::Idle);
    }
}

//--------------------<�ҋ@>--------------------//
void SpearEnemy::fIdleInit()
{
    // �A�j���[�V�������Đ�
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::idle, true);
    
}
void SpearEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
    if(mWaitTime>=mIdleSec)
    {
        fChangeState(DivedState::Move);
    }
}
//--------------------<�ړ�>--------------------//
void SpearEnemy::fMoveInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::walk, true);
    // �ːi��̍��W��������
    mThrustTarget = mPlayerPosition;

}
void SpearEnemy::fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<�v���C���[�̕����ɉ�]>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition,10.0f);
    // �v���C���[�Ɠːi�����̂Ȃ��p�����ȉ��ɂȂ�����ːi����
    auto v1 = mThrustTarget- mPosition;
    auto v2 = forward;
    v1 = Math::Normalize(v1);
    v2 = Math::Normalize(v2);
    float dot = Math::Dot(v1, v2);
    dot = acosf(dot);
    dot = DirectX::XMConvertToDegrees(dot);

    // ��苗���ɂȂ�����ːi���[�V�����ɓ���
    if(mLengthFromPlayer<=mAttackLength&&
        fabs(dot)<=mThrustDegree)
    {
        fChangeState(DivedState::ThrustBegin);
    }
    else
    {
        //--------------------<�v���C���[�̂�������Ɉړ�>--------------------//
        auto moveVec = mPlayerPosition - mPosition;
        moveVec = Math::Normalize(moveVec);
        velocity = moveVec * mMoveSpeed;
        mPosition += velocity * elapsedTime_;
    }
}
//--------------------<�ːi����>--------------------//
void SpearEnemy::fThrustBeginInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_idle);
    mWaitTime = 0.0f;
}
void SpearEnemy::fThrustBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
   if(mWaitTime>=mThrustBeginSec)
   {
       fChangeState(DivedState::ThrustMiddle);
   }
}
//--------------------<����˂��o���A�j���[�V����>--------------------//
void SpearEnemy::fThrustMiddleInit()
{
    // �˂����̃A�j���[�V�������Đ�
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_up);
    mWaitTime = 0.0f;
}

void SpearEnemy::fThrustMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
    if(mWaitTime >= mThrustMiddleSec)
    {
        fChangeState(DivedState::ThrustEnd);
    }
}
//--------------------<�ːi��>--------------------//
void SpearEnemy::fThrustEndInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_down, true);
    mWaitTime = 0.0f;
}

void SpearEnemy::fThrustEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // ���ʕ����ɓˌ�����
    const auto V = forward * mThrustSpeed * elapsedTime_;
    mPosition += V;

    // �ːi���̓v���C���[�̕����Ɍy���z�[�~���O����
    fTurnToTarget(elapsedTime_, mPlayerPosition, 0.7f);

    mWaitTime += elapsedTime_;
    if(mWaitTime>=mThrustEndSec)
    {
        fChangeState(DivedState::Idle);
    }
}

void SpearEnemy::fDamageInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDamageUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

inline SpearEnemy::~SpearEnemy()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());
}

void SpearEnemy::fInitialize()
{
    
}
