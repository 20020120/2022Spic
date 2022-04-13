#include"SpearEnemy.h"
#include"Operators.h"
SpearEnemy::SpearEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_spear.fbx")
{
   // �p����������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.05f,0.05f,0.05f };
    mParam.mHitPoint = 1;
    fRegisterFunctions();
}

void SpearEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_, Graphics_);
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
    fChangeState(DivedState::Idle);
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
{}

void SpearEnemy::fInitialize()
{
    
}
