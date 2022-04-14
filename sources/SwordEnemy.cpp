#include"SwordEnemy.h"
#include"Operators.h"
SwordEnemy::SwordEnemy(GraphicsPipeline& graphics_, 
    int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_,ParamGetFunction Func_ )
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_sword.fbx")
{
    // �ʒu�̏�����
    mPosition = EmitterPoint_;
    mScale = { 0.05f,0.05f,0.05f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    // �X�e�[�g�}�V����������
    SwordEnemy::fRegisterFunctions();
    mParam.mHitPoint = 100;
    fGetParam(this, Func_);

    // �U���Ɋւ���parameter��������
    mAttackInformation.mInvincible_time = { 0.2f };
     // �{�[����������
    mSwordBone = mpSkinnedMesh->get_bone_by_name("hand_r_joint");
}

void SwordEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_,Graphics_);
    fSetAttackCapsuleCollider(); // �U���p�̃J�v�Z���ʒu���X�V
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
