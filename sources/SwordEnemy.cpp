#include"SwordEnemy.h"
#include"Operators.h"
SwordEnemy::SwordEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_sword.cereal")
{
    // �ʒu�̏�����
    mPosition = EmitterPoint_;
    mScale = { 0.05f,0.05f,0.05f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    // �X�e�[�g�}�V����������
    SwordEnemy::fRegisterFunctions();
    mParam.mHitPoint = 1;
}

void SwordEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_,Graphics_);
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
            fAttackInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackEnd, tuple));
    }
    fChangeState(DivedState::Start);
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
    auto moveVec = mPlayerPosition - mPosition;
    moveVec = Math::Normalize(moveVec);
    velocity = moveVec * mMoveSpeed;
    mPosition += velocity * elapsedTime_;

    //--------------------<�v���C���[�̕����ɉ�]>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition);
    
    // �v���C���[�Ƃ̋��������ȉ��ɂȂ�����
    if(mAttackRange >= Math::Length(mPlayerPosition-mPosition))
    {
        fChangeState(DivedState::AttackBegin);
    }
}

//--------------------<�U���͂���>--------------------//
void SwordEnemy::fAttackBeginInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::attack_idle);
}
void SwordEnemy::fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToTarget(elapsedTime_, mPlayerPosition, 20.0f);
    if(mpSkinnedMesh->end_of_animation())
    {
        fChangeState(DivedState::AttackMiddle);
    }
}

//--------------------<�U�����i�����蔻��ON�j>--------------------//
void SwordEnemy::fAttackInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::attack_down);
}
void SwordEnemy::fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

//--------------------<�U���I���i�㌄)>--------------------//
void SwordEnemy::fAttackEndInit()
{
    
}
void SwordEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}


void SwordEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SwordEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}
